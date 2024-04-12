#include "FileDownloadHandler.h"

Server::FileDownloadHandler::FileDownloadHandler(Server::socket_t _client,
                                                 Database::ConnectionPool &_pool)
    : client(_client), pool(_pool) {}

bool Server::FileDownloadHandler::operator()() {
    switch (state) {
        case State::READING_TOKEN:
            read_token();
            break;
        case State::SENDING_FILE:
            send_file();
            break;
        case State::FINISHED:
            return false;
        case State::ERROR:
            return false;
    }
    return true;
}

Server::AbstractHandler::Result Server::FileDownloadHandler::get_result() {
    switch (state) {
        case State::FINISHED:
            return Result::OK;
        case State::ERROR:
            return Result::ERROR;
        default:
            return Result::PROCESSING;
    }
}

std::string Server::FileDownloadHandler::get_response() {
    return "";
}

void Server::FileDownloadHandler::read_token() {
    if (state != State::READING_TOKEN) {
        return;
    }
    ssize_t read = recv(client, token.data(), token.size(), MSG_DONTWAIT);
    if (read == -1 && errno == EAGAIN) {
        return;
    } else if (read == -1) {
        state = State::ERROR;
        throw SocketException("Ошибка при чтении токена.");
    } else if (read == 0) {
        state = State::ERROR;
        throw BadInputException(
            "Клиент отключился, не передав токен, либо переданный токен слишком короткий.");
    }
    bytes_read += read;
    if (bytes_read < token.size()) {
        return;
    }
    bytes_read = 0;
    state = State::SENDING_FILE;
}

void Server::FileDownloadHandler::set_path(const std::string &token) {
    if (filepath.has_value()) {
        return;
    }
    Database::Connection conn = pool.get_connection();
    pqxx::work w(conn.get_connection());
    pqxx::result res = w.exec_params(token_query, token);
    if (res.empty()) {
        state = State::ERROR;
        throw NotFoundException("Токен не найден в базе данных.");
    }
    if (res.size() != 1) {
        throw std::runtime_error("Найдено несколько файлов с одинаковым токеном.");
    }
    std::filesystem::path path = res[0][1].as<std::string>();
    w.commit();
    filepath = path;
    file_size = res[0][2].as<std::size_t>();
}

#include <iostream>

// TODO надо переписать так, чтобы к базе данных обращаться только один раз
// при этом где-то хранить размер файла
// а пока я иду кайфовать

void Server::FileDownloadHandler::send_file() {
    if (state != State::SENDING_FILE) {
        return;
    }
    if (filepath == std::nullopt)
        set_path(std::string(token.begin(), token.end()));
    if (!filepath.has_value()) {
        state = State::ERROR;
        throw NotFoundException("Файл не найден в базе данных.");
    }
    std::ifstream file(filepath.value(), std::ios::binary);
    if (!file.is_open()) {
        state = State::ERROR;
        throw std::runtime_error("Не удалось открыть файл для чтения. Имя файла: " +
                                 filepath.value().string());
    }
    file.seekg(bytes_sent, std::ios::beg);
    file.read(write_buffer.data(), write_buffer.size());
    ssize_t sent = send(client, write_buffer.data(), file.gcount(), MSG_DONTWAIT);
    if (sent == -1 && errno == EAGAIN) {
        return;
    } else if (sent == -1) {
        state = State::ERROR;
        throw SocketException("Ошибка при отправке файла.");
    } else if (sent == 0) {
        state = State::ERROR;
        throw std::runtime_error("Клиент отключился во время отправки файла.");
    }
    bytes_sent += sent;
    if (bytes_sent == file_size) {
        state = State::FINISHED;
    }
}
