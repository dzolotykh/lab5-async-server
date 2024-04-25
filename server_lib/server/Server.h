#ifndef LAB5_SERVER_H
#define LAB5_SERVER_H

#include <Socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include "Exceptions.h"
#include "handlers/EndpointHandler.h"
#include "log.h"

#include <thread>
#include "Params.h"
#include "thread-pool/Pool.h"
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

    template <typename handler_t, typename... handler_constructor_params_t>
    void add_endpoint(char name, handler_constructor_params_t&&...constructor_params) {
        endpoints[name] = [&constructor_params...](const Socket &client) {
            return std::make_unique<handler_t>(client, constructor_params...);
        };
    }

    /// Принудительно выключает сервер.
    /// Самый грубый способ завершения, поэтому лучше использовать только в крайнем случае
    void stop();

    /// Принудительно выключает сервер после определенного таймаута.
    std::thread stop(std::chrono::milliseconds timeout);

   private:
    [[nodiscard]] std::string start_message() const;

    void use_logger(const std::string &message);

    void process_listener(pollfd listener);

    bool process_client(pollfd fd, const Socket &client);

    std::vector<pollfd> generate_pollfds();

    using pollfds_iter = std::vector<pollfd>::iterator;
    using sockets_iter = std::vector<std::unique_ptr<Socket>>::iterator;
    void process_all_clients(pollfds_iter pollfds_begin, pollfds_iter pollfds_end,
                             sockets_iter sockets_begin, sockets_iter sockets_end);

    Params params;
    Socket listener_socket;
    std::vector<std::unique_ptr<Socket>> clients;
    std::mutex logger_mtx;
    std::atomic<bool> is_running = true;
    std::atomic<bool> stop_accept = false;

    ThreadPool::Pool pool;

    /* Тут будем хранить функции-обработчики для каждого клиента. Если работа с клиентом завершена,
     * то обработчик должен вернуть false. */
    std::unordered_map<Socket::fd, std::unique_ptr<AbstractHandler>> client_handlers;
    std::unordered_map<Socket::fd, bool> client_status;

    std::unordered_map<char, handler_provider_t> endpoints;

    constexpr static const char *INTERNAL_ERROR_TEXT = "ERROR|Internal server error.";
};
}    // namespace Server

#endif    // LAB5_SERVER_H
