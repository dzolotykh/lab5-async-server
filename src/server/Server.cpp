#include "Server.h"
#include "log.h"
#include <stdexcept>
#include <utility>
#include <sys/poll.h>

void Server::use_logger(const std::string& message)
{
    std::lock_guard<std::mutex> lock(logger_mtx);
    params.logger(message);
}

std::string Server::start_message() const
{
    std::stringstream hello;
    hello << "\n✅ Сервер запущен. Параметры: \n";
    hello << "⚡️ Порт: " << params.port << "\n";
    hello << "⚡️ Максимальное количество подключений в очереди: " << params.max_connections_in_queue
          << "\n";
    hello << "⚡️ Максимальное количество одновременно работающих процессов: " << params.max_process
          << "\n";
    return hello.str();
}

Server::Server(ServerParams  _params)
    : params(std::move(_params))
{
}

void Server::set_nonblock(Server::socket_t socket)
{
    int status = fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
    if (status < 0)
    {
        throw std::runtime_error(ERROR_SET_NONBLOCK + std::to_string(errno));
    }
}

void Server::prepare_listener_socket()
{
    set_nonblock(listener_socket);

    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(params.port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    listener_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (listener_socket < 0)
    {
        throw std::runtime_error(ERROR_CREATE_SOCKET +
                                 std::to_string(errno));
    }

    int bind_status = bind(listener_socket, (sockaddr*)&socket_address, sizeof(socket_address));

    if (bind_status < 0)
    {
        throw std::runtime_error(ERROR_BIND + std::to_string(errno));
    }
    if (listen(listener_socket, params.max_connections_in_queue) < 0)
    {
        throw std::runtime_error(ERROR_LISTEN + std::to_string(errno));
    }
}

std::vector<Server::Connection> Server::process_listener(pollfd listener)
{
    std::vector<Connection> result;

    if (listener.revents & POLLERR)
    {
        throw std::runtime_error(ERROR_POLL_LISTENER + std::to_string(POLLERR));
    }

    for (int i = 0; i < listener.revents; ++i)
    {
        sockaddr_in peer{};
        socklen_t   peer_size = sizeof(peer);
        socket_t    channel = accept(listener_socket, (sockaddr*)&peer, &peer_size);
        if (channel < 0)
        {
            if (errno == EWOULDBLOCK) {
                break;
            }
            else {
                throw std::runtime_error(ERROR_ACCEPT + std::to_string(errno));
            }
        }
        set_nonblock(channel);
        result.push_back({ channel, SocketType::client });
        use_logger("Создано подключение для нового клиента.");
    }
    return result;
}

std::string Server::read_from_socket(socket_t socket)
{
    char        buffer[1024];
    std::string data;

    while (true)
    {
        ssize_t bytes_read = recv(socket, buffer, sizeof(buffer), MSG_DONTWAIT);
        if (bytes_read <= 0)
        {
            // Если нет данных для чтения или произошла ошибка, выходим из цикла
            break;
        }
        data.append(buffer, bytes_read);
    }

    return data;
}

bool Server::process_client(pollfd fd, socket_t client)
{
    if (fd.revents & POLLERR)
    {
        return false;
    }
    if (fd.revents & POLLHUP)
    {
        use_logger("Пользователь отключился.");
        close(client);
        return false;
    }
    if (fd.revents & POLLIN)
    {
        std::string data = read_from_socket(client);
        if (data.empty())
        {
            return false;
        }
        use_logger("Получены данные: " + data);
    }
    return true;
}

void Server::start()
{
    prepare_listener_socket();

    conn_sockets.push_back({ listener_socket, SocketType::listener });
    conn_sockets_pollfd.push_back({
        .fd = listener_socket,
        .events = POLLIN,
    });
    use_logger(start_message());

    while (true)
    {
        poll(conn_sockets_pollfd.data(), conn_sockets_pollfd.size(), -1);
        // Проверяем, есть ли новые подключения
        auto new_connections = process_listener(conn_sockets_pollfd.back());
        // Обрабатываем все старые подключения (пока что просто проверим, что они живы)
        for (size_t i = 0; i + 1 < conn_sockets_pollfd.size(); ++i)
        {
            bool result = process_client(conn_sockets_pollfd[i], conn_sockets[i].socket);
            if (!result)
            {
                conn_sockets[i].socket = -1;
            }
        }
        // Объединим новые подключения и еще живые старые в 1 список
        std::vector<Connection> updated_connections;
        std::vector<pollfd>     updated_pollfd;
        for (auto& conn : new_connections)
        {
            updated_connections.push_back(conn);
            updated_pollfd.push_back({
                .fd = conn.socket,
                .events = POLLIN,
            });
        }

        for (size_t i = 0; i < conn_sockets.size(); ++i)
        {
            if (conn_sockets[i].socket != -1)
            {
                updated_connections.push_back(conn_sockets[i]);
                updated_pollfd.push_back(conn_sockets_pollfd[i]);
            }
        }

        conn_sockets = std::move(updated_connections);
        conn_sockets_pollfd = std::move(updated_pollfd);
    }
}

Server::~Server()
{

}