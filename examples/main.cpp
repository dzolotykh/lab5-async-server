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
#include <server/handlers/AmogusHandler.h>

int main() {
    Server::Files::FileManager fm("../uploads");
    const int num_threads = 8;
    const int num_threads_in_generation = 8;
    try {
        Server::Server server(8080, 1024, num_threads);
        Server::Multithreading::ThreadPool pool(num_threads_in_generation);
        server.set_endpoint<Server::Handlers::EchoHandler>('e');
        server.set_endpoint<Server::Handlers::UploadHandler>('u', fm);
        server.set_endpoint<Server::Handlers::DownloadHandler>('d', fm);
        server.set_endpoint<Server::Handlers::GenerationHandler>('g', fm, pool);
        server.set_endpoint<Server::Handlers::Sus::AmogusHandler>('i');
        server.start();
    } catch (std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }
}