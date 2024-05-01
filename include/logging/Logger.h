#ifndef ASYNC_SERVER_EXAMPLE_LOGGER_H
#define ASYNC_SERVER_EXAMPLE_LOGGER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>

namespace Server::Logging {
// Logger не является потокобезопасным при инициализации. Для остальных операций (вывод сообщений) он потокобезопасен.
// Поэтому, если хочется использовать его в многопоточной среде, то перед запуском потоков нужно вызвать Logger::get_instance();
class Logger {
   public:
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger* get_instance() {
        static Logger instance;
        return &instance;
    }

    template <typename T>
    Logger& operator<<(const T& message) {
        std::lock_guard<std::mutex> lock(mtx);
        get_stream() << get_time() << " " << message;
        return *this;
    }

   private:
    Logger() = default;
    bool is_first = true;
    std::mutex mtx{};

    static std::ostream& get_stream() { return std::cout; }

    static std::string get_time() {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        auto now_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&now_c), "%d.%m.%Y") << " | "
           << std::put_time(std::localtime(&now_c), "%T:") << std::setfill('0') << std::setw(3)
           << now_ms.count() << "]";
        return ss.str();
    }
};
}    // namespace Server::Logging
#endif    //ASYNC_SERVER_EXAMPLE_LOGGER_H
