#ifndef LAB5_SERVER_H
#define LAB5_SERVER_H

#include <iostream>    // for debugging

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <unordered_map>

namespace Server {
using logger_t = std::function<void(const std::string &)>;

struct ServerParams {
    ServerParams(unsigned int port, logger_t logger, unsigned int max_connections_in_queue,
                 unsigned int max_process, unsigned int buff_size)
        : port(port),
          logger(std::move(logger)),
          max_connections_in_queue(max_connections_in_queue),
          max_process(max_process),
          buff_size(buff_size) {}

    ServerParams(const ServerParams &other) = default;

    ServerParams &operator=(const ServerParams &other) = default;

    ServerParams(ServerParams &&other) = default;

    ServerParams &operator=(ServerParams &&other) = default;

    unsigned int port;
    logger_t logger;
    unsigned int max_connections_in_queue;
    unsigned int max_process;
    unsigned int buff_size;
};

using socket_t = int;

enum class SocketType { listener, client };

// Класс-обертка для удобной работы с поллингом
// Умеет:
// добавлять сокеты в массив для поллинга
// удалять уже ненужны сокеты из массива (они должны быть помечены как -1)
// возвращать массив сокетов для поллинга. Последний элемент массива это сокет-слушатель
// отдельно хранится главный сокет-слушатель, который не должен быть удален
struct PollingWrapper {
   private:
    std::vector<pollfd> pollfds;
    std::vector<socket_t> connections;
    socket_t listener_socket;

   public:
    explicit PollingWrapper(socket_t listener_socket) : listener_socket(listener_socket) {}

    PollingWrapper(std::vector<socket_t> _connections, std::vector<pollfd> _pollfds)
        : connections(std::move(_connections)),
          pollfds(std::move(_pollfds)) {
        listener_socket = connections.back();
        connections.pop_back();
    }

    PollingWrapper() : listener_socket(-1) {}

    std::vector<socket_t> get_connections() {
        std::vector<socket_t> res = connections;
        res.push_back(listener_socket);
        return res;
    }

    std::vector<pollfd> get_pollfds() {
        std::vector<pollfd> res = pollfds;
        pollfd listener_pollfd = {.fd = listener_socket, .events = POLLIN};
        res.push_back(listener_pollfd);
        return res;
    }

    /* Возвращает пару: массив сокетов, которые необходимо прослушивать и массив pollfd */
    auto get() { return std::make_pair(get_connections(), get_pollfds()); }

    void remove_disconnected() {
        std::vector<socket_t> filtered_connections;
        std::vector<pollfd> filtered_pollfds;
        for (size_t i = 0; i < connections.size(); ++i) {
            if (connections[i] != -1) {
                filtered_connections.push_back(connections[i]);
                filtered_pollfds.push_back(pollfds[i]);
            }
        }
        connections = std::move(filtered_connections);
        pollfds = std::move(filtered_pollfds);
    }

    void add_connection(socket_t socket) {
        connections.push_back(socket);
        pollfd pollfd = {.fd = socket, .events = POLLHUP | POLLIN | POLLERR};
        pollfds.push_back(pollfd);
    }

    void add_connection(const std::vector<socket_t>& sockets) {
        for (auto socket : sockets) {
            add_connection(socket);
        }
    }

    size_t size() const { return connections.size(); }
};

class Server {
   public:
    explicit Server(ServerParams params);

    ~Server();

    Server(const Server &) = delete;

    Server &operator=(const Server &) = delete;

    Server(Server &&) = delete;

    Server &operator=(Server &&) = delete;

    void start();

   private:
    [[nodiscard]] std::string start_message() const;

    void prepare_listener_socket();

    void use_logger(const std::string &message);

    static void set_nonblock(socket_t socket);

    std::vector<socket_t> process_listener(pollfd listener);

    bool process_client(pollfd fd, socket_t client);

    std::string read_from_socket(socket_t socket);

    ServerParams params;
    socket_t listener_socket{};
    std::mutex logger_mtx;
    PollingWrapper polling_wrapper;
};
}    // namespace Server

#endif    // LAB5_SERVER_H
