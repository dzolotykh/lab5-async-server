#ifndef LAB5_SOCKET_H
#define LAB5_SOCKET_H

#include "SocketException.h"
#include <sys/fcntl.h>

namespace Server {

/// RAII-обертка для сокета
class Socket {
   public:
    using fd = int;
    enum class attributes;

    explicit Socket(int _socket) noexcept;
    Socket() noexcept;

    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    ~Socket() noexcept;

    [[nodiscard]] fd get_fd() const noexcept;
    void set_attribute(attributes attr) const;

    static Socket create_listener_socket(unsigned int port, int max_connections_in_queue);
private:
    int socket;
};

enum class Socket::attributes {
    NONBLOCK = O_NONBLOCK,
};
}    // namespace Server

#endif    //LAB5_SOCKET_H
