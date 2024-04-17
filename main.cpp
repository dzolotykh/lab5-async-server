#include <Server.h>
#include <functional>
#include <array>
#include <iostream>

class EchoHandler: public Server::AbstractHandler {
public:
    explicit EchoHandler(Server::socket_t _client): client(_client) {
        reader = Server::AbstractHandler::read_bytes_nonblock(client, buff.data(), buff.size(), [this](size_t read){
            written_in_buffer = read;
        });
        writer = Server::AbstractHandler::write_bytes_nonblock(client, 0, [this](){
            return std::make_pair(buff.data(), 0);
        });
    }

    std::string get_response() final {return "";}

    bool operator()() final {
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
private:
    Server::socket_t client;
    std::function<bool()> reader;
    std::function<bool()> writer;
    std::array<char, 1024> buff;
    size_t written_in_buffer;
};

int main() {
    const int port = 8082;
    const int max_connections_in_queue = 1000;
    const int working_threads = 6;
    auto logger = [](const std::string& s) {
        std::cout << s << std::endl;
    };
    Server::Params params(port, logger, max_connections_in_queue, working_threads);
    Server::Server serv(params);
    serv.add_endpoint<EchoHandler>('e');
    serv.start();
}