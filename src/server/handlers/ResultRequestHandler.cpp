#include "ResultRequestHandler.h"
#include "../Exceptions.h"

Server::ResultRequestHandler::ResultRequestHandler(Server::socket_t _client,
                                                   Database::ConnectionPool &_pool)
    : pool(_pool), client(_client) {}

bool Server::ResultRequestHandler::read_token() {
    if (state != State::READING_TOKEN) {
        return false;
    }
    ssize_t read = recv(client, buff.data() + bytes_read, buff.size() - bytes_read, MSG_DONTWAIT);
    if (read == -1 && errno == EAGAIN) {    // пока нет данных
        return true;
    } else if (read == -1) {
        state = State::ERROR;
        throw SocketException("Ошибка при чтении токена.");
    } else if (read == 0) {
        state = State::ERROR;
        throw BadInputException(
            "Клиент отключился, не передав токен, либо переданный токен слишком короткий.");
    }
    bytes_read += read;
    if (bytes_read < buff.size()) {
        return true;
    }
    generation_token = std::string(buff.begin(), buff.end());
    state = State::PROCESSING;
    return false;
}

#include <iostream>

bool Server::ResultRequestHandler::save_request_to_db() {
    if (state != State::PROCESSING) {
        return false;
    }
    auto conn = pool.get_connection();
    pqxx::work w(conn.get_connection());
    pqxx::result res = w.exec_params(token_query, generation_token);
    if (res.empty()) {
        state = State::ERROR;
        throw NotFoundException("Токен не найден в базе данных.");
    }
    if (res.size() != 1) {
        state = State::ERROR;
        throw std::runtime_error("Найдено несколько запросов с одинаковым токеном.");
    }

    response = res[0][3].as<std::string>();

    w.commit();
    state = State::FINISHED;
    return false;
}

std::string Server::ResultRequestHandler::get_response() {
    return response;
}

Server::AbstractHandler::Result Server::ResultRequestHandler::get_result() {
    switch (state) {
        case State::ERROR:
            return Result::ERROR;
        case State::FINISHED:
            return Result::OK;
        default:
            return Result::PROCESSING;
    }
}

bool Server::ResultRequestHandler::operator()() {
    switch (state) {
        case State::READING_TOKEN:
            return read_token();
        case State::PROCESSING:
            return save_request_to_db();
        case State::ERROR:
            return false;
        case State::FINISHED:
            return false;
    }
}
