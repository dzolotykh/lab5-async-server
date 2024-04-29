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

std::string Server::Socket::get_ip() const {
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);
    int result = getpeername(socket_fd, reinterpret_cast<sockaddr*>(&addr), &addr_len);
    if (result == -1) {
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    return {ip};
}
