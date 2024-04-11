#include <ConnectionPool.h>
#include <json/json.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include "src/server/Server.h"

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
    // TODO сделать проверку конфигурации сервера
    Json::Value config;
    std::ifstream config_doc("../config.json", std::ifstream::binary);
    config_doc >> config;
    Json::Value server_cfg = config["server"];

    Server::Params params(server_cfg["port"].asInt(), logger,
                          server_cfg["max_connections_in_queue"].asInt(),
                          server_cfg["working_threads"].asInt());
    Server::Server server(params);
    try {
        run_server(server);
    } catch (const std::runtime_error& err) {
        logger("Ошибка: " + std::string(err.what()));
        return 0;
    }
}