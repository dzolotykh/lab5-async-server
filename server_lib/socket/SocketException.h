#ifndef ASYNC_SERVER_EXAMPLE_SOCKETEXCEPTION_H
#define ASYNC_SERVER_EXAMPLE_SOCKETEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

namespace Server {
    class SocketException : public std::exception {
    public:
        explicit SocketException(std::string message);

        [[nodiscard]] const char *what() const noexcept override;
    private:
        std::string message;
    };
}


#endif //ASYNC_SERVER_EXAMPLE_SOCKETEXCEPTION_H
