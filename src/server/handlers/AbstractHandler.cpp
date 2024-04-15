#include "AbstractHandler.h"
#include <iostream>

void Server::AbstractHandler::read_bytes(int client_socket, size_t bytes, char* dst) {
    size_t bytes_read = 0;
    while (bytes_read < bytes) {
        ssize_t read = recv(client_socket, dst + bytes_read, bytes - bytes_read, MSG_DONTWAIT);
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
        if (bytes_read < need_read) {
            size_t want_read = std::min(need_read - bytes_read, buff_size);
            ssize_t read = recv(client_socket, dst, want_read, MSG_DONTWAIT);
            if (read == -1 && errno != EAGAIN) {
                throw SocketException("Ошибка при чтении данных из сокета.");
            } else if (read == -1) {
                return true;
            } else if (read == 0) {
                throw BadInputException("Клиент отключился.");
            }
            bytes_read += read;
            on_read(read);
        }
        return bytes_read < need_read;
    };
}

std::function<bool()> Server::AbstractHandler::construct_writer(
    int client_socket, size_t need_write,
    const std::function<std::pair<const char*, size_t>()>& get_bytes) {
    size_t src_size = 0;
    const char* src = nullptr;
    size_t bytes_written_total = 0;
    size_t bytes_written_from_src = 0;
    size_t bytes_collected = 0;
    auto result = [=]() mutable {
        if (bytes_written_total == need_write) {
            return false;
        }
        if (bytes_written_from_src == src_size) {
            auto [new_src, new_size] = get_bytes();
            src = new_src;
            src_size = new_size;
            bytes_collected += new_size;
            bytes_written_from_src = 0;
        }
        size_t want_write =
            std::min(src_size - bytes_written_from_src, need_write - bytes_written_total);
        ssize_t written =
            send(client_socket, src + bytes_written_from_src, want_write, MSG_NOSIGNAL);
        if (written == -1 && errno != EAGAIN) {
            throw SocketException("Ошибка при записи данных в сокет.");
        } else if (written == 0) {
            throw BadInputException("Клиент отключился.");
        } else if (written == -1) {
            return true;
        }
        bytes_written_total += written;
        bytes_written_from_src += written;
        return bytes_written_total < need_write;
    };
    return result;
}

std::function<bool()> Server::AbstractHandler::write_bytes_nonblock(
    int client_socket, size_t need_write,
    const std::function<std::pair<const char*, size_t>()>& get_bytes) {
    return construct_writer(client_socket, need_write, get_bytes);
}
