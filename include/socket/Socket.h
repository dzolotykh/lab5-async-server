#ifndef ASYNC_SERVER_EXAMPLE_SOCKET_H
#define ASYNC_SERVER_EXAMPLE_SOCKET_H

#include <arpa/inet.h>
#include <unistd.h>
#include <string>

namespace Server {
class Socket {
   public:
    using fd = int;

    virtual fd get_fd() const noexcept;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept = default;
    Socket& operator=(Socket&& other) noexcept = default;

    virtual ~Socket() noexcept;

   protected:
    explicit Socket(fd _socket_fd);

    fd socket_fd;
};
}    // namespace Server

#endif    //ASYNC_SERVER_EXAMPLE_SOCKET_H
