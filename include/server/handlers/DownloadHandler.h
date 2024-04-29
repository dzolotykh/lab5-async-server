#ifndef ASYNC_SERVER_EXAMPLE_DOWNLOADHANDLER_H
#define ASYNC_SERVER_EXAMPLE_DOWNLOADHANDLER_H

#include <files/FileManager.h>
#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>
#include <fstream>

namespace Server::Handlers {
class DownloadHandler final : public IHandler {
   public:
    DownloadHandler(const ClientSocket& _client, Files::FileManager& _fm);

    Response handle() override;

   private:
    const ClientSocket& client;
    Files::FileManager& fm;

    std::vector<char> buffer;
};
}    // namespace Server::Handlers

#endif    //ASYNC_SERVER_EXAMPLE_DOWNLOADHANDLER_H
