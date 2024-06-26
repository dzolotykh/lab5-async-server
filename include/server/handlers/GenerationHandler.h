#ifndef ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H
#define ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H

#include <exceptions/GenerationExceptions.h>
#include <exceptions/SocketExceptions.h>
#include <files/FileManager.h>
#include <graphgen.h>
#include <logging/Logger.h>
#include <multithreading/ThreadPool.h>
#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>
#include <functional>
#include <set>

namespace Server::Handlers {
class GenerationHandler final : public IHandler {
   public:
    GenerationHandler(const ClientSocket& _client, Files::FileManager& _fm,
                      Multithreading::ThreadPool& _tp);

    Response handle() override;

   private:
    Files::FileManager& fm;
    Multithreading::ThreadPool& tp;
    std::string input_data;
};
}    // namespace Server::Handlers

#endif    //ASYNC_SERVER_EXAMPLE_GENERATIONHANDLER_H
