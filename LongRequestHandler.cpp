#include "LongRequestHandler.h"

std::string LongRequestHandler::get_response() {
    return return_message;
}

bool LongRequestHandler::operator()() {
    auto after_start = std::chrono::system_clock::now() - time_started;
    return std::chrono::duration_cast<std::chrono::milliseconds>(after_start) < std::chrono::milliseconds(2000);
}

LongRequestHandler::LongRequestHandler(Server::socket_t _client, std::string _return_message): client(_client), return_message(std::move(_return_message)) {
    time_started = std::chrono::system_clock::now();
}
