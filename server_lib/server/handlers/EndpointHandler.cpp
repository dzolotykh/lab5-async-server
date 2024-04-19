#include "EndpointHandler.h"

#include <iostream>

bool Server::EndpointHandler::operator()() {

    if (reader == nullptr) {
        reader = read_bytes_nonblock(client, sizeof(endpoint), &endpoint, sizeof(endpoint),
                                     [this](size_t) {});
    }

    if (!reader()) {
        if (handlers.find(endpoint) == handlers.end()) {
            response = "ERROR|Unknown endpoint.";
            return false;
        }
        auto handler = handlers[endpoint](client);
        change_handler(std::move(handler));
    }
    return true;
}

std::string Server::EndpointHandler::get_response() {
    return response;
}

Server::EndpointHandler::EndpointHandler(std::unordered_map<char, handler_provider_t>& _handlers,
                                         const Socket& _client, changer_t _change_handler)
    : handlers(_handlers), client(_client), change_handler(std::move(_change_handler)) {

}