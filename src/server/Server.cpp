#include <server/Server.h>
#include <csignal>
#include <iostream>
#include "exceptions/SocketExceptions.h"

Server::Server::Server(uint16_t port, int max_connections, int _tp_size)
    : listener_socket(port, max_connections), tp(_tp_size) {
    signal(SIGPIPE, SIG_IGN);
}

Server::Server::~Server() {
    stop();
}

void Server::Server::start() {
    std::vector<std::future<void>> clients;
    while (is_running) {
        auto client = listener_socket.accept_client(1);
        if (client == nullptr) {
            continue;
        }
        clients.push_back(tp.add_task([this, client]() {
            try {
                handle_client(*client);
            } catch (const std::exception& err) {
                std::cout << err.what() << std::endl;
            }
            std::cout << "Пользователь на сокете " << client->get_fd() << " обработан."
                      << std::endl;
        }));
    }
    std::cout << "Ожидание завершения всех клиентов..." << std::endl;
    for (auto& client : clients) {
        client.get();
    }
    std::cout << "Сервер завершил выполнение" << std::endl;
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
    } catch (Exceptions::SocketException& e) {}
}

void Server::Server::stop() {
    is_running = false;
}
