#include "EchoHandler.h"

 EchoHandler::EchoHandler(Server::socket_t _client) : client(_client) {
    reader = Server::AbstractHandler::read_bytes_nonblock(client, buff.data(), buff.size(), [this](size_t read){
        written_in_buffer = read;
    });
    writer = Server::AbstractHandler::write_bytes_nonblock(client, 0, [this](){
        return std::make_pair(buff.data(), 0);
    });
}

bool EchoHandler::operator()() {
    if (!writer()) {
        bool is_disconnected = !reader();
        if (is_disconnected) {
            return false;
        }
        writer = Server::AbstractHandler::write_bytes_nonblock(client, written_in_buffer, [this](){
            return std::make_pair(buff.data(), written_in_buffer);
        });
    }
    writer();
    return true;
}

std::string EchoHandler::get_response() {return "";}
