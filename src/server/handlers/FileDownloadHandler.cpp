#include "FileDownloadHandler.h"

#include <iostream>

Server::FileDownloadHandler::FileDownloadHandler(Server::socket_t _client,
                                                 Database::ConnectionPool& _pool)
    : client(_client),
      pool(_pool),
      write_buffer(1024 * 1024 * 5),
      token_buffer(32),
      token_reader(AbstractHandler::read_bytes_nonblock(
          _client, 32, token_buffer.data(), 32, [this](size_t read) {
              token.insert(token.end(), token_buffer.begin(), token_buffer.begin() + read);
          })) {}

bool Server::FileDownloadHandler::operator()() {
    if (state == State::FINISHED) {
        return false;
    }
    if (state == State::TOKEN) {
        try {
            if (token_reader()) {
                return true;
            }
            state = State::FILE;
            set_path();
            auto bytes_provider = [path = filepath, size = file_size, bytes_provided = (size_t)0,
                                   buff = write_buffer.data(), buff_size = write_buffer.size(),
                                   pref = std::string()]() mutable {

                // заранее откроем файл, чтобы не возвращать OK если он не открылся
                std::ifstream file(path, std::ios::binary);
                if (!file.is_open()) {
                    throw std::runtime_error("Не удалось открыть файл для чтения. Имя файла: " +
                                             path.string());
                }

                // Если это первый вызов, то отправляем префикс.
                // Это сильно портит протокол взаимодействия, ведь мы ожидаем что полезная нагрузка будет в
                // методе get_response(), но у нас нет возможности возвращать данные в get_response() по кускам.
                // возможно, имеет смысл сделать метод get_response_parted(), который будет возвращать результат по частям.
                if (pref.empty()) {
                    pref = "OK|";
                    int32_t response_size = pref.size() + size;
                    pref = std::string(reinterpret_cast<char*>(&response_size), 4) + pref;
                    return std::make_pair(pref.data(), pref.size());
                }
                file.seekg(bytes_provided, std::ios::beg);
                file.read(buff, buff_size);
                bytes_provided += file.gcount();
                return std::make_pair(buff, static_cast<size_t>(file.gcount()));
            };

            // 7 дополнительных байт выделяется для того, чтобы отправить размер ответа вместе с префиксом
            writer = AbstractHandler::write_bytes_nonblock(client, file_size + 7, bytes_provider);
            return true;
        } catch (const BadInputException& e) {
            response = "ERROR|Wrong token.";
            state = State::FINISHED;
            return false;
        } catch (const TokenNotFoundException& e) {
            response = "ERROR|Wrong token.";
            state = State::FINISHED;
            return false;
        } catch (const FileNotFoundException& e) {
            response = "ERROR|File not found.";
            state = State::FINISHED;
            return false;
        } catch (const std::exception& e) {
            response = "ERROR|Internal server error.";
            state = State::FINISHED;
            return false;
        }
    }
    bool need_continue = writer();
    if (!need_continue) {
        state = State::FINISHED;
    }
    return need_continue;
}

std::string Server::FileDownloadHandler::get_response() {
    return response;
}

void Server::FileDownloadHandler::set_path() {
    Database::Connection conn = pool.get_connection();
    pqxx::work w(conn.get_connection());
    pqxx::result res = w.exec_params(token_query, token);
    if (res.empty()) {
        throw TokenNotFoundException("Токен не найден в базе данных.");
    }
    if (res.size() != 1) {
        throw std::runtime_error("Найдено несколько файлов с одинаковым токеном.");
    }
    if (res[0][0].is_null()) {
        throw FileNotFoundException("Файл не найден.");
    }
    std::filesystem::path path = res[0][0].as<std::string>();
    w.commit();
    filepath = path;
    file_size = std::filesystem::file_size(path);
}
