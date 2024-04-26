#ifndef ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H
#define ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H

#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>
#include <exceptions/SocketExceptions.h>

namespace Server::Handlers {
    class EchoHandler : public IHandler {
        public:
        explicit EchoHandler(const ClientSocket& _client): client(_client) {}

        Response handle() override;
    private:
        const ClientSocket& client;
    };
}


#endif //ASYNC_SERVER_EXAMPLE_ECHOHANDLER_H
