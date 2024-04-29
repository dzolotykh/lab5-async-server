#include <server/handlers/UploadHandler.h>

namespace Server::Handlers {

UploadHandler::UploadHandler(const ClientSocket &_client, Files::FileManager &_fm)
    : client(_client), fm(_fm), buffer(1024 * 1024) {}

Response UploadHandler::handle() {
    auto &logger = *Logging::Logger::get_instance();
    int32_t file_size;
    client.read_bytes(reinterpret_cast<char *>(&file_size), sizeof(int32_t));
    if (file_size < 0 || file_size > 1024 * 1024) {
        logger << "Пользователь " + client.get_info() +
                      " попытался загрузить файл неправильного размера\n";
        throw Exceptions::EndpointException("Invalid file size");
    }
    auto filename = fm.create_unique();
    std::ofstream file(filename);
    logger << "Пользователь " + client.get_info() + " загружает файл " +
                  filename.filename().string() + "\n";
    if (!file.is_open()) {
        logger << "Не удалось открыть файл для пользователя " + client.get_info() + "\n";
        throw Exceptions::EndpointException("Can't open file");
    }
    size_t bytes_read = 0;
    while (bytes_read < file_size) {
        size_t bytes_to_read = std::min(buffer.size(), file_size - bytes_read);
        client.read_bytes(buffer.data(), bytes_to_read);
        file.write(buffer.data(), bytes_to_read);
        bytes_read += bytes_to_read;
        logger << "Загружено " + std::to_string(bytes_read) + " байт для пользователя " +
                      client.get_info() + "\n";
    }
    file.close();
    logger << "Файл для пользователя " + client.get_info() + " загружен\n";
    return Response{filename.filename()};
}
}    // namespace Server::Handlers