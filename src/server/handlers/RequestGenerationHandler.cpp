#include "RequestGenerationHandler.h"

Server::RequestGenerationHandler::RequestGenerationHandler(Server::socket_t _client,
                                                           Database::ConnectionPool &_pool)
    : client(_client), pool(_pool) {}

std::string Server::RequestGenerationHandler::get_response() {
    return response;
}

Server::AbstractHandler::Result Server::RequestGenerationHandler::get_result() {
    return result;
}

bool Server::RequestGenerationHandler::operator()() {
    if (result != Result::PROCESSING) {
        return false;
    }
    try {
        read_bytes(client, buff.size(), buff.data());
    } catch (const SocketException &e) {
        result = Result::ERROR;
        response = "ERROR@Socket error.";
        return false;
    } catch (const BadInputException &e) {
        result = Result::ERROR;
        response = "ERROR@Wrong token.";
        return false;
    }
    std::string_view token(buff.data(), buff.size());
    auto conn = pool.get_connection();
    pqxx::work w(conn.get_connection());
    pqxx::result r = w.exec_params(input_data_token_query, token);
    if (r.empty()) {
        result = Result::ERROR;
        response = "ERROR@Wrong token.";
        throw NotFoundException("Токен не найден в базе данных.");
    }
    if (r.size() > 1) {
        result = Result::ERROR;
        response = "ERROR@Intrnal error: more than one token found.";
        throw std::runtime_error("Найдено более одного токена в базе данных.");
    }
    auto input_path = r[0][1].as<std::string>();
    w.exec_params(input_data_query, token, input_path);
    w.commit();
    response = "OK@";
    response += token;
    result = Result::OK;
    return false;
}
