#include "AbstractHandler.h"
#include <iostream>

void Server::AbstractHandler::read_bytes(int client_socket, size_t bytes, char* dst) {
    size_t bytes_read = 0;
    while (bytes_read < bytes) {
        ssize_t read = recv(client_socket, dst + bytes_read, bytes - bytes_read, MSG_DONTWAIT);
        std::cout << read << std::endl;
        if (read == -1 && errno != EAGAIN) {
            throw SocketException("Ошибка при чтении данных из сокета.");
        } else if (read == -1) {
            continue;
        } else if (read == 0) {
            throw BadInputException("Клиент отключился.");
        }
        bytes_read += read;
    }
}

void Server::AbstractHandler::write_bytes(int client_socket, size_t bytes, const char* src) {
    size_t bytes_written = 0;
    while (bytes_written < bytes) {
        ssize_t written =
            send(client_socket, src + bytes_written, bytes - bytes_written, MSG_NOSIGNAL);
        if (written == -1 && errno != EAGAIN) {
            throw SocketException("Ошибка при записи данных в сокет.");
        } else if (written == 0) {
            throw BadInputException("Клиент отключился.");
        }
        bytes_written += written;
    }
}

#include <iostream>

std::function<bool()> Server::AbstractHandler::read_bytes_nonblock(
    int client_socket, size_t need_read, char* dst, size_t buff_size,
    const std::function<void(size_t)>& on_read) {
    size_t bytes_read = 0;
    return [client_socket, need_read, dst, buff_size, bytes_read, on_read]() mutable {
        std::cout << "bytes_read: " << bytes_read << std::endl;
        if (bytes_read < need_read) {
            size_t want_read = std::min(need_read - bytes_read, buff_size);
            read_bytes(client_socket, want_read, dst);
            bytes_read += want_read;
            on_read(want_read);
        }
        std::cout << "bytes_read: " << bytes_read << " " << need_read << std::endl;
        return bytes_read < need_read;
    };
}

std::function<bool()> Server::AbstractHandler::write_bytes_nonblock(
    int client_socket, size_t bytes_write,
    const std::function<std::pair<const char*, size_t>()>& get_bytes) {
    size_t bytes_written = 0;
    return [client_socket, bytes_write, bytes_written, get_bytes]() mutable {
        if (bytes_written < bytes_write) {
            auto [src, size] = get_bytes();
            size_t want_write = std::min(bytes_write - bytes_written, size);
            write_bytes(client_socket, want_write, src);
            bytes_written += want_write;
        }
        return bytes_written < bytes_write;
    };
}
