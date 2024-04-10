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

#include "Params.h"

namespace Server {
using logger_t = std::function<void(const std::string &)>;

using socket_t = int;

class Server {
   public:
    explicit Server(Params params);

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

    Params params;
    socket_t listener_socket{};
    std::mutex logger_mtx;
    PollingWrapper polling_wrapper;
};
}    // namespace Server

#endif    // LAB5_SERVER_H
