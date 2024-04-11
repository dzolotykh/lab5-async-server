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

std::string get_env_var(const std::string& key) {
    char* val = std::getenv(key.c_str());
    return val == nullptr ? std::string("") : std::string(val);
}

int main() {
    // TODO сделать проверку конфигурации сервера
    Json::Value config;
    std::ifstream config_doc("../config.json", std::ifstream::binary);
    config_doc >> config;

    Json::Value server_cfg = config["server"];
    Json::Value database_cfg = config["database"];

    std::string db_host = get_env_var(database_cfg["host"].asString());
    std::string db_port = get_env_var(database_cfg["port"].asString());
    std::string db_user = get_env_var(database_cfg["user"].asString());
    std::string db_password = get_env_var(database_cfg["password"].asString());
    std::string db_name = get_env_var(database_cfg["name"].asString());

    Database::ConnectionPool pool(database_cfg["num_connections"].asInt(),
                                  "host=" + db_host + " port=" + db_port + " user=" + db_user +
                                      " password=" + db_password + " dbname=" + db_name);

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