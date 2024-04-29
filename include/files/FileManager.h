#ifndef ASYNC_SERVER_EXAMPLE_FILEMANAGER_H
#define ASYNC_SERVER_EXAMPLE_FILEMANAGER_H

#include <filesystem>
#include <fstream>
#include <mutex>
#include <random>

namespace Server::Files {
// класс, отвечающий за потокобезопасную работу с определенной директорией
class FileManager {
   public:
    explicit FileManager(std::filesystem::path _dir);
    ;
    [[nodiscard]] std::filesystem::path create_unique();
    void remove_file(const std::filesystem::path& file);
    [[nodiscard]] std::filesystem::path get_dir() const;
    [[nodiscard]] bool exists(const std::filesystem::path& file);
    [[nodiscard]] std::ifstream open(const std::filesystem::path& file);

   private:
    [[nodiscard]] std::string random_string(size_t length);

    std::mutex mutex;
    const std::filesystem::path dir;
    std::mt19937 gen{std::random_device{}()};
    const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
};
}    // namespace Server::Files

#endif    //ASYNC_SERVER_EXAMPLE_FILEMANAGER_H
