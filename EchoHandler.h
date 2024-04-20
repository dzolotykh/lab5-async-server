#ifndef ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H
#define ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H

#include <Server.h>

class EchoHandler : public Server::AbstractHandler {
   public:
    explicit EchoHandler(const Server::Socket& _client);

    std::string get_response() final;

    bool operator()() final;

   private:
    const Server::Socket& client;
    std::function<bool()> reader;
    std::function<bool()> writer;
    std::array<char, 1024> buff;
    size_t written_in_buffer;
};

#endif    //ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H
