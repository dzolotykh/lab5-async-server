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

   private:
    enum class State {
        READING_TOKEN,
        SENDING_FILE,
        FINISHED,
        ERROR,
    } state = State::READING_TOKEN;

    Database::ConnectionPool& pool;
    socket_t client;
    std::array<char, 32> token;
    size_t bytes_sent = 0;
    std::array<char, 1024 * 1024 * 5> write_buffer;    // TODO вынести в конфиг

    static constexpr const char* token_query = "SELECT * FROM files WHERE token = $1";
    std::optional<std::filesystem::path> filepath = std::nullopt;
    std::size_t file_size;
    std::optional<std::function<bool()>> writer = std::nullopt;
    std::ifstream source_file;

    void read_token();
    void send_file();
    void set_path(const std::string& token);
};
}    // namespace Server

#endif    //LAB5_FILEDOWNLOADHANDLER_H
