#ifndef LAB5_FILEDOWNLOADHANDLER_H
#define LAB5_FILEDOWNLOADHANDLER_H

#include <ConnectionPool.h>
#include <sys/socket.h>
#include <array>
#include <filesystem>
#include <fstream>
#include <optional>
#include "../Exceptions.h"
#include "../typenames.h"
#include "AbstractHandler.h"

namespace Server {
class FileDownloadHandler : public AbstractHandler {
   public:
    FileDownloadHandler(socket_t _client, Database::ConnectionPool& _pool);

    bool operator()() override;
    std::string get_response() override;

    class FileNotFoundException : public NotFoundException {
       public:
        explicit FileNotFoundException(const std::string& message) : NotFoundException(message) {}
    };

    class TokenNotFoundException : public NotFoundException {
       public:
        explicit TokenNotFoundException(const std::string& message) : NotFoundException(message) {}
    };

   private:
    enum class State { TOKEN, FILE, FINISHED } state = State::TOKEN;

    Database::ConnectionPool& pool;
    socket_t client;
    std::vector<char> token_buffer;
    std::string token;
    size_t bytes_sent = 0;
    std::vector<char> write_buffer;    // TODO вынести в конфиг размер буфера

    static constexpr const char* token_query = "SELECT output_path FROM requests WHERE token = $1";
    std::filesystem::path filepath = std::filesystem::path();
    std::size_t file_size;
    std::function<bool()> writer = nullptr;
    std::function<bool()> token_reader = nullptr;
    std::ifstream source_file;
    std::string response;

    void set_path();
};
}    // namespace Server

#endif    //LAB5_FILEDOWNLOADHANDLER_H
