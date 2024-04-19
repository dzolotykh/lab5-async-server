#ifndef LAB5_SOCKET_H
#define LAB5_SOCKET_H

namespace Server {

/// RAII-обертка для сокета
class Socket {
   public:
    using fd = int;
    explicit Socket(int _socket) noexcept;
    Socket() noexcept;

    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    ~Socket() noexcept;

    [[nodiscard]] fd get_fd() const noexcept;
private:
    int socket;
};
}    // namespace Server

#endif    //LAB5_SOCKET_H
