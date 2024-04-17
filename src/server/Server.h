#ifndef LAB5_SERVER_H
#define LAB5_SERVER_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <unordered_map>

#include "thread-pool/Pool.h"
#include <thread>
#include "Params.h"
#include "PollingWrapper.h"
#include "typenames.h"

namespace Server {
class Server {
   public:
    explicit Server(Params _params);

    ~Server();

    Server(const Server &) = delete;

    Server &operator=(const Server &) = delete;

    Server(Server &&) = delete;

    Server &operator=(Server &&) = delete;

    void start();

    template<typename handler_t, typename... handler_constructor_params_t>
    void add_endpoint(char name, handler_constructor_params_t&... constructor_params) {
        endpoints[name] = [&constructor_params...](socket_t client) {
            return std::make_unique<handler_t>(client, constructor_params...);
        };
    }

    void stop();

   private:
    [[nodiscard]] std::string start_message() const;

    void prepare_listener_socket();

    void use_logger(const std::string &message);

    static void set_nonblock(socket_t socket);

    std::vector<socket_t> process_listener(pollfd listener);

    bool process_client(pollfd fd, socket_t client);

    using pollfds_iter = std::vector<pollfd>::iterator;
    using sockets_iter = std::vector<socket_t>::iterator;
    void process_all_clients(pollfds_iter pollfds_begin, pollfds_iter pollfds_end,
                             sockets_iter sockets_begin, sockets_iter sockets_end);

    Params params;
    socket_t listener_socket;
    std::mutex logger_mtx;
    PollingWrapper polling_wrapper;
    std::atomic<bool> is_running = true;
    ThreadPool::Pool pool;

    /* Тут будем хранить функции-обработчики для каждого клиента. Если работа с клиентом завершена,
     * то обработчик должен вернуть false. */
    std::unordered_map<socket_t, std::unique_ptr<AbstractHandler>> client_handlers;
    std::unordered_map<socket_t, bool> client_status;

    std::unordered_map<char, handler_provider_t> endpoints;
    constexpr static const char *INTERNAL_ERROR_TEXT = "ERROR|Internal server error.";
};
}    // namespace Server

#endif    // LAB5_SERVER_H
