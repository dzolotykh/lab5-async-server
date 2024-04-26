#ifndef ASYNC_SERVER_EXAMPLE_ENDPOINTEXCEPTIONS_H
#define ASYNC_SERVER_EXAMPLE_ENDPOINTEXCEPTIONS_H

#include <string>
#include <utility>

namespace Server::Exceptions {
    class EndpointException : public std::exception {
    public:
        explicit EndpointException(std::string message) : message(std::move(message)) {}

        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message;
    };

    class EndpointNotFoundException : public EndpointException {
    public:
        explicit EndpointNotFoundException(int8_t endpoint_byte) : EndpointException("Endpoint not found: " + std::to_string(endpoint_byte)) {}
    };

}

#endif //ASYNC_SERVER_EXAMPLE_ENDPOINTEXCEPTIONS_H
