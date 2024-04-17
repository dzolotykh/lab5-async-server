#include "Server.h"
#include <sys/poll.h>
#include <stdexcept>
#include <utility>
#include "Exceptions.h"
#include "handlers/EndpointHandler.h"
#include "log.h"

namespace Server {
void Server::use_logger(const std::string &message) {
    std::lock_guard<std::mutex> lock(logger_mtx);
    params.logger(message);
}

std::string Server::start_message() const {
    std::stringstream hello;
    hello << "\n✅ Сервер запущен. Параметры: \n";
    hello << "⚡️ Порт: " << params.port << "\n";
    hello << "⚡️ Максимальное количество подключений в очереди: " << params.max_connections_in_queue
          << "\n";
    hello << "⚡️ Максимальное количество одновременно работающих процессов: "
          << params.working_threads << "\n";
    return hello.str();
}

Server::Server(Params _params) : params(std::move(_params)), pool(params.working_threads) {}

void Server::set_nonblock(socket_t socket) {
    int status = fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
    if (status < 0) {
        throw std::runtime_error(ERROR_SET_NONBLOCK + std::to_string(errno));
    }
}

void Server::prepare_listener_socket() {
    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(params.port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    listener_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (listener_socket < 0) {
        throw std::runtime_error(ERROR_CREATE_SOCKET + std::to_string(errno));
    }

    int bind_status = bind(listener_socket, (sockaddr *)&socket_address, sizeof(socket_address));

    if (bind_status < 0) {
        throw std::runtime_error(ERROR_BIND + std::to_string(errno));
    }
    set_nonblock(listener_socket);
    if (listen(listener_socket, params.max_connections_in_queue) < 0) {
        throw std::runtime_error(ERROR_LISTEN + std::to_string(errno));
    }
}

std::string get_ip(socket_t socket) {
    auto *pV4Addr = (sockaddr_in *)&socket;
    in_addr ipAddr = pV4Addr->sin_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
    return {str};
}

/*
 * Функция process_listener принимает на вход структуру pollfd, которая содержит информацию о сокете-слушателе.
 * Если в структуре pollfd есть событие POLLERR, то функция бросает исключение с сообщением ERROR_POLL_LISTENER.
 * Если произошло событие POLLIN, то функция принимает новое подключение и добавляет его в вектор result.
 * В конце функция возвращает вектор c новыми подключениями.
 */
std::vector<socket_t> Server::process_listener(pollfd listener) {

    std::vector<socket_t> result;

    if (listener.revents & POLLERR) {
        throw std::runtime_error(ERROR_POLL_LISTENER + std::to_string(POLLERR));
    }

    sockaddr_in peer{};
    socklen_t peer_size = sizeof(peer);

    while (true) {
        socket_t channel = accept(listener_socket, (sockaddr *)&peer, &peer_size);
        if (channel < 0) {
            if (errno == EWOULDBLOCK) {
                break;
            } else {
                throw std::runtime_error(ERROR_ACCEPT + std::to_string(errno));
            }
        }

        changer_t changer = [this, channel](std::unique_ptr<AbstractHandler> handler) {
            client_handlers[channel] = std::move(handler);
        };

        client_handlers[channel] = std::make_unique<EndpointHandler>(endpoints, channel, changer);
        client_status[channel] = true;

        set_nonblock(channel);
        result.push_back({channel});
        use_logger("Создано подключение для нового клиента. IP: " + get_ip(channel));
    }
    return result;
}

bool Server::process_client(pollfd fd, socket_t client) {
    if (fd.revents & POLLERR) {
        throw SocketException("Ошибка при работе с сокетом: " + std::to_string(client));
    }
    return client_handlers[client]->operator()();
}

std::string prepare_response(const std::string &response) {
    int32_t response_size = response.size();
    std::string response_size_str(reinterpret_cast<char *>(&response_size), sizeof(response_size));
    response_size_str += response;
    return response_size_str;
}

void Server::process_all_clients(pollfds_iter pollfds_begin, pollfds_iter pollfds_end,
                                 sockets_iter sockets_begin, sockets_iter sockets_end) {
    if (pollfds_end - pollfds_begin != sockets_end - sockets_begin) {
        throw std::runtime_error("pollfds and sockets have different sizes");
    }
    auto pollfd_i = pollfds_begin;
    auto socket_i = sockets_begin;
    for (; pollfd_i != pollfds_end; pollfd_i++, socket_i++) {
        socket_t &client = *socket_i;
        pollfd &fd = *pollfd_i;
        try {
            if (!process_client(fd, client)) {
                auto response = client_handlers[client]->get_response();
                response = prepare_response(response);
                send(client, response.c_str(), response.size(), MSG_NOSIGNAL);
                shutdown(client, SHUT_RDWR);
                close(client);
                client_handlers.erase(client);
                use_logger("Клиент отключен: " + std::to_string(client));
                client = -1;
            }
        } catch (const std::exception &e) {
            use_logger("Ошибка при обработке клиента: " + std::string(e.what()));
            auto response = prepare_response(INTERNAL_ERROR_TEXT);
            send(client, response.c_str(), response.size(), MSG_NOSIGNAL);
            shutdown(client, SHUT_RDWR);
            close(client);
            client_handlers.erase(client);
            client = -1;
        }
    }
}

void Server::start() {
    prepare_listener_socket();
    polling_wrapper = PollingWrapper(listener_socket);
    use_logger(start_message());

    while (is_running) {
        auto [connections, pollfds] = polling_wrapper.get();
        poll(pollfds.data(), pollfds.size(), -1);
        // Проверяем, есть ли новые подключения
        auto new_connections = process_listener(pollfds.back());
        // Обрабатываем все старые подключения

        size_t clients_per_thread =
            (connections.size() + params.working_threads - 1) / params.working_threads;

        for (size_t i = 0; i + 1 < pollfds.size(); i += clients_per_thread) {
            auto connections_begin = connections.begin() + i;
            auto connections_end =
                std::min(connections_begin + clients_per_thread, connections.end() - 1);
            auto pollfds_begin = pollfds.begin() + i;
            auto pollfds_end = std::min(pollfds_begin + clients_per_thread, pollfds.end() - 1);
            pool.add_task([this, connections_begin, connections_end, pollfds_begin, pollfds_end]() {
                process_all_clients(pollfds_begin, pollfds_end, connections_begin, connections_end);
            });
        }

        pool.wait_all();

        // process_all_clients(pollfds.begin(), pollfds.end() - 1, connections.begin(), connections.end() - 1);

        polling_wrapper = PollingWrapper(connections, pollfds);
        polling_wrapper.remove_disconnected();

        polling_wrapper.add_connection(new_connections);
    }
}

void Server::stop() {
    close(listener_socket);
    is_running = false;
}

Server::~Server() = default;
}    // namespace Server