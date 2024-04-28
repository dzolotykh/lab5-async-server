#ifndef ASYNC_SERVER_EXAMPLE_SERVER_H
#define ASYNC_SERVER_EXAMPLE_SERVER_H

#include <socket/ClientSocket.h>
#include <socket/ListenerSocket.h>
#include <server/handlers/IHandler.h>
#include <server/Response.h>
#include <exceptions/EndpointExceptions.h>
#include <logging/Logger.h>
#include <multithreading/ThreadPool.h>

#include <functional>

namespace Server {
    class Server {
    public:
        using handler_provider_t = std::function<std::unique_ptr<Handlers::IHandler>(const ClientSocket&)>;

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
        void set_endpoint(char endpoint_byte, handler_constructor_params_t&&...constructor_params) {
            endpoints[endpoint_byte] = [&constructor_params...](const ClientSocket& client) {
                return std::make_unique<handler_t>(client, constructor_params...);
            };
        }

    private:
        ListenerSocket listener_socket;
        std::unordered_map<char, handler_provider_t> endpoints;
        Multithreading::ThreadPool tp;

        void handle_client(const ClientSocket& client);
    };
}

#endif //ASYNC_SERVER_EXAMPLE_SERVER_H
