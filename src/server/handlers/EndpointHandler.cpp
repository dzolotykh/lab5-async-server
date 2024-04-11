#include "EndpointHandler.h"

bool Server::EndpointHandler::operator()() {
    char endpoint;
    ssize_t read = recv(client, &endpoint, sizeof(endpoint), MSG_DONTWAIT);
    if (read == -1 && errno == EAGAIN) {
        return true;
    } else if (read == -1) {
        result = Result::ERROR;
        throw std::runtime_error("Ошибка при чтении эндпоинта. Код ошибки: " +
                                 std::to_string(errno));
    }
    if (handlers.find(endpoint) == handlers.end()) {
        result = Result::ERROR;
        throw std::runtime_error("Неизвестный эндпоинт: " + std::to_string(endpoint));
    }
    auto handler = handlers[endpoint](client);
    change_handler(std::move(handler));
    result = Result::OK;
    return false;
}

std::string Server::EndpointHandler::get_response() {
    return "";
}

Server::AbstractHandler::Result Server::EndpointHandler::get_result() {
    return result;
}

Server::EndpointHandler::EndpointHandler(std::unordered_map<char, handler_provider_t>& _handlers,
                                         socket_t _client, changer_t _change_handler)
    : handlers(_handlers), client(_client), change_handler(std::move(_change_handler)) {}