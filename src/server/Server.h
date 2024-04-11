#ifndef LAB5_SERVER_H
#define LAB5_SERVER_H

#include <iostream>    // for debugging

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <unordered_map>

#include "Params.h"
#include "PollingWrapper.h"
#include "handlers/FileUploadHandler.h"

namespace Server {
using logger_t = std::function<void(const std::string &)>;
using socket_t = int;

class Server {
   public:
    explicit Server(Params _params);

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

    Params params;
    socket_t listener_socket;
    std::mutex logger_mtx;
    PollingWrapper polling_wrapper;

    /* Тут будем хранить функции-обработчики для каждого клиента. Если работа с клиентом завершена,
     * то обработчик должен вернуть false. */
    std::unordered_map<socket_t, std::unique_ptr<AbstractHandler>> client_handlers;
    std::unordered_map<socket_t, bool> client_status;
};
}    // namespace Server

#endif    // LAB5_SERVER_H
