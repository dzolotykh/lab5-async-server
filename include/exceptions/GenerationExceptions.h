#ifndef ASYNC_SERVER_EXAMPLE_GENERATIONEXCEPTIONS_H
#define ASYNC_SERVER_EXAMPLE_GENERATIONEXCEPTIONS_H

#include <exception>
#include <string>

namespace Server::Exceptions {
    class InvalidInputException : public std::exception {
    public:
        explicit InvalidInputException(std::string _message) : message(std::move(_message)) {}

        const char *what() const noexcept override {
            return message.c_str();
        }

    protected:
        std::string message;
    };
}

#endif //ASYNC_SERVER_EXAMPLE_GENERATIONEXCEPTIONS_H
