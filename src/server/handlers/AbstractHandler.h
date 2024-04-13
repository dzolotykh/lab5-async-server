#ifndef LAB5_ABSTRACTHANDLER_H
#define LAB5_ABSTRACTHANDLER_H

#include <functional>
#include <string>
#include "../Exceptions.h"
#include <sys/socket.h>

namespace Server {
class AbstractHandler {
   public:
    enum class Result {
        OK,
        ERROR,
        PROCESSING,
    };
    virtual bool operator()() = 0;
    virtual std::string get_response() = 0;
    virtual Result get_result() = 0;

    virtual ~AbstractHandler() = default;

   protected:
    /// \brief Чтение определенного количества байт из сокета.
    /// Функция принудительно читает все байты, пока сокет доступен.
    /// \param bytes Количество байт, которое нужно прочитать.
    /// \param dst Указатель на буфер, в который нужно записать прочитанные байты.
    /// \throws SocketException, если произошла ошибка при чтении данных из сокета.
    /// \throws BadInputException, если клиент отключился.

    static void read_bytes(int client_socket, size_t bytes, char* dst);

    /// \brief Запись определенного количества байт в сокет.
    /// Функция принудительно пишет все байты, пока сокет доступен.
    /// \param bytes Количество байт, которое нужно записать.
    /// \param src Указатель на буфер, из которого нужно записать байты.
    /// \throws SocketException, если произошла ошибка при чтении данных из сокета.
    /// \throws BadInputException, если клиент отключился.

    static void write_bytes(int client_socket, size_t bytes, const char* src);

    // Функция read_bytes_nonblock умеет постепенно считывать определенное количество байт из сокета.
    // На вход необходимо подать сокет, количество байт, которое необходимо считать, размер буфера
    // (то есть то, сколько байт за одну итерацию будет считано максимально), указатель на буфер, а также
    // функцию, которая будет вызываться при считывании байтов в буфер.

    static std::function<bool()> read_bytes_nonblock(int client_socket, size_t need_read, char* dst,
                                                     size_t buff_size,
                                                     const std::function<void(size_t)>& on_read);

    // Функция write_bytes_nonblock умеет постепенно записывать определенное количество байт в сокет.
    // На вход необходимо подать сокет, куда будет производиться запись, количество байт, которое необходимо записать суммарно,
    // и функцию, которая будет предоставлять байты для записи в сокет.
    static std::function<bool()> write_bytes_nonblock(int client_socket, size_t bytes_write,
                                                      std::function<std::pair<const char *, size_t>()> get_bytes);
};
} // namespace Server

#endif    //LAB5_ABSTRACTHANDLER_H
