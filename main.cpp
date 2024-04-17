#include <Server.h>
#include <iostream>

#include "EchoHandler.h"
#include "LongRequestHandler.h"

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
    serv.add_endpoint<LongRequestHandler>('l', "finished");
    serv.start();
}