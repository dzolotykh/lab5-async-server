#include "src/server/Server.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

auto logger = [](const std::string& s)
{
    auto   now = std::chrono::system_clock::now();
    time_t raw_time = std::chrono::system_clock::to_time_t(now);
    std::cout << std::put_time(std::localtime(&raw_time), "[%Y-%m-%d %X] ");
    std::cout << s << std::endl;
};

void run_server(Server& serv)
{
    serv.start();
}

int main()
{
    ServerParams params(8081, logger, 10, 10, 1024 * 1024);
    Server       server(params);
    std::thread  server_thread(run_server, std::ref(server));
    while (true)
    {
        std::string command;
        std::cin >> command;
        if (command == "exit")
        {
            server.gentle_shutdown();
            break;
        }
    }

    server_thread.join();
    return 0;
}