#ifndef LAB5_EXCEPTIONS_H
#define LAB5_EXCEPTIONS_H

#include <exception>
#include <string>
#include <utility>

namespace Server {
class HandlerException : public std::exception {
   public:
    explicit HandlerException(std::string message) : message(std::move(message)) {}

    [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }

   private:
    std::string message;
};

class BadInputException : public HandlerException {
   public:
    explicit BadInputException(const std::string& message) : HandlerException(message) {}
};

class NotFoundException : public HandlerException {
   public:
    explicit NotFoundException(const std::string& message) : HandlerException(message) {}
};
}    // namespace Server

#endif    //LAB5_EXCEPTIONS_H
