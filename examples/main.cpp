#include <iostream>
#include <socket/ListenerSocket.h>
#include <socket/ClientSocket.h>
#include <exceptions/SocketExceptions.h>
#include <thread>
#include <server/Server.h>
#include "server/handlers/EchoHandler.h"

int main() {
    try {
        Server::Server server(8081, 10);
        server.set_endpoint<Server::Handlers::EchoHandler>('e');
        server.start();
    } catch (std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }
}