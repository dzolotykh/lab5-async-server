#ifndef ASYNC_SERVER_EXAMPLE_LISTENERSOCKET_H
#define ASYNC_SERVER_EXAMPLE_LISTENERSOCKET_H

#include <socket/ClientSocket.h>
#include <socket/Socket.h>
#include <memory>

namespace Server {
class ListenerSocket final : public Socket {
   public:
    ListenerSocket() = delete;
    ListenerSocket(const ListenerSocket& other) = delete;
    ListenerSocket& operator=(const ListenerSocket& other) = delete;

    ListenerSocket(ListenerSocket&& other) noexcept;
    ListenerSocket& operator=(ListenerSocket&& other) noexcept;

    ListenerSocket(uint16_t port, int max_connections);

    [[nodiscard]] std::shared_ptr<ClientSocket> accept_client(int timeout) const;
};
}    // namespace Server

#endif    //ASYNC_SERVER_EXAMPLE_LISTENERSOCKET_H
