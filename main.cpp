#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <ctime>
#include <thread>
#include <chrono>
#include <iomanip>
#include <string>
#include <sstream>
#include <memory>

template<typename logger_t>
struct ServerParams {
    ServerParams(unsigned int port, logger_t logger, unsigned int max_connections_in_queue, unsigned int max_process,
                 unsigned int buff_size) : port(port), logger(logger),
                                           max_connections_in_queue(max_connections_in_queue), max_process(max_process),
                                           buff_size(buff_size) {}

    unsigned int port;
    logger_t logger;
    unsigned int max_connections_in_queue;
    unsigned int max_process;
    unsigned int buff_size;

};

template<typename logger_t>
void create_server(ServerParams<logger_t> params) {
    auto& logger = params.logger;

    if (params.port <= 0) {
        throw std::invalid_argument("Номер порта должен быть больше нуля");
    }

    auto buff = std::make_unique<char*>(new char[params.buff_size]);

    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(params.port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_descriptor < 0) {
        throw std::runtime_error(
                "Ошибка при создании сокета. Код ошибки: " + std::to_string(errno)
        );
    }

    int bind_status = bind(socket_descriptor, (sockaddr* )&socket_address, sizeof(socket_address));

    if (bind_status < 0) {
        throw std::runtime_error(
                "Произошла ошибка при связывании сокета с адресом. Код ошибки: " + std::to_string(errno)
                );
    }

    int listen_status = listen(socket_descriptor, params.max_connections_in_queue);

    if (listen_status < 0) {
        throw std::runtime_error("Произошла ошибка при прослушивании сокета. Код ошибки: " + std::to_string(errno));
    }

    std::stringstream hello;
    hello << "\n✅ Сервер запущен. Параметры: \n";
    hello << "⚡️ Порт: " << params.port << "\n";
    hello << "⚡️ Максимальное количество подключений в очереди: " << params.max_connections_in_queue << "\n";
    hello << "⚡️ Максимальное количество одновременно работающих процессов: " <<  params.max_process << "\n";

    logger(hello.str());

    while (true) {
        int sock = accept(socket_descriptor, nullptr, nullptr);
        if (sock < 0) {
            throw std::runtime_error("Произошла ошибка при соединении сокета с адресом. Код ошибки: " + std::to_string(errno));
        }
        logger("Подключился.... ");
        while (true) {
            size_t bytes_read = recv(sock, buff.get(), 1024, 0);
            if(bytes_read <= 0)
                break;
            send(sock, buff.get(), bytes_read, 0);
        }
        logger("Отключился");
        shutdown(sock, SHUT_RDWR);
    }
}

int main() {
    auto logger = [](const std::string &s) {
        auto now = std::chrono::system_clock::now();
        time_t raw_time = std::chrono::system_clock::to_time_t(now);
        std::cout << std::put_time(std::localtime(&raw_time), "[%Y-%m-%d %X] ");
        std::cout << s << std::endl;
    };

    ServerParams<decltype(logger)> p(
            8081,
            logger,
            1,
            1,
            1024
    );

    std::thread server_thread([&p]() {
        create_server(p);
    });

    while (1) {
        // chill
    }
}