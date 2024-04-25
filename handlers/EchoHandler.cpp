#include "EchoHandler.h"
#include <iostream>

EchoHandler::EchoHandler(const Server::Socket& _client) : client(_client) {
    reader = client.read_bytes_nonblock(buff.data(), buff.size(),
                                        [this](size_t read) {
        written_in_buffer = read;
    });
    writer = client.write_bytes_nonblock(0, [this]() { return std::make_pair(buff.data(), written_in_buffer); });
}

bool EchoHandler::operator()() {
    if (!writer()) {
        bool is_disconnected = !reader();
        if (is_disconnected) {
            return false;
        }
        writer = client.write_bytes_nonblock(written_in_buffer, [ptr = buff.data(),w= written_in_buffer]() {
            return std::make_pair(ptr, w);
        });
        written_in_buffer = 0;
    }
    writer();
    return true;
}

std::string EchoHandler::get_response() {
    return "";
}
