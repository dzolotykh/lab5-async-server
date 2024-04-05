#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

#include <Pool.h>

template <typename logger_t>
struct ServerParams
{
    ServerParams(unsigned int port,
                 logger_t     logger,
                 unsigned int max_connections_in_queue,
                 unsigned int max_process,
                 unsigned int buff_size)
        : port(port), logger(logger), max_connections_in_queue(max_connections_in_queue),
          max_process(max_process), buff_size(buff_size)
    {
    }

    unsigned int port;
    logger_t     logger;
    unsigned int max_connections_in_queue;
    unsigned int max_process;
    unsigned int buff_size;
};

template <typename logger_t>
void create_server(ServerParams<logger_t> params)
{
    auto& logger = params.logger;

    if (params.port <= 0)
    {
        throw std::invalid_argument("Номер порта должен быть больше нуля");
    }

    auto buff = std::make_unique<char*>(new char[params.buff_size]);

    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(params.port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_descriptor < 0)
    {
        throw std::runtime_error("Ошибка при создании сокета. Код ошибки: " +
                                 std::to_string(errno));
    }

    int bind_status = bind(socket_descriptor, (sockaddr*)&socket_address, sizeof(socket_address));

    if (bind_status < 0)
    {
        throw std::runtime_error("Произошла ошибка при связывании сокета с адресом. Код ошибки: " +
                                 std::to_string(errno));
    }

    int listen_status = listen(socket_descriptor, params.max_connections_in_queue);

    if (listen_status < 0)
    {
        throw std::runtime_error("Произошла ошибка при прослушивании сокета. Код ошибки: " +
                                 std::to_string(errno));
    }

    std::stringstream hello;
    hello << "\n✅ Сервер запущен. Параметры: \n";
    hello << "⚡️ Порт: " << params.port << "\n";
    hello << "⚡️ Максимальное количество подключений в очереди: " << params.max_connections_in_queue
          << "\n";
    hello << "⚡️ Максимальное количество одновременно работающих процессов: " << params.max_process
          << "\n";

    logger(hello.str());

    ThreadPool::Pool pool(params.max_process, ThreadPool::Pool::destructor_policy::JOIN);
    std::mutex       logger_mutex;
    while (true)
    {
        int sock = accept(socket_descriptor, nullptr, nullptr);
        if (sock < 0)
        {
            throw std::runtime_error(
                "Произошла ошибка при соединении сокета с адресом. Код ошибки: " +
                std::to_string(errno));
        }

        pool.add_task(
            [sock, &logger, &logger_mutex, &buff, &params]()
            {
                auto logger_safe = [&logger, &logger_mutex](const std::string& s)
                {
                    std::lock_guard<std::mutex> lock(logger_mutex);
                    logger(s);
                };
                logger_safe("Подключился.... ");
                while (true)
                {
                    size_t bytes_read = recv(sock, buff.get(), params.buff_size, 0);
                    if (bytes2_read <= 0)
                        break;
                    send(sock, buff.get(), bytes_read, 0);
                }
                logger_safe("Отключился");
                shutdown(sock, SHUT_RDWR);
            });
    }
}

void run_serv()
{
    auto logger = [](const std::string& s)
    {
        auto   now = std::chrono::system_clock::now();
        time_t raw_time = std::chrono::system_clock::to_time_t(now);
        std::cout << std::put_time(std::localtime(&raw_time), "[%Y-%m-%d %X] ");
        std::cout << s << std::endl;
    };

    ServerParams<decltype(logger)> p(8082, logger, 10, 10, 1024);

    std::thread server_thread([&p]() { create_server(p); });

    server_thread.join();
}

using namespace std;

mutex mtx;
int   counter = 0;

void func()
{
    this_thread::sleep_for(chrono::milliseconds(1000));
    lock_guard lock(mtx);
    counter++;
    cout << counter << endl;
}

int main()
{
    auto             start = chrono::system_clock::now();
    ThreadPool::Pool pool(10, ThreadPool::Pool::destructor_policy::JOIN);
    for (int i = 0; i < 100; i++)
    {
        pool.add_task(func);
    }
    pool.wait_all();
    auto                     end = chrono::system_clock::now();
    chrono::duration<double> elapsed_milliseconds = end - start;
    cout << "Elapsed time: " << elapsed_milliseconds.count() << "s" << endl;
}