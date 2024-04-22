#include <Server.h>
#include <iostream>

#include "EchoHandler.h"
#include "LongRequestHandler.h"

int main() {
    const int port = 8081;
    const int max_connections_in_queue = 1000;
    const int working_threads = 6;
    auto logger = [](const std::string& s) {
        std::cout << s << std::endl;
    };
    Server::Params params(port, logger, max_connections_in_queue, working_threads);
    Server::Server serv(params);
    serv.add_endpoint<EchoHandler>('e');
    serv.add_endpoint<LongRequestHandler>('l', "finished");
    auto server_thread = std::thread([&serv]() { serv.start(); });
    std::string command;
    std::cin >> command;
    if (command == "stop") {
        auto stopping_thread = serv.stop(std::chrono::milliseconds(100));
        stopping_thread.join();
    }
    server_thread.join();
}
