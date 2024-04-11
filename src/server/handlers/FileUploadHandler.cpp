#include "FileUploadHandler.h"

void Server::FileUploadHandler::generate_filename() {
    do {
        filepath = std::filesystem::temp_directory_path() / std::to_string(gen());
    } while (std::filesystem::exists(filepath));
}

#include <iostream>

Server::FileUploadHandler::FileUploadHandler(Server::socket_t client) : client(client), gen(rd()) {
    generate_filename();
    std::cout << "Создан обработчик для загрузки файла. Имя файла: " << filepath << std::endl;
}

bool Server::FileUploadHandler::read_file_size() {
    std::cout << "Чтение размера файла" << std::endl;
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
    std::cout << "Прочитано " << read << " байт" << std::endl;
    std::cout << "Всего прочитано " << bytes_read << " байт" << std::endl;
    if (read + bytes_read < header_size) {
        return true;
    }
    file_size = *reinterpret_cast<int32_t*>(buffer.data());
    std::cout << "Размер файла: " << file_size << std::endl;

    buffer = std::vector<char>(buffer.begin() + header_size, buffer.end());
    buffer.resize(buffer_size);
    state = State::FILE_CONTENT;
    bytes_read -= header_size;
    if (bytes_read > 0) {
        begin_not_written = true;
    }
    bytes_not_read = file_size - bytes_read;
    // Надо что-то сделать с тем, что мы прочитали какой-то кусок буфера для самого содержимого файла
    return true;
}

bool Server::FileUploadHandler::read_file_content() {
    std::cout << "Чтение содержимого файла" << std::endl;
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
            std::cout << "Файл загружен" << std::endl;
            state = State::FINISHED;
            return false;
        }
    }

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
    std::cout << "Прочитано " << read << " байт" << std::endl;
    std::cout << "Всего прочитано " << bytes_read << " байт" << std::endl;
    if (read > bytes_not_read) {
        state = State::ERROR;
        throw std::runtime_error("Прочитано больше байт, чем заявлено в заголовке.");
    }
    bytes_not_read -= read;
    std::cout << "Осталось прочитать " << bytes_not_read << " байт" << std::endl;
    file.write(buffer.data(), read);
    if (bytes_not_read == 0) {
        std::cout << "Файл загружен" << std::endl;
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
    return filepath;
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