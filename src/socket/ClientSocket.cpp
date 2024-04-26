#include <socket/ClientSocket.h>
#include <exceptions/SocketExceptions.h>
#include <sys/ioctl.h>

void Server::ClientSocket::send_bytes(const int8_t *bytes_dst, size_t amount) const {
    size_t sent = 0;
    while (sent < amount) {
        size_t need_send = amount - sent;
        ssize_t result = send(socket_fd, bytes_dst + sent, need_send, 0);
        if (result != -1) {
            sent += result;
            continue;
        }

        if (errno == ECONNRESET) {
            throw Server::Exceptions::ClientDisconnectedException(*this);
        }
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
}

void Server::ClientSocket::read_bytes(int8_t* to, size_t amount) const {
    ssize_t result = recv(socket_fd, to, amount, MSG_WAITALL);
    if (result == -1) throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    if (result != amount) throw Server::Exceptions::ClientDisconnectedException(*this);
}

size_t Server::ClientSocket::ready_to_read() const {
    int bytes_available;
    int result = ioctl(socket_fd, FIONREAD, &bytes_available);
    if (result == -1) {
        throw Server::Exceptions::SocketExceptionErrno(*this, errno);
    }
    return bytes_available;
}

Server::ClientSocket::ClientSocket(Server::ClientSocket &&other) noexcept : Socket(other.socket_fd) {
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

std::vector<int8_t> Server::ClientSocket::read_bytes(size_t amount) const {
    std::vector<int8_t> buffer(amount);
    read_bytes(buffer.data(), amount);
    return buffer;
}

int8_t Server::ClientSocket::read_byte() const {
    int8_t byte;
    read_bytes(&byte, 1);
    return byte;
}

void Server::ClientSocket::send_bytes(const std::string &bytes) const {
    send_bytes(reinterpret_cast<const int8_t*>(bytes.data()), bytes.size());
}

void Server::ClientSocket::send_byte(int8_t byte) const {
    send_bytes(&byte, 1);
}
