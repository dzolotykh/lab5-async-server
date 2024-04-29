#include <exceptions/SocketExceptions.h>
#include <files/FileManager.h>
#include <server/Server.h>
#include <server/handlers/AmogusHandler.h>
#include <server/handlers/DownloadHandler.h>
#include <server/handlers/GenerationHandler.h>
#include <server/handlers/UploadHandler.h>
#include <socket/ClientSocket.h>
#include <socket/ListenerSocket.h>
#include <csignal>
#include <iostream>
#include <thread>
#include "server/handlers/EchoHandler.h"

class ServerRunner {
   public:
    ServerRunner() {
        instance = this;
        std::signal(SIGTERM, sigterm_handler);
    };

    static void sigterm_handler(int) {
        instance->stop_server();
        exit(0);
    }

    void run_server() {
        server_thread = std::thread([this]() {
            try {
                server.set_endpoint<Server::Handlers::EchoHandler>('e');
                server.set_endpoint<Server::Handlers::UploadHandler>('u', fm);
                server.set_endpoint<Server::Handlers::DownloadHandler>('d', fm);
                server.set_endpoint<Server::Handlers::GenerationHandler>('g', fm, pool);
                server.set_endpoint<Server::Handlers::Sus::AmogusHandler>('i');
                server.start();
            } catch (std::exception& e) {
                std::cerr << "An error occurred: " << e.what() << std::endl;
            }
        });
    }

    void stop_server() {
        server.stop();
        server_thread.join();
    }

   private:
    static const int num_threads = 8;
    static const int num_threads_in_generation = 8;
    static const int port = 8080;
    static const int max_conn_queued = 1024;
    Server::Server server{port, max_conn_queued, num_threads};
    Server::Files::FileManager fm{"../uploads"};
    Server::Multithreading::ThreadPool pool{num_threads_in_generation};
    std::thread server_thread;

    static ServerRunner* instance;
};

ServerRunner* ServerRunner::instance = nullptr;

int main() {
    ServerRunner runner;
    runner.run_server();
    std::string command;
    while (std::cin >> command) {
        if (command == "exit") {
            runner.stop_server();
            break;
        }
    }
}