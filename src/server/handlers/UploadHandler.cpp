#include <server/handlers/UploadHandler.h>

namespace Server::Handlers {

    UploadHandler::UploadHandler(const ClientSocket &_client, Files::FileManager &_fm) : client(_client), fm(_fm), buffer(1024 * 1024) {}

    Response UploadHandler::handle() {
        auto filename = fm.create_unique();
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw Exceptions::EndpointException("Can't open file");
        }
        int32_t file_size;
        client.read_bytes(reinterpret_cast<char*>(&file_size), sizeof(int32_t));
        size_t bytes_read = 0;
        while (bytes_read < file_size) {
            size_t bytes_to_read = std::min(buffer.size(), file_size - bytes_read);
            client.read_bytes(buffer.data(), bytes_to_read);
            file.write(buffer.data(), bytes_to_read);
            bytes_read += bytes_to_read;
        }
        file.close();
        return Response{filename.filename()};
    }
}