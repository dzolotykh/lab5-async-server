#ifndef ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H
#define ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H

#include <server/handlers/IHandler.h>
#include <socket/ClientSocket.h>
#include <files/FileManager.h>
#include <fstream>
#include <exceptions/EndpointExceptions.h>
#include <vector>

namespace Server::Handlers {
    class UploadHandler final : public IHandler {
    public:
        explicit UploadHandler(const ClientSocket& _client, Files::FileManager& _fm);

        Response handle() override;
    private:
        const ClientSocket& client;
        Files::FileManager& fm;

        std::vector<char> buffer;
    };
}

#endif //ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H
