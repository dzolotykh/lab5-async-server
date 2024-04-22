#ifndef LAB5_SOCKET_H
#define LAB5_SOCKET_H

#include <arpa/inet.h>
#include <sys/fcntl.h>
#include "SocketException.h"
#include <functional>

namespace Server {

/// RAII-обертка для сокета
class Socket {
   public:
    class NonblockingReader;
    class NonblockingWriter;

    using fd = int;
    using on_read_t = std::function<void(size_t)>;
    using get_bytes_t = std::function<std::pair<const char*, size_t>()>;
    using predicate_t = std::function<bool()>;
    enum class attributes;

    explicit Socket(int _socket) noexcept;
    Socket() noexcept;

    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    ~Socket() noexcept;

    [[nodiscard]] fd get_fd() const noexcept;
    [[nodiscard]] std::string get_ip() const;

    void set_attribute(attributes attr);

    predicate_t read_bytes_nonblock(size_t need_read, char* dst, size_t buff_size,
                                    const on_read_t& on_read) const;

    predicate_t read_bytes_nonblock(char* dst, size_t buff_size, const on_read_t& on_read) const;

    predicate_t write_bytes_nonblock(size_t need_write, const get_bytes_t& get_bytes) const;

    static Socket make_listener(unsigned int port, int max_connections_in_queue);

   private:
    int socket;
};

enum class Socket::attributes {
    NONBLOCK = O_NONBLOCK,
};

class Socket::NonblockingWriter {
public:
    NonblockingWriter(size_t _need_write, const get_bytes_t& _get_bytes, const Socket& _client);
    bool operator()();
    ~NonblockingWriter() = default;
private:
    const Socket& client;
    const get_bytes_t& get_bytes;
    const size_t need_write;
    size_t src_size = 0;
    const char* src = nullptr;
    size_t bytes_written_total = 0;
    size_t bytes_written_from_src = 0;
    size_t bytes_collected = 0;
};

class Socket::NonblockingReader {
   public:
    NonblockingReader(size_t need_read, char* dst, size_t buff_size, on_read_t on_read,
                      const Socket& _client);
    NonblockingReader(char* dst, size_t buff_size, on_read_t on_read, const Socket& _client);
    bool operator()();
    ~NonblockingReader() = default;

   private:
    size_t need_read;
    char* dst;
    size_t buff_size;
    on_read_t on_read;
    size_t bytes_read;
    const Socket& client;

    bool read_all = false;    // TODO надо переименовать
};

}    // namespace Server

#endif    //LAB5_SOCKET_H
