#include <ConnectionPool.h>
#include <FileDownloadHandler.h>
#include <FileUploadHandler.h>
#include <RequestGenerationHandler.h>
#include <ResultRequestHandler.h>
#include <json/json.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include "src/server/Server.h"
#include "src/server/typenames.h"

auto logger = [](const std::string& s) {
    auto now = std::chrono::system_clock::now();
    std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
    std::cout << std::put_time(std::localtime(&raw_time), "[%Y-%m-%d %X] ");
    std::cout << s << std::endl;
};

Json::Value config;

/// \brief Немного о том, каким должен быть обработчик
/// Обработчик должен быть наследником AbstractHandler.
/// При этом, конструктор у него может быть любым. Главное, чтобы он корректно переопределил
/// оператор круглых скобок. Проблема в том, что у нас на каждого клиента создается свой экземпляр
/// класса-обработчика. Значит, ответственность за создание этого класса надо возложить на человека,
/// который задает эндпоинт со своим кастомным обработчиком. Получается, в add_endpoint
/// надо передать символ эндпоинта и функцию, которая будет порождать нам обработчик.

void run_server(Server::Server& serv, Database::ConnectionPool& pool) {
    serv.add_endpoint('u', [&pool](Server::socket_t client) {
        return std::make_unique<Server::FileUploadHandler>(
            client, pool, config["file-uploader"]["upload_dir"].asString(),
            config["file-uploader"]["max_file_size"].asInt());
    });

    serv.add_endpoint('r', [&pool](Server::socket_t client) {
        return std::make_unique<Server::ResultRequestHandler>(client, pool);
    });

    serv.add_endpoint('d', [&pool](Server::socket_t client) {
        return std::make_unique<Server::FileDownloadHandler>(client, pool);
    });

    serv.add_endpoint('g', [&pool](Server::socket_t client) {
        return std::make_unique<Server::RequestGenerationHandler>(client, pool);
    });

    serv.start();
}

std::string get_env_var(const std::string& key) {
    char* val = std::getenv(key.c_str());
    return val == nullptr ? std::string("") : std::string(val);
}

int main() {
    // TODO сделать проверку конфигурации сервера
    std::ifstream config_doc("../config.json", std::ifstream::binary);
    config_doc >> config;

    Json::Value server_cfg = config["server"];
    Json::Value database_cfg = config["database"];
    Json::Value file_uploader_cfg = config["file-uploader"];

    std::string file_uploader_path = file_uploader_cfg["path"].asString();

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
        run_server(server, pool);
    } catch (const std::runtime_error& err) {
        logger("Ошибка: " + std::string(err.what()));
        return 0;
    }
}