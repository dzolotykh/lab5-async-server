#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include "src/server/Server.h"
#include <pqxx/pqxx>

auto logger = [](const std::string& s) {
    auto now = std::chrono::system_clock::now();
    std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
    std::cout << std::put_time(std::localtime(&raw_time), "[%Y-%m-%d %X] ");
    std::cout << s << std::endl;
};

void run_server(Server::Server& serv) {
    serv.start();
}

int main() {
    Server::Params params(8081, logger, 10, 10, 1024 * 1024);
    Server::Server server(params);
    try {
        run_server(server);
    } catch (const std::runtime_error& err) {
        logger("Ошибка: " + std::string(err.what()));
        return 0;
    }
}