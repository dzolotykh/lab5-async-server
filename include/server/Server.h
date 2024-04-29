#ifndef ASYNC_SERVER_EXAMPLE_SERVER_H
#define ASYNC_SERVER_EXAMPLE_SERVER_H

#include <exceptions/EndpointExceptions.h>
#include <logging/Logger.h>
#include <multithreading/ThreadPool.h>
#include <server/Response.h>
#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>
#include <socket/ListenerSocket.h>
#include <functional>
#include <unordered_map>

namespace Server {
class Server {
   public:
    using handler_provider_t =
        std::function<std::unique_ptr<Handlers::IHandler>(const ClientSocket&)>;

    Server() = delete;
    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    Server(Server&& other) noexcept = delete;
    Server& operator=(Server&& other) noexcept = delete;

    Server(uint16_t port, int max_connections, int thread_pool_size);
    ~Server();

    void start();
    void stop();

    template <typename handler_t, typename... handler_constructor_params_t>
    void set_endpoint(char endpoint_byte, handler_constructor_params_t&&... constructor_params) {
        endpoints[endpoint_byte] = [&constructor_params...](const ClientSocket& client) {
            return std::make_unique<handler_t>(client, constructor_params...);
        };
    }

   private:
    ListenerSocket listener_socket;
    std::unordered_map<char, handler_provider_t> endpoints;
    Multithreading::ThreadPool tp;

    bool is_running = true;

    void handle_client(const ClientSocket& client);
};
}    // namespace Server

#endif    //ASYNC_SERVER_EXAMPLE_SERVER_H
