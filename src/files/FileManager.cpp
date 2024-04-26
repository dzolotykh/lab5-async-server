#include <files/FileManager.h>

Server::Files::FileManager::FileManager(std::filesystem::path _dir) : dir(std::move(_dir)) {}

std::string Server::Files::FileManager::random_string(size_t length) {
    std::uniform_int_distribution<> dist(0, CHARS.size() - 1);
    std::string result;
    result.reserve(32);
    for (size_t i = 0; i < 32; ++i) {
        result += CHARS[dist(gen)];
    }
    return result;
}

void Server::Files::FileManager::remove_file(const std::filesystem::path &file) {
    std::lock_guard lock(mutex);
    std::filesystem::remove(file);
}

std::filesystem::path Server::Files::FileManager::create_unique() {
    std::lock_guard lock(mutex);
    return dir / random_string(32);
}

std::filesystem::path Server::Files::FileManager::get_dir() const {
    return dir;
}
