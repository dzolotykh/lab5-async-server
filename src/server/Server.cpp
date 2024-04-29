#include <server/Server.h>
#include "exceptions/SocketExceptions.h"
#include <iostream>
#include <csignal>

Server::Server::Server(uint16_t port, int max_connections, int _tp_size): listener_socket(port, max_connections), tp(_tp_size) {
    signal(SIGPIPE, SIG_IGN);
}

Server::Server::~Server() {
    stop();
}

void Server::Server::start() {
    while (true) {
        auto client = listener_socket.accept_client();
        tp.add_task([this, client]() {
            try {
                handle_client(*client);
            } catch (const std::exception& err) {
                std::cout << err.what() << std::endl;
            }
            std::cout << "Пользователь на сокете " << client->get_fd() << " обработан." << std::endl;
        });
    }
}

void Server::Server::handle_client(const ClientSocket& client) {
    std::cout << "Handling client: " << client.get_fd() << "\n";
    char endpoint_byte = client.read_byte();
    if (endpoints.find(endpoint_byte) == endpoints.end()) {
        client.send_bytes("Unknown endpoint");
        return;
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



