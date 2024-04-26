#ifndef ASYNC_SERVER_EXAMPLE_SOCKETEXCEPTIONS_H
#define ASYNC_SERVER_EXAMPLE_SOCKETEXCEPTIONS_H

#include <exception>
#include <string>
#include <socket/Socket.h>

namespace Server::Exceptions {
    class SocketException : public std::exception {
    public:
        explicit SocketException(std::string _message) : message(std::move(_message)) {}

        const char *what() const noexcept override {
            return message.c_str();
        }

    protected:
        std::string message;
    };

    class ClientDisconnectedException : public SocketException {
    public:
        explicit ClientDisconnectedException(const Socket& s): SocketException("User on socket ") {
            message += std::to_string(s.get_fd()) + " disconnected.";
        }
    };

    class NotEnoughBytesException: public SocketException {
    public:
        explicit NotEnoughBytesException(const Socket& s, size_t need_bytes, size_t available_bytes): SocketException("Cannot read ") {
            message += std::to_string(need_bytes) + " bytes from socket " + std::to_string(s.get_fd()) + ". Bytes available: " + std::to_string(available_bytes);
        }
    };

    class SocketExceptionErrno: public SocketException {
    public:
        explicit SocketExceptionErrno(const Socket&s, int errno_value): SocketException("Error on socket ") {
            message += std::to_string(s.get_fd()) + ". Errno value: " + std::to_string(errno_value);
        }
    };

    class SocketCreationException: public SocketException {
    public:
        explicit SocketCreationException(int errno_value): SocketException("Error while creating socket. Errno value: ") {
            message += std::to_string(errno_value);
        }
    };

    class SocketBindException: public SocketException {
    public:
        explicit SocketBindException(int errno_value): SocketException("Error while binding socket. Errno value: ") {
            message += std::to_string(errno_value);
        }
    };

    class SocketListenException: public SocketException {
    public:
        explicit SocketListenException(int errno_value): SocketException("Error while listening socket. Errno value: ") {
            message += std::to_string(errno_value);
        }
    };

    class SocketAcceptException: public SocketException {
    public:
        explicit SocketAcceptException(int errno_value): SocketException("Error while accepting socket. Errno value: ") {
            message += std::to_string(errno_value);
        }
    };

    class NoClientException: public SocketException {
    public:
        explicit NoClientException(): SocketException("No client to accept.") {}
    };


}

#endif //ASYNC_SERVER_EXAMPLE_SOCKETEXCEPTIONS_H
