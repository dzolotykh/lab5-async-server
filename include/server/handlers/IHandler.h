#ifndef ASYNC_SERVER_EXAMPLE_IHANDLER_H
#define ASYNC_SERVER_EXAMPLE_IHANDLER_H

#include <server/Response.h>
#include <socket/ClientSocket.h>

namespace Server::Handlers {
class IHandler {
   public:
    virtual Response handle() = 0;
    virtual ~IHandler() = default;

   protected:
    explicit IHandler(const ClientSocket& _client) : client(_client) {}

    const ClientSocket& client;
};
}    // namespace Server::Handlers

#endif    //ASYNC_SERVER_EXAMPLE_IHANDLER_H
