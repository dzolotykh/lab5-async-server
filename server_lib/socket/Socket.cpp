#include "Socket.h"
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <utility>

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

#include <iostream>

Server::Socket::~Socket() noexcept {
    if (socket == -1) {
        return;
    }
    std::cout << "Socket destructor for socket " << socket << std::endl;
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

Server::Socket Server::Socket::make_listener(unsigned int port, int max_connections_in_queue) {
    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw SocketException("Ошибка во время создания сокета. Код ошибки: " +
                              std::to_string(errno));
    }
    if (bind(fd, (sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        throw SocketException("Ошибка во время привязки сокета к адресу. Код ошибки: " +
                              std::to_string(errno));
    }
    if (listen(fd, max_connections_in_queue) < 0) {
        throw SocketException("Ошибка во время прослушивания сокета. Код ошибки: " +
                              std::to_string(errno));
    }
    return Socket(fd);
}

void Server::Socket::set_attribute(Server::Socket::attributes attr) {
    int current_flags = fcntl(socket, F_GETFL, 0);
    if (fcntl(socket, F_SETFL, current_flags | int(attr)) < 0) {
        throw SocketException("Ошибка во время установки атрибута сокета. Код ошибки: " +
                              std::to_string(errno));
    }
}

Server::Socket::predicate_t Server::Socket::read_bytes_nonblock(
    size_t need_read, char *dst, size_t buff_size, const Server::Socket::on_read_t &on_read) const {
    return NonblockingReader(need_read, dst, buff_size, on_read, *this);
}

Server::Socket::predicate_t Server::Socket::read_bytes_nonblock(
    char *dst, size_t buff_size, const Server::Socket::on_read_t &on_read) const {
    return NonblockingReader(dst, buff_size, on_read, *this);
}

Server::Socket::predicate_t Server::Socket::write_bytes_nonblock(
    size_t need_write, const Server::Socket::get_bytes_t &get_bytes) const {
    return Server::Socket::predicate_t();
}

Server::Socket::NonblockingReader::NonblockingReader(size_t need_read, char *dst, size_t buff_size,
                                                     Server::Socket::on_read_t on_read,
                                                     const Socket &_client)
    : need_read(need_read),
      dst(dst),
      buff_size(buff_size),
      on_read(std::move(on_read)),
      bytes_read(0),
      client(_client) {}

bool Server::Socket::NonblockingReader::operator()() {
    if (read_all) {
        ssize_t read = recv(client.get_fd(), dst, buff_size, MSG_DONTWAIT);

        if (read == -1 && errno != EAGAIN) {
            throw SocketException("Ошибка при чтении данных из сокета.");
        } else if (read == -1) {
            return true;
        } else if (read == 0) {
            return false;
        }
        on_read(read);

        return true;
    }

    if (bytes_read < need_read) {
        size_t want_read = std::min(need_read - bytes_read, buff_size);
        ssize_t read = recv(client.get_fd(), dst, want_read, MSG_DONTWAIT);
        if (read == -1 && errno != EAGAIN) {
            throw SocketException("Ошибка при чтении данных из сокета. Код ошибки: " +
                                  std::to_string(errno));
        } else if (read == -1) {
            return true;
        } else if (read == 0) {
            throw ClientDisconnectedException("Клиент отключился.");
        }
        bytes_read += read;
        on_read(read);
    }
    return bytes_read < need_read;
}

Server::Socket::NonblockingReader::NonblockingReader(char *dst, size_t buff_size,
                                                     Server::Socket::on_read_t on_read,
                                                     const Server::Socket &_client)
    : dst(dst),
      buff_size(buff_size),
      on_read(std::move(on_read)),
      client(_client),
      read_all(true) {}

std::string Server::Socket::get_ip() const {
    sockaddr_in addr{};
    socklen_t addr_size = sizeof(addr);
    if (getpeername(socket, (sockaddr *)&addr, &addr_size) < 0) {
        throw SocketException("Ошибка при получении адреса клиента. Код ошибки: " +
                              std::to_string(errno));
    }
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    return {ip};
}
