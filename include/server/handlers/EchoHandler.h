#ifndef ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H
#define ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H

#include <exceptions/SocketExceptions.h>
#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>

namespace Server::Handlers {
class EchoHandler final : public IHandler {
   public:
    explicit EchoHandler(const ClientSocket& _client) : IHandler(_client) {}

    Response handle() override;
};
}    // namespace Server::Handlers

#endif    //ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H
