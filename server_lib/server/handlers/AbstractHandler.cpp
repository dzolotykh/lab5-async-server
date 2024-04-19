#include "AbstractHandler.h"

std::function<bool()> Server::AbstractHandler::construct_writer(
    Socket::fd client_socket, size_t need_write,
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
    const Socket& client_socket, size_t need_write,
    const std::function<std::pair<const char*, size_t>()>& get_bytes) {
    return construct_writer(client_socket.get_fd(), need_write, get_bytes);
}
