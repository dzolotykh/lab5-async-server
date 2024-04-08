#ifndef LAB5_SOCKET_H
#define LAB5_SOCKET_H

namespace Server {

// RAII-обертка для сокета
class Socket {
    using socket_t = int;
    socket_t socket;

   public:
    explicit Socket(socket_t _socket) : socket(_socket) {}

    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    ~Socket();

    [[nodiscard]] socket_t get() const noexcept { return socket; }
};
}    // namespace Server

#endif    //LAB5_SOCKET_H
