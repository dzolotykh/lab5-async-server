#include <server/handlers/DownloadHandler.h>

Server::Handlers::DownloadHandler::DownloadHandler(const Server::ClientSocket &_client, Server::Files::FileManager &_fm)
        : client(_client), fm(_fm), buffer(1024 * 1024) {}

Server::Response Server::Handlers::DownloadHandler::handle() {
    std::string filename;
    filename.reserve(32);
    client.read_bytes(filename.data(), 32);
    std::ifstream file(fm.get_dir() / filename, std::ios::binary);
    if (!file.is_open()) {
        return not_found_response();
    }
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    client.send_bytes(reinterpret_cast<const char*>(&file_size), sizeof(size_t));
    while (!file.eof()) {
        file.read(buffer.data(), buffer.size());
        client.send_bytes(buffer.data(), file.gcount());
    }
    return empty_response();
}
