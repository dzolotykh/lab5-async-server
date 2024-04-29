#include <exceptions/SocketExceptions.h>
#include <files/FileManager.h>
#include <server/Server.h>
#include <server/handlers/DownloadHandler.h>
#include <server/handlers/GenerationHandler.h>
#include <server/handlers/UploadHandler.h>
#include <socket/ClientSocket.h>
#include <socket/ListenerSocket.h>
#include <iostream>
#include <thread>
#include "server/handlers/EchoHandler.h"

int main() {
    Server::Files::FileManager fm("../uploads");
    try {
        Server::Server server(8080, 10, 5);
        Server::Multithreading::ThreadPool pool(4);
        server.set_endpoint<Server::Handlers::EchoHandler>('e');
        server.set_endpoint<Server::Handlers::UploadHandler>('u', fm);
        server.set_endpoint<Server::Handlers::DownloadHandler>('d', fm);
        server.set_endpoint<Server::Handlers::GenerationHandler>('g', fm, pool);
        server.start();
    } catch (std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }
}