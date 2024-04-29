#include <logging/Logger.h>

namespace Server::Logging {

Logger::Logger(std::ostream &stream) : stream(stream) {}

void Logger::commit() noexcept {
    is_first = true;
}

void Logger::print_time() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    stream << std::put_time(std::localtime(&in_time_t), "[%H:%M:%S %d.%m.%Y]") << " ";
}
}    // namespace Server::Logging