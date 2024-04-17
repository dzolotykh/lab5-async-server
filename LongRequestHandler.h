#ifndef ASYNC_SERVER_EXAMPLE_LONGREQUESTHANDLER_H
#define ASYNC_SERVER_EXAMPLE_LONGREQUESTHANDLER_H

#include <Server.h>
#include <thread>
#include <chrono>

class LongRequestHandler: public Server::AbstractHandler {
public:
    LongRequestHandler(Server::socket_t _client, std::string _return_message);
    std::string get_response() final;
    bool operator()() final;
private:
    std::chrono::time_point<std::chrono::system_clock> time_started;
    std::string return_message;
    Server::socket_t client;
};


#endif //ASYNC_SERVER_EXAMPLE_LONGREQUESTHANDLER_H
