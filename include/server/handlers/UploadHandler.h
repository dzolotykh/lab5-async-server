#ifndef ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H
#define ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H

#include <exceptions/EndpointExceptions.h>
#include <files/FileManager.h>
#include <logging/Logger.h>
#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>
#include <fstream>
#include <vector>

namespace Server::Handlers {
class UploadHandler final : public IHandler {
   public:
    explicit UploadHandler(const ClientSocket& _client, Files::FileManager& _fm);

    Response handle() override;

   private:
    Files::FileManager& fm;

    std::vector<char> buffer;
};
}    // namespace Server::Handlers

#endif    //ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H
