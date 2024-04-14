#include "FileUploadHandler.h"

void Server::FileUploadHandler::generate_filename() {
    do {
        filepath = std::filesystem::current_path() / (StringUtils::random_string(32) + ".txt");
    } while (std::filesystem::exists(filepath));
}

#include <iostream>
#include <utility>

Server::FileUploadHandler::FileUploadHandler(socket_t client, Database::ConnectionPool& _pool,
                                             std::filesystem::path _save_path)
    : client(client),
      pool(_pool),
      file_size(0),
      save_path(std::move(_save_path)),
      token(StringUtils::random_string(32)) {
    std::filesystem::current_path(save_path);
    generate_filename();
}

bool Server::FileUploadHandler::read_file_size() {
    if (state != State::FILE_SIZE) {
        return false;
    }

    if (token_reader == nullptr) {
        token_reader =
            read_bytes_nonblock(client, header_size, buffer.data(), buffer.size(), [](size_t) {});
    }
    if (token_reader()) {
        return true;
    }

    file_size = *reinterpret_cast<int32_t*>(buffer.data());

    if (file_size <= 0) {
        state = State::ERROR;
        response = "ERROR|Bad input. File size must be positive.";
        throw BadInputException("Размер файла не может быть отрицательным или равным нулю.");
    }

    state = State::FILE_CONTENT;
    return true;
}

void Server::FileUploadHandler::save_file_to_db(const std::string& token) {
    auto conn = pool.get_connection();
    pqxx::work w(conn.get_connection());
    w.exec("INSERT INTO files (filepath, size, token) VALUES ('" + filepath.string() + "', " +
           std::to_string(file_size) + ", '" + token + "')");
    w.commit();
}

bool Server::FileUploadHandler::read_file_content() {
    if (state != State::FILE_CONTENT) {
        return false;
    }
    file.open(filepath, std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        state = State::ERROR;
        response = "ERROR|Internal server error.";
        throw std::runtime_error("Не удалось открыть файл для записи. Имя файла: " +
                                 filepath.string());
    }

    if (file_reader == nullptr) {
        file_reader = read_bytes_nonblock(
            client, file_size, buffer.data(), buffer.size(),
            [this](size_t need_write) { file.write(buffer.data(), need_write); });
    }

    try {
        bool need_continue = file_reader();
        file.close();
        if (!need_continue) {
            save_file_to_db(token);
            response = "OK|" + token + "|" + filepath.string();
            state = State::FINISHED;
        }
        std::cout << need_continue << std::endl;
        return need_continue;
    } catch (BadInputException& e) {
        std::filesystem::remove(filepath);
        state = State::ERROR;
        response = "ERROR|Bad input. " + std::string(e.what());
        return false;
    } catch (std::exception& e) {
        std::filesystem::remove(filepath);
        state = State::ERROR;
        throw;
    }
}

bool Server::FileUploadHandler::operator()() {
    if (state == State::FINISHED || state == State::ERROR) {
        return false;
    } else if (state == State::FILE_SIZE) {
        return read_file_size();
    } else {
        return read_file_content();
    }
}

std::string Server::FileUploadHandler::get_response() {
    return response;
}
