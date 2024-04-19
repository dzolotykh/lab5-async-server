#include "Socket.h"
#include <unistd.h>
#include <sys/socket.h>

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
