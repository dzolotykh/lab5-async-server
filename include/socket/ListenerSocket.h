#ifndef ASYNC_SERVER_EXAMPLE_LISTENERSOCKET_H
#define ASYNC_SERVER_EXAMPLE_LISTENERSOCKET_H

#include <socket/Socket.h>
#include <socket/ClientSocket.h>

namespace Server {
    class ListenerSocket: public Socket {
    public:
        ListenerSocket() = delete;
        ListenerSocket(const ListenerSocket& other) = delete;
        ListenerSocket& operator=(const ListenerSocket& other) = delete;

        ListenerSocket(ListenerSocket&& other) noexcept;
        ListenerSocket& operator=(ListenerSocket&& other) noexcept;

        ListenerSocket(uint16_t port, int max_connections);

        [[nodiscard]] ClientSocket accept_client() const;
        [[nodiscard]] ClientSocket accept_client_block(size_t timeout) const;
    };
}

#endif //ASYNC_SERVER_EXAMPLE_LISTENERSOCKET_H
