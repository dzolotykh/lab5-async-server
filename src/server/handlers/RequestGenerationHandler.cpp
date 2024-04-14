#include "RequestGenerationHandler.h"

Server::RequestGenerationHandler::RequestGenerationHandler(Server::socket_t _client,
                                                           Database::ConnectionPool &_pool)
    : client(_client), pool(_pool) {}

std::string Server::RequestGenerationHandler::get_response() {
    return response;
}

bool Server::RequestGenerationHandler::operator()() {
    try {
        read_bytes(client, buff.size(), buff.data());
    } catch (const BadInputException &e) {
        response = "ERROR|Wrong token.";
        return false;
    }
    std::string_view token(buff.data(), buff.size());
    auto conn = pool.get_connection();
    pqxx::work w(conn.get_connection());
    pqxx::result r = w.exec_params(input_data_token_query, token);
    if (r.empty()) {
        response = "ERROR|Wrong token.";
        return false;
    }
    if (r.size() > 1) {
        response = "ERROR|Intrnal error: more than one token found.";
        return false;
    }
    auto input_path = r[0][1].as<std::string>();
    w.exec_params(input_data_query, token, input_path);
    w.commit();
    response = "OK|";
    response += token;
    return false;
}
