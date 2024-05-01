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
    auto& logger = *Logging::Logger::get_instance();
    while (is_running) {
        auto client = listener_socket.accept_client(1);
        if (client == nullptr) {
            continue;
        }
        clients.push_back(tp.add_task([this, client, &logger]() {
            try {
                handle_client(*client);
            } catch (const std::exception& err) {
                logger << "Ошибка при работе с пользователем " + client->get_info() + ": " +
                              err.what()
                       << "\n";
            }
            logger << "Пользователь " + client->get_info() + " отключился\n";
        }));
    }
    logger << "Ожидание завершения всех клиентов\n";
    for (auto& client : clients) {
        try {
            client.get();
        } catch (const std::exception& err) {
            logger << "Ошибка при работе с пользователем: " + std::string(err.what()) << "\n";
        }
    }
    logger << "Все клиенты отключились\n";
}

void Server::Server::handle_client(const ClientSocket& client) {
    auto& logger = *Logging::Logger::get_instance();
    logger << "Пользователь " + client.get_info() + " подключился\n";
    char endpoint_byte = client.read_byte();
    if (endpoints.find(endpoint_byte) == endpoints.end()) {
        logger << "Пользователь " + client.get_info() + " отправил неизвестный запрос\n";
        client.send_bytes("Unknown endpoint");
        return;
    }
    auto handler = endpoints[endpoint_byte](client);
    logger << "Пользователь " + client.get_info() + " отправил запрос на эндпоинт " +
                  endpoint_byte + "\n";
    try {
        auto response = handler->handle();
        client.send_bytes(response.message);
    } catch (Exceptions::ClientDisconnectedException& e) {
        logger << "Пользователь " + client.get_info() + " отключился\n";
    } catch (Exceptions::SocketException& e) {
        logger << "Ошибка при работе с пользователем " + client.get_info() + ": " + e.what()
               << "\n";
    }
}

void Server::Server::stop() {
    is_running = false;
}
