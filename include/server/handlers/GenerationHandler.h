#ifndef ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H
#define ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H

#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>
#include <functional>

namespace Server::Handlers {
    class GenerationHandler final : public IHandler {
    public:
        GenerationHandler(const ClientSocket& _client);

        Response handle() override;
    private:
        const ClientSocket& client;
    };
}

#endif //ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H
