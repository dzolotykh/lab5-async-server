#ifndef LAB5_FILEUPLOADHANDLER_H
#define LAB5_FILEUPLOADHANDLER_H

#include <filesystem>
#include <fstream>
#include <random>
#include <sys/socket.h>
#include "AbstractHandler.h"

// TODO сделать ограничение на размер файла

// Протокол загрузки файла:
// 1 сообщение от клиента: размер файла
// 2 сообщение от клиента: содержимое файла
// 3 сообщение от сервера: результат загрузки файла

namespace Server {
    using socket_t = int;
    /* Обработчик для загрузки файла от клиента и сохранения его на диск */
    class FileUploadHandler: public AbstractHandler {
    private:
        const int buffer_size = 1024;
        const int header_size = 4; // хедер состоит просто из размера файла

        socket_t client;
        size_t file_size;
        std::filesystem::path filepath;
        std::random_device rd;
        std::mt19937 gen;
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
        bool begin_not_written = false;

        bool read_file_size();
        bool read_file_content();

    public:
        explicit FileUploadHandler(socket_t client);
        bool operator()() override;
        std::string get_response() override;
        Result get_result() override;
    };
}


#endif //LAB5_FILEUPLOADHANDLER_H
