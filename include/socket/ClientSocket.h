#ifndef ASYNC_SERVER_EXAMPLE_CLIENTSOCKET_H
#define ASYNC_SERVER_EXAMPLE_CLIENTSOCKET_H

#include <socket/Socket.h>
#include <sys/socket.h>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Server {
class ClientSocket final : public Socket {
   public:
    ClientSocket() = delete;
    ClientSocket(const ClientSocket& other) = delete;
    ClientSocket& operator=(const ClientSocket& other) = delete;

    ClientSocket(ClientSocket&& other) noexcept;
    ClientSocket& operator=(ClientSocket&& other) noexcept;

    void send_bytes(const char* bytes_dst, size_t amount) const;
    void send_bytes(const std::string& bytes) const;
    void send_byte(char byte) const;

    void read_bytes(char* to, size_t amount) const;
    [[nodiscard]] std::vector<char> read_bytes(size_t amount) const;
    [[nodiscard]] char read_byte() const;
    [[nodiscard]] size_t ready_to_read() const;

    [[nodiscard]] std::string get_ip() const;
    [[nodiscard]] std::string get_info() const;

    ~ClientSocket();

   private:
    explicit ClientSocket(fd _socket_fd) : Socket(_socket_fd) {}
    friend class ListenerSocket;
};
}    // namespace Server

#endif    //ASYNC_SERVER_EXAMPLE_CLIENTSOCKET_H
