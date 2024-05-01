#include <exceptions/SocketExceptions.h>
#include <netinet/tcp.h>
#include <socket/Socket.h>

Server::Socket::Socket(Server::Socket::fd _socket_fd) : socket_fd(_socket_fd) {}

Server::Socket::~Socket() noexcept {
    if (socket_fd != -1) {
        close(socket_fd);
    }
}

Server::Socket::fd Server::Socket::get_fd() const noexcept {
    return socket_fd;
}
