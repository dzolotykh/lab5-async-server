#ifndef ASYNC_SERVER_EXAMPLE_SOCKET_H
#define ASYNC_SERVER_EXAMPLE_SOCKET_H

#include <string>
#include <unistd.h>
#include <arpa/inet.h>

namespace Server {
    class Socket {
    public:
        using fd = int;

        fd get_fd() const noexcept;
        std::string get_ip() const;

        ~Socket() noexcept;

    protected:
        Socket(fd _socket_fd);
        fd socket_fd;
    };
}

#endif //ASYNC_SERVER_EXAMPLE_SOCKET_H
