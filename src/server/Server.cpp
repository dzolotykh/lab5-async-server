#include "Server.h"
#include <sys/poll.h>
#include <stdexcept>
#include <utility>
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
    hello << "⚡️ Максимальное количество одновременно работающих процессов: " << params.max_process
          << "\n";
    return hello.str();
}

Server::Server(ServerParams _params) : params(std::move(_params)) {}

void Server::set_nonblock(socket_t socket) {
    int status = fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
    if (status < 0) {
        throw std::runtime_error(ERROR_SET_NONBLOCK + std::to_string(errno));
    }
}

void Server::prepare_listener_socket() {
    set_nonblock(listener_socket);

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
    if (listen(listener_socket, params.max_connections_in_queue) < 0) {
        throw std::runtime_error(ERROR_LISTEN + std::to_string(errno));
    }
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

    // эту часть надо переписать: в revents хранится не количество событий, а их битовая маска
    for (int i = 0; i < listener.revents; ++i) {
        sockaddr_in peer{};
        socklen_t peer_size = sizeof(peer);
        socket_t channel = accept(listener_socket, (sockaddr *)&peer, &peer_size);
        if (channel < 0) {
            if (errno == EWOULDBLOCK) {
                break;
            } else {
                throw std::runtime_error(ERROR_ACCEPT + std::to_string(errno));
            }
        }
        set_nonblock(channel);
        result.push_back({channel});
        use_logger("Создано подключение для нового клиента.");
    }
    return result;
}

std::string Server::read_from_socket(socket_t socket) {
    const size_t buff_size = 1024;
    static_assert(buff_size > 2);
    char buff[buff_size];
    std::string data;
    bool f = false;
    while (true) {
        memset(buff, 0, buff_size);    // clear buffer (fill with zeros
        ssize_t bytes_read =
            recv(socket, buff, buff_size - 2, MSG_DONTWAIT);    // leave space for null terminator
        data.append(buff);                                      // add buffer to data

        if (bytes_read <= 0) {
            // Если нет данных для чтения или произошла ошибка, выходим из цикла
            break;
        }
        if (!f) {
            f = true;
            std::string HTTP_ANS = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
            send(socket, HTTP_ANS.c_str(), HTTP_ANS.size(), MSG_NOSIGNAL);
        }
        send(socket, buff, bytes_read, MSG_NOSIGNAL);    // echo back
    }
    shutdown(socket, SHUT_RDWR);
    close(socket);

    return data;
}

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return (!std::isspace(ch) && ch != '\0' && ch != '\n' && ch != '\r');
            }));
}

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char ch) {
                             return (!std::isspace(ch) && ch != '\0' && ch != '\n' && ch != '\r');
                         })
                .base(),
            s.end());
}

void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

bool Server::process_client(pollfd fd, socket_t client) {
    if (fd.revents & POLLERR) {
        return false;
    }
    if (fd.revents & POLLHUP) {
        use_logger("Пользователь отключился.");
        close(client);
        return false;
    }
    if (fd.revents & POLLIN) {
        std::string data = read_from_socket(client);
        if (data.empty()) {
            return false;
        }
        trim(data);

        use_logger("Получены данные: " + data);
        return true;
    }
    return true;
}

void Server::start() {
    prepare_listener_socket();

    polling_wrapper = PollingWrapper(listener_socket);

    use_logger(start_message());

    while (true) {
        auto [connections, pollfds] = polling_wrapper.get();
        poll(pollfds.data(), pollfds.size(), 1000);
        // Проверяем, есть ли новые подключения
        auto new_connections = process_listener(pollfds.back());
        // Обрабатываем все старые подключения (пока что просто проверим, что они
        // живы)
        for (size_t i = 0; i + 1 < connections.size(); ++i) {
            bool result = process_client(pollfds[i], connections[i]);
            if (!result) {
                connections[i] = -1;
            }
        }
        polling_wrapper = PollingWrapper(connections, pollfds);
        polling_wrapper.remove_disconnected();
        polling_wrapper.add_connection(new_connections);
    }
}

Server::~Server() {}
}    // namespace Server