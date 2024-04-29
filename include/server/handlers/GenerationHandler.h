#ifndef ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H
#define ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H

#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>
#include <files/FileManager.h>
#include <multithreading/ThreadPool.h>
#include <exceptions/SocketExceptions.h>
#include <exceptions/GenerationExceptions.h>
#include <set>
#include <functional>
#include <graphgen.h>

namespace Server::Handlers {
    class GenerationHandler final : public IHandler {
    public:
        GenerationHandler(const ClientSocket& _client, Files::FileManager& _fm, Multithreading::ThreadPool& _tp);

        Response handle() override;
    private:
        const ClientSocket& client;
        Files::FileManager& fm;
        Multithreading::ThreadPool& tp;
        std::string input_data;
    };
}

#endif //ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H
