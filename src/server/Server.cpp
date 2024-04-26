#include <server/Server.h>
#include "exceptions/SocketExceptions.h"
#include <iostream>

Server::Server::Server(uint16_t port, int max_connections): listener_socket(port, max_connections) {

}

Server::Server::~Server() {
    stop();
}

void Server::Server::start() {
    while (true) {
        auto client = listener_socket.accept_client();
        handle_client(client);
    }
}

Server::Server::Server(Server &&other) noexcept : listener_socket(std::move(other.listener_socket)) {
    endpoints = std::move(other.endpoints);
}

Server::Server &Server::Server::operator=(Server &&other) noexcept {
    Server tmp(std::move(other));
    std::swap(listener_socket, tmp.listener_socket);
    std::swap(endpoints, tmp.endpoints);
    return *this;
}

void Server::Server::handle_client(const ClientSocket& client) {
    int8_t endpoint_byte = client.read_byte();
    if (endpoints.find(endpoint_byte) == endpoints.end()) {
        throw Exceptions::EndpointNotFoundException(endpoint_byte);
    }
    auto handler = endpoints[endpoint_byte](client);
    try {
        auto response = handler->handle();
        client.send_bytes(response.message);
    } catch (Exceptions::ClientDisconnectedException& e) {
        // Do nothing
    } catch (Exceptions::SocketException& e) {

    }
}

void Server::Server::stop() {

}



