#include <exceptions/SocketExceptions.h>
#include <files/FileManager.h>
#include <logging/Logger.h>
#include <server/Server.h>
#include <server/handlers/AmogusHandler.h>
#include <server/handlers/DownloadHandler.h>
#include <server/handlers/GenerationHandler.h>
#include <server/handlers/UploadHandler.h>
#include <socket/ClientSocket.h>
#include <socket/ListenerSocket.h>
#include <csignal>
#include <iostream>
#include <sstream>
#include <thread>
#include "server/handlers/EchoHandler.h"

using namespace Server::Logging;

class ServerRunner {
   public:
    ServerRunner() {
        instance = this;
        std::signal(SIGTERM, sigterm_handler);
        std::signal(SIGINT, sigterm_handler);
    };

    static void sigterm_handler(int) {
        instance->stop_server();
        auto& logger = *Logger::get_instance();
        logger << "💤 Сервер остановлен\n";
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
                auto& logger = *Logger::get_instance();

                std::stringstream hello_msg;
                hello_msg << "🚀 Сервер запущен. Порт: " << ServerRunner::port << "\n";
                hello_msg << "Количество потоков сервера: " << ServerRunner::num_threads << "\n";
                hello_msg << "Количество потоков в пуле для генераций: "
                          << ServerRunner::num_threads_in_generation << "\n";
                hello_msg << "Максимальное количество подключений в очереди: "
                          << ServerRunner::max_conn_queued << "\n";
                logger << hello_msg.str();
                server.start();
            } catch (std::exception& e) {
                auto& logger = *Logger::get_instance();
                logger << "❌ Ошибка: " + std::string(e.what()) << "\n";
            }
        });
    }

    void stop_server() {
        server.stop();
        server_thread.join();
    }

   private:
    const int num_threads = 8;
    const int num_threads_in_generation = 8;
    const int port = 8080;
    const int max_conn_queued = 1024;
    Server::Server server{static_cast<uint16_t>(port), max_conn_queued, num_threads};
    Server::Files::FileManager fm{"../uploads"};
    Server::Multithreading::ThreadPool pool{static_cast<size_t>(num_threads_in_generation)};
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