#include "ResultRequestHandler.h"

Server::ResultRequestHandler::ResultRequestHandler(Server::socket_t _client,
                                                   Database::ConnectionPool &_pool)
    : pool(_pool), client(_client), buff(32),
    token_reader(AbstractHandler::read_bytes_nonblock(_client, 32, buff.data(), 32, [](size_t){})) {}

std::string Server::ResultRequestHandler::save_request_to_db() {
    auto conn = pool.get_connection();
    pqxx::work w(conn.get_connection());
    pqxx::result res = w.exec_params(token_query, std::string_view{buff.data(), buff.size()});
    if (res.empty()) {
        throw NotFoundException("Токен не найден в базе данных.");
    }
    if (res.size() != 1) {
        throw std::runtime_error("Найдено несколько запросов с одинаковым токеном.");
    }
    w.commit();
    return res[0][3].as<std::string>();
}

std::string Server::ResultRequestHandler::get_response() {
    return response;
}

bool Server::ResultRequestHandler::operator()() {
    try {
        if (token_reader()) {
            return true;
        }
        response = "OK|" + save_request_to_db();
        return false;
    } catch (const BadInputException& e) {
        response = "ERROR|Wrong token.";
        return false;
    } catch (const NotFoundException& e) {
        response = "ERROR|Wrong token.";
        return false;
    }
}
