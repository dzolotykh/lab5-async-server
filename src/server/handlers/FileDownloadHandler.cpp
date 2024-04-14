#include "FileDownloadHandler.h"

#include <iostream>

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

std::string Server::FileDownloadHandler::get_response() {
    return "";
}

void Server::FileDownloadHandler::read_token() {
    if (state != State::READING_TOKEN) {
        return;
    }
    read_bytes(client, token.size(), token.data());
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
    source_file.open(filepath.value(), std::ios::binary);
    if (!source_file.is_open()) {
        state = State::ERROR;
        throw std::runtime_error("Не удалось открыть файл для чтения. Имя файла: " +
                                 filepath.value().string());
    }

    if (!writer.has_value()) {
        writer = write_bytes_nonblock(client, file_size, [this]() {
            source_file.seekg(bytes_sent, std::ios::beg);
            source_file.read(write_buffer.data(), write_buffer.size());
            bytes_sent += source_file.gcount();
            return std::make_pair(write_buffer.data(), source_file.gcount());
        });
    }
    bool need_continue = writer.value()();
    if (!need_continue) {
        state = State::FINISHED;
    }
}
