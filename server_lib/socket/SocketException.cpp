#include "SocketException.h"

Server::SocketException::SocketException(std::string message) : message(std::move(message)) {}

const char *Server::SocketException::what() const noexcept {
    return message.c_str();
}
