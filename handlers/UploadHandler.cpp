#include "UploadHandler.h"

#include <iostream>

UploadHandler::UploadHandler(const Server::Socket &_client): client(_client) {
    reader = client.read_bytes_nonblock(sizeof(file_size), buffer.data(), buffer_size, [this, total_read = 0](size_t read) mutable {
        char* ptr = reinterpret_cast<char*>(&file_size);
        std::copy(buffer.begin(), buffer.begin() + read, ptr + total_read);
        total_read += static_cast<int>(read);
        if (total_read == sizeof(file_size)) {
            std::cout << file_size << std::endl;
            if (file_size > max_file_size) {
                response = "ERROR|File is too big. Max file size is 10MB.";
                state = STATE::FINISHED;
                return;
            }
            state = STATE::FILE_CONTENT;
            filename = generate_filename();
            file.open(output / filename, std::ios::binary);
            reader = client.read_bytes_nonblock(file_size, buffer.data(), buffer_size, [this, total_read = 0](size_t read) mutable {
                file.write(buffer.data(), static_cast<int>(read));
                total_read += static_cast<int>(read);
                if (total_read == file_size) {
                    file.close();
                    response = "OK|" + filename.string();
                    state = STATE::FINISHED;
                }
            });
        }
    });
}

bool UploadHandler::operator()() {
    if (state == STATE::FILE_SIZE) {
        return reader();
    }
    if (state == STATE::FILE_CONTENT) {
        return reader();
    }
    return false;
}

std::filesystem::path UploadHandler::generate_filename() {
    return std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

std::string UploadHandler::get_response() {
    return response;
}