#ifndef ASYNC_SERVER_EXAMPLE_CLIENTSOCKET_H
#define ASYNC_SERVER_EXAMPLE_CLIENTSOCKET_H

#include <cstdint>
#include <cstddef>
#include <socket/Socket.h>
#include <sys/socket.h>

namespace Server {
    class ClientSocket: public Socket {
    public:
        ClientSocket() = delete;
        ClientSocket(const ClientSocket& other) = delete;
        ClientSocket& operator=(const ClientSocket& other) = delete;

        ClientSocket(ClientSocket&& other) noexcept;
        ClientSocket& operator=(ClientSocket&& other) noexcept;

        void send_bytes(const int8_t* bytes_dst, size_t amount) const;
        void send_bytes(const std::string& bytes) const;
        void send_byte(int8_t byte) const;

        void read_bytes(int8_t* to, size_t amount) const;
        [[nodiscard]] std::vector<int8_t> read_bytes(size_t amount) const;
        [[nodiscard]] int8_t read_byte() const;
        [[nodiscard]] size_t ready_to_read() const;

        ~ClientSocket();
    private:
        explicit ClientSocket(fd _socket_fd): Socket(_socket_fd) {}
        friend class ListenerSocket;
    };
}

#endif //ASYNC_SERVER_EXAMPLE_CLIENTSOCKET_H
