#include "FileUploadHandler.h"

std::string Server::FileUploadHandler::random_string(size_t length) {
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result.push_back(characters[gen() % characters_size]);
    }
    return result;
}

void Server::FileUploadHandler::generate_filename() {
    do {
        filepath = std::filesystem::current_path() / (random_string(32) + ".txt");
    } while (std::filesystem::exists(filepath));
}

#include <iostream>

Server::FileUploadHandler::FileUploadHandler(socket_t client, Database::ConnectionPool& _pool,
                                             const std::filesystem::path& _save_path)
    : client(client), gen(rd()), pool(_pool), file_size(0), save_path(_save_path) {
    std::filesystem::current_path(save_path);
    generate_filename();
}

bool Server::FileUploadHandler::read_file_size() {
    if (state != State::FILE_SIZE) {
        return false;
    }

    ssize_t read = recv(client, buffer.data() + bytes_read, buffer_size - bytes_read, MSG_DONTWAIT);
    if (read == -1 && errno == EAGAIN) {
        return true;
    } else if (read == -1) {
        state = State::FINISHED;
        throw std::runtime_error("Ошибка при чтении размера файла");
    } else if (read == 0) {
        state = State::FINISHED;
        return false;    // клиент отключился
    }
    bytes_read += read;
    if (read + bytes_read < header_size) {
        return true;
    }
    file_size = *reinterpret_cast<int32_t*>(buffer.data());

    buffer = std::vector<char>(buffer.begin() + header_size, buffer.end());
    buffer.resize(buffer_size);
    state = State::FILE_CONTENT;
    bytes_read -= header_size;
    if (bytes_read > 0) {
        begin_not_written = true;
    }
    bytes_not_read = file_size - bytes_read;
    return true;
}

void Server::FileUploadHandler::save_file_to_db(const std::string& token) {
    auto conn = pool.get_connection();
    pqxx::work w(conn.get_connection());
    w.exec("INSERT INTO files (filepath, size, token) VALUES ('" + filepath.string() + "', " +
           std::to_string(file_size) + ", '" + token + "')");
    w.commit();
}

bool Server::FileUploadHandler::read_file_content() {
    if (state != State::FILE_CONTENT) {
        return false;
    }
    std::ofstream file(filepath, std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        state = State::ERROR;
        throw std::runtime_error("Не удалось открыть файл для записи. Имя файла: " +
                                 filepath.string());
    }

    if (begin_not_written) {
        file.write(buffer.data(), static_cast<std::streamsize>(bytes_read));
        begin_not_written = false;
        if (bytes_read == file_size) {
            token = random_string(32);
            save_file_to_db(absolute(filepath).string());
            state = State::FINISHED;
            return false;
        }
    }

    size_t read_in_this_iteration = 0;
    while (read_in_this_iteration < buffer_size && bytes_not_read > 0) {
        ssize_t read = recv(client, buffer.data(), buffer_size, MSG_DONTWAIT);
        if (read == -1 && errno == EAGAIN) {
            return true;
        } else if (read == -1) {
            state = State::ERROR;
            throw std::runtime_error("Ошибка при чтении содержимого файла");
        } else if (read == 0) {
            state = State::ERROR;
            return false;    // клиент отключился
        }
        bytes_read += read;
        if (read > bytes_not_read) {
            state = State::ERROR;
            throw std::runtime_error("Прочитано больше байт, чем заявлено в заголовке.");
        }
        read_in_this_iteration += read;
        bytes_not_read -= read;
        file.write(buffer.data(), read);
    }
    if (bytes_not_read == 0) {
        token = random_string(32);
        save_file_to_db(absolute(filepath).string());
        state = State::FINISHED;
        return false;
    }
    return true;
}

bool Server::FileUploadHandler::operator()() {
    if (state == State::FINISHED || state == State::ERROR) {
        return false;
    } else if (state == State::FILE_SIZE) {
        read_file_size();
    } else {
        read_file_content();
    }
    return true;
}

std::string Server::FileUploadHandler::get_response() {
    return token + "@" + absolute(filepath).string();
}

Server::AbstractHandler::Result Server::FileUploadHandler::get_result() {
    if (state == State::FINISHED) {
        return Result::OK;
    } else if (state == State::ERROR) {
        return Result::ERROR;
    } else {
        return Result::PROCESSING;
    }
}