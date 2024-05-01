#include <exceptions/SocketExceptions.h>
#include <socket/ClientSocket.h>
#include <sys/ioctl.h>
#include <csignal>
#include <iostream>

void Server::ClientSocket::send_bytes(const char *bytes_dst, size_t amount) const {
    size_t sent = 0;
    while (sent < amount) {
        size_t need_send = amount - sent;
        ssize_t result = send(socket_fd, bytes_dst + sent, need_send, 0);
        if (result != -1) {
            sent += result;
            continue;
        }

        if (errno == ECONNRESET || errno == SIGPIPE || errno == EPIPE) {
            throw Server::Exceptions::ClientDisconnectedException(*this);
        }
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
}

void Server::ClientSocket::read_bytes(char *to, size_t amount) const {
    ssize_t result = recv(socket_fd, to, amount, MSG_WAITALL);
    if (result == -1) {
        if (errno == ECONNRESET || errno == SIGPIPE || errno == EPIPE) {
            throw Server::Exceptions::ClientDisconnectedException(*this);
        }
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
    if (result != amount)
        throw Server::Exceptions::ClientDisconnectedException(*this);
}

size_t Server::ClientSocket::ready_to_read() const {
    int bytes_available;
    int result = ioctl(socket_fd, FIONREAD, &bytes_available);
    if (result == -1) {
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
    return bytes_available;
}

Server::ClientSocket::ClientSocket(Server::ClientSocket &&other) noexcept
    : Socket(other.socket_fd) {
    other.socket_fd = -1;
}

Server::ClientSocket &Server::ClientSocket::operator=(Server::ClientSocket &&other) noexcept {
    ClientSocket tmp(std::move(other));
    socket_fd = tmp.socket_fd;
    tmp.socket_fd = -1;
    return *this;
}

Server::ClientSocket::~ClientSocket() {
    shutdown(socket_fd, SHUT_RDWR);
}

std::vector<char> Server::ClientSocket::read_bytes(size_t amount) const {
    std::vector<char> buffer(amount);
    read_bytes(buffer.data(), amount);
    return buffer;
}

char Server::ClientSocket::read_byte() const {
    char byte;
    read_bytes(&byte, 1);
    return byte;
}

void Server::ClientSocket::send_bytes(const std::string &bytes) const {
    send_bytes(reinterpret_cast<const char *>(bytes.data()), bytes.size());
}

void Server::ClientSocket::send_byte(char byte) const {
    send_bytes(&byte, 1);
}

std::string Server::ClientSocket::get_info() const {
    return "IP: " + get_ip() + ", сокет: " + std::to_string(get_fd());
}

std::string Server::ClientSocket::load_ip() const {
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);
    int result = getpeername(socket_fd, reinterpret_cast<sockaddr *>(&addr), &addr_len);
    if (result == -1) {
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    return {ip};
}

std::string Server::ClientSocket::get_ip() const {
    return ip;
}