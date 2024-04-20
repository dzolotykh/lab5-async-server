#ifndef ASYNC_SERVER_EXAMPLE_LONGREQUESTHANDLER_H
#define ASYNC_SERVER_EXAMPLE_LONGREQUESTHANDLER_H

#include <Server.h>
#include <chrono>
#include <thread>

class LongRequestHandler : public Server::AbstractHandler {
   public:
    LongRequestHandler(const Server::Socket& _client, std::string _return_message);
    std::string get_response() final;
    bool operator()() final;

   private:
    std::chrono::time_point<std::chrono::system_clock> time_started;
    std::string return_message;
    const Server::Socket& client;
};

#endif    //ASYNC_SERVER_EXAMPLE_LONGREQUESTHANDLER_H
