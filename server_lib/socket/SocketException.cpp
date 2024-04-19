#include "SocketException.h"

#include <utility>

Server::SocketException::SocketException(std::string message) : message(std::move(message)) {}

const char *Server::SocketException::what() const noexcept {
    return message.c_str();
}

Server::ClientDisconnectedException::ClientDisconnectedException(std::string message) : SocketException(std::move(message)) {

}
