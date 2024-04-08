#include "Socket.h"
#include <unistd.h>

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

Server::Socket::~Socket() {
    if (socket != -1) {
        close(socket);
    }
}
