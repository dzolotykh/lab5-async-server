#ifndef ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H
#define ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H

#include "Server.h"
#include <fstream>
#include <filesystem>

class UploadHandler: public Server::AbstractHandler {
public:
    explicit UploadHandler(const Server::Socket& _client);
    std::string get_response() final;
    bool operator()() final;
private:
    static constexpr int header_size = 4; // размер хедера сообщения, должен содержать в себе размер файла
    static constexpr int buffer_size = 1024 * 1024; // размер буфера для чтения файла
    static constexpr int max_file_size = 1024 * 1024 * 10; // максимальный размер файла
    static constexpr const char* output = "../uploads/"; // папка, куда сохранять файлы
    std::array<char, buffer_size> buffer;

    const Server::Socket& client;
    int file_size = 0;
    std::filesystem::path filename;
    std::ofstream file;

    enum class STATE {
            FILE_SIZE,
            FILE_CONTENT,
            FINISHED
    } state = STATE::FILE_SIZE;

    Server::Socket::predicate_t reader = nullptr;
    Server::Socket::predicate_t writer = nullptr;
    std::string response;

    static std::filesystem::path generate_filename();
};


#endif //ASYNC_SERVER_EXAMPLE_UPLOADHANDLER_H
