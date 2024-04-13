#ifndef LAB5_FILEUPLOADHANDLER_H
#define LAB5_FILEUPLOADHANDLER_H

#include <ConnectionPool.h>
#include <StringUtils.h>
#include <sys/socket.h>
#include <filesystem>
#include <fstream>
#include <random>
#include "../typenames.h"
#include "AbstractHandler.h"

// TODO сделать ограничение на размер файла

// Протокол загрузки файла:
// 1 сообщение от клиента: размер файла
// 2 сообщение от клиента: содержимое файла
// 3 сообщение от сервера: результат загрузки файла

namespace Server {

/// \brief Обработчик загрузки файла на сервер.
class FileUploadHandler : public AbstractHandler {
   private:
    const int buffer_size = 1024 * 1024;
    const int header_size = 4;    // хедер состоит просто из размера файла, влезает в int

    socket_t client;
    size_t file_size = 0;
    Database::ConnectionPool& pool;
    std::filesystem::path filepath;
    std::vector<char> buffer = std::vector<char>(buffer_size, 0);
    void generate_filename();
    enum class State {
        FILE_SIZE,
        FILE_CONTENT,
        FINISHED,
        ERROR,
    } state = State::FILE_SIZE;
    size_t bytes_read = 0;
    size_t bytes_not_read = header_size;

    bool read_file_size();
    bool read_file_content();

    void save_file_to_db(const std::string& token);

    std::string token;
    std::filesystem::path save_path;
    std::ofstream file;

    std::optional<std::function<bool()>> reader;

   public:
    FileUploadHandler(socket_t client, Database::ConnectionPool& _pool,
                      std::filesystem::path _save_path);
    bool operator()() override;
    std::string get_response() override;
    Result get_result() override;
};
}    // namespace Server

#endif    //LAB5_FILEUPLOADHANDLER_H
