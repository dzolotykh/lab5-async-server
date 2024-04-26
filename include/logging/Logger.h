#ifndef ASYNC_SERVER_EXAMPLE_LOGGER_H
#define ASYNC_SERVER_EXAMPLE_LOGGER_H

#include <string>
#include <ostream>
#include <chrono>
#include <iomanip>
#include <sstream>
namespace Server::Logging {
    class Logger {
    public:
        explicit Logger(std::ostream &stream);

        template<typename T>
        Logger &operator<<(const T &item) {
            if (is_first) {
                print_time();
                is_first = false;
            }
            stream << item;
            return *this;
        }

        void commit() noexcept;

    private:
        std::ostream &stream;
        bool is_first = true;

        void print_time();
    };
}
#endif //ASYNC_SERVER_EXAMPLE_LOGGER_H
