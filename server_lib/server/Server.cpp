#include "Server.h"

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

Server::Server(Params _params) : params(std::move(_params)), pool(params.working_threads), listener_socket() {}

std::string get_ip(const Socket& socket) {
    auto fd = socket.get_fd();
    auto *pV4Addr = (sockaddr_in *)&fd;
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
void Server::process_listener(pollfd listener) {
    if (listener.revents & POLLERR) {
        if (!is_running)
            throw std::runtime_error(ERROR_POLL_LISTENER + std::to_string(POLLERR));
        else
            return;
    }

    sockaddr_in peer{};
    socklen_t peer_size = sizeof(peer);

    while (true) {
        clients.emplace_back(std::make_unique<Socket>(accept(listener_socket.get_fd(), (sockaddr *)&peer, &peer_size)));
        Socket& client = *clients.back();
        if (client.get_fd() < 0) {
            clients.pop_back();
            if (errno == EWOULDBLOCK) {
                break;
            } else {
                throw std::runtime_error(ERROR_ACCEPT + std::to_string(errno));
            }
        }
        changer_t changer = [this, channel = client.get_fd()](std::unique_ptr<AbstractHandler> handler) {
            client_handlers[channel] = std::move(handler);
        };
        client_handlers[client.get_fd()] = std::make_unique<EndpointHandler>(endpoints, client, changer);
        client_status[client.get_fd()] = true;
        client.set_attribute(Socket::attributes::NONBLOCK);
        use_logger("Создано подключение для нового клиента. IP: " + get_ip(client) + "Socket: " + std::to_string(client.get_fd()));
    }
}

bool Server::process_client(pollfd fd, const Socket& client) {

    if (fd.revents & POLLERR) {
        throw SocketException("Ошибка при работе с сокетом: " + std::to_string(client.get_fd()));
    }

    if (fd.revents & POLLHUP) {
        return false;
    }

    return client_handlers[client.get_fd()]->operator()();
}

std::string prepare_response(const std::string &response) {
    auto response_size = static_cast<int32_t>(response.size()); // размер ответа должен влезать в int32
    std::string response_size_str(reinterpret_cast<char*>(&response_size), sizeof(response_size));
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
        const Socket &client = **socket_i;
        pollfd &fd = *pollfd_i;
        try {
            if (!process_client(fd, client)) {
                auto response = client_handlers[client.get_fd()]->get_response();
                response = prepare_response(response);
                send(client.get_fd(), response.c_str(), response.size(), MSG_NOSIGNAL);
                client_handlers.erase(client.get_fd());
                use_logger("Клиент отключился. IP: " + get_ip(client));
            }
        } catch (const std::exception &e) {
            use_logger("Ошибка при обработке клиента: " + std::string(e.what()));
            auto response = prepare_response(INTERNAL_ERROR_TEXT);
            send(client.get_fd(), response.c_str(), response.size(), MSG_NOSIGNAL);
            client_handlers.erase(client.get_fd());
            use_logger("Клиент отключился. IP: " + get_ip(client));
        }
    }
}

std::vector<pollfd> Server::generate_pollfds() {
    std::vector<pollfd> pollfds;
    for (const auto &client : clients) {
        pollfd client_pollfd{};
        client_pollfd.fd = client->get_fd();
        client_pollfd.events = POLLIN | POLLERR | POLLHUP;
        pollfds.push_back(client_pollfd);
    }
    pollfd listener_pollfd{};
    listener_pollfd.fd = listener_socket.get_fd();
    listener_pollfd.events = POLLIN;
    pollfds.push_back(listener_pollfd);

    return pollfds;
}

void Server::start() {
    listener_socket = Socket::create_listener_socket(params.port, params.max_connections_in_queue);
    listener_socket.set_attribute(Socket::attributes::NONBLOCK);
    use_logger(start_message());

    while (is_running) {
        auto pollfds = generate_pollfds();
        poll(pollfds.data(), pollfds.size(), -1);
        // Проверяем, есть ли новые подключения
        pollfd listener_pollfd = pollfds.back();
        pollfds.pop_back();

        process_listener(listener_pollfd);
        // Обрабатываем все старые подключения

        size_t clients_per_thread = (clients.size() + params.working_threads - 1) / params.working_threads;

        for (size_t i = 0; i < pollfds.size(); i += clients_per_thread) {
            auto connections_begin = clients.begin() + i;
            auto connections_end = std::min(connections_begin + clients_per_thread, clients.end());
            auto pollfds_begin = pollfds.begin() + i;
            auto pollfds_end = std::min(pollfds_begin + clients_per_thread, pollfds.end());
            pool.add_task([this, connections_begin, connections_end, pollfds_begin, pollfds_end]() {
                process_all_clients(pollfds_begin, pollfds_end, connections_begin, connections_end);
            });

        }

        pool.wait_all();

        clients.erase(std::remove_if(clients.begin(), clients.end(), [this](const auto& elem){
            return client_handlers.count(elem->get_fd()) == 0;
        }), clients.end());

    }
}

void Server::stop() {
    is_running = false;
}

Server::~Server() {
    stop();
}

void Server::stop(std::chrono::milliseconds timeout) {
    is_running = false;
    std::this_thread::sleep_for(timeout);
}
}    // namespace Server