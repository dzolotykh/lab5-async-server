#include <server/handlers/DownloadHandler.h>

Server::Handlers::DownloadHandler::DownloadHandler(const Server::ClientSocket &_client,
                                                   Server::Files::FileManager &_fm)
    : client(_client), fm(_fm), buffer(1024 * 1024) {}

Server::Response Server::Handlers::DownloadHandler::handle() {
    auto &logger = *Logging::Logger::get_instance();
    std::string filename;
    filename.resize(32);
    client.read_bytes(filename.data(), 32);
    logger << "Пользователь " + client.get_info() + " запросил файл " + filename + "\n";
    std::ifstream file(fm.get_dir() / filename, std::ios::binary);
    if (!file.is_open()) {
        logger << "Файл для пользователя " + client.get_info() + " не найден\n";
        return not_found_response();
    }
    int file_size = std::filesystem::file_size(std::filesystem::absolute(fm.get_dir() / filename));
    client.send_bytes(reinterpret_cast<const char *>(&file_size), sizeof(int));
    logger << "Отправка файла пользователю " + client.get_info() +
                  ". Размер файла: " + std::to_string(file_size) + " байт\n";
    while (!file.eof()) {
        file.read(buffer.data(), buffer.size());
        client.send_bytes(buffer.data(), file.gcount());
    }
    logger << "Файл для пользователя " + client.get_info() + " отправлен\n";
    return empty_response();
}
