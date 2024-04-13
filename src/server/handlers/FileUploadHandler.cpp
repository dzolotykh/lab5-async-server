#include "FileUploadHandler.h"

void Server::FileUploadHandler::generate_filename() {
    do {
        filepath = std::filesystem::current_path() / (StringUtils::random_string(32) + ".txt");
    } while (std::filesystem::exists(filepath));
}

#include <iostream>
#include <utility>

Server::FileUploadHandler::FileUploadHandler(socket_t client, Database::ConnectionPool& _pool,
                                             std::filesystem::path _save_path)
    : client(client), pool(_pool), file_size(0), save_path(std::move(_save_path)), token(StringUtils::random_string(32)) {
    std::filesystem::current_path(save_path);
    generate_filename();
}

bool Server::FileUploadHandler::read_file_size() {
    if (state != State::FILE_SIZE) {
        return false;
    }

    // так как ввод достаточно маленький, мы можем считать его за 1 раз
    read_bytes(client, header_size, buffer.data());

    file_size = *reinterpret_cast<int32_t*>(buffer.data());

    if (file_size <= 0) {
        state = State::ERROR;
        throw BadInputException("Размер файла не может быть отрицательным или равным нулю.");
    }

    state = State::FILE_CONTENT;
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
    std::cout << file_size << std::endl;
    if (state != State::FILE_CONTENT) {
        return false;
    }
    file.open(filepath, std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        state = State::ERROR;
        throw std::runtime_error("Не удалось открыть файл для записи. Имя файла: " +
                                 filepath.string());
    }

    if (!reader.has_value()) {
        char* buffer_ptr = buffer.data();
        reader = read_bytes_nonblock(
            client, file_size, buffer.data(), buffer.size(),
            [buffer_ptr, this](size_t need_write) { file.write(buffer_ptr, need_write); });
    }

    auto need_continue = reader.value()();
    file.close();
    if (!need_continue) {
        save_file_to_db(token);
        state = State::FINISHED;
    }
    return need_continue;
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