#include "Socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/fcntl.h>

Server::Socket &Server::Socket::operator=(Server::Socket &&other) noexcept {
    if (this != &other) {
        socket = other.socket;
        other.socket = -1;
    }
    return *this;
}

Server::Socket::Socket(Server::Socket &&other) noexcept {
    socket = other.socket;
    other.socket = -1;
}

Server::Socket::~Socket() noexcept {
    if (socket != -1) {
        shutdown(socket, SHUT_RDWR);
        close(socket);
    }
}

Server::Socket::fd Server::Socket::get_fd() const noexcept {
    return socket;
}

Server::Socket::Socket(int _socket) noexcept : socket(_socket) {}

Server::Socket::Socket() noexcept : socket(-1) {}

Server::Socket Server::Socket::create_listener_socket(unsigned int port, int max_connections_in_queue) {
    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw SocketException("Ошибка во время создания сокета. Код ошибки: " + std::to_string(errno));
    }
    if (bind(fd, (sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        throw SocketException("Ошибка во время привязки сокета к адресу. Код ошибки: " + std::to_string(errno));
    }
    if (listen(fd, max_connections_in_queue) < 0) {
        throw SocketException("Ошибка во время прослушивания сокета. Код ошибки: " + std::to_string(errno));
    }
    return Socket(fd);
}

void Server::Socket::set_attribute(Server::Socket::attributes attr) const {
    int current_flags = fcntl(socket, F_GETFL, 0);
    if (fcntl(socket, F_SETFL, current_flags | int(attr)) < 0) {
        throw SocketException("Ошибка во время установки атрибута сокета. Код ошибки: " + std::to_string(errno));
    }
}
