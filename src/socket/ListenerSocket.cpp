#include <socket/ListenerSocket.h>
#include <netinet/tcp.h>
#include <exceptions/SocketExceptions.h>

Server::ListenerSocket::ListenerSocket(uint16_t port, int max_connections): Socket(-1) {
    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw Server::Exceptions::SocketCreationException(errno);
    }
    if (bind(fd, (sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        throw Server::Exceptions::SocketBindException(errno);
    }
    if (listen(fd, max_connections) < 0) {
        throw Server::Exceptions::SocketListenException(errno);
    }
    socket_fd = fd;
}

Server::ListenerSocket::ListenerSocket(Server::ListenerSocket &&other) noexcept : Socket(other.socket_fd) {
    other.socket_fd = -1;
}

Server::ListenerSocket &Server::ListenerSocket::operator=(Server::ListenerSocket &&other) noexcept {
    ListenerSocket tmp(std::move(other));
    socket_fd = tmp.socket_fd;
    tmp.socket_fd = -1;
    return *this;
}

Server::ClientSocket Server::ListenerSocket::accept_client() const {
    sockaddr_in client_address{};
    socklen_t client_address_len = sizeof(client_address);
    int client_fd = accept(socket_fd, (sockaddr *)&client_address, &client_address_len);
    if (client_fd < 0) {
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
    return Server::ClientSocket(client_fd);
}

Server::ClientSocket Server::ListenerSocket::accept_client_block(size_t timeout) const {
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(socket_fd, &read_set);
    timeval tv{};
    tv.tv_sec = timeout;
    int result = select(socket_fd + 1, &read_set, nullptr, nullptr, &tv);
    if (result == -1) {
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
    if (result == 0) {
        throw Server::Exceptions::NoClientException();
    }
    return accept_client();
}
