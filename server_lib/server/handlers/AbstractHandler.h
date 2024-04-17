#ifndef LAB5_ABSTRACTHANDLER_H
#define LAB5_ABSTRACTHANDLER_H

#include <sys/socket.h>
#include <functional>
#include <string>
#include "../Exceptions.h"

namespace Server {
class AbstractHandler {
   public:
    virtual bool operator()() = 0;

    virtual std::string get_response() = 0;

    virtual ~AbstractHandler() = default;

   protected:
    // Функция read_bytes_nonblock умеет постепенно считывать определенное количество байт из сокета.
    // На вход необходимо подать сокет, количество байт, которое необходимо считать, размер буфера
    // (то есть то, сколько байт за одну итерацию будет считано максимально), указатель на буфер, а также
    // функцию, которая будет вызываться при считывании байтов в буфер.

    static std::function<bool()> read_bytes_nonblock(int client_socket, size_t need_read, char *dst,
                                                     size_t buff_size,
                                                     const std::function<void(size_t)> &on_read);

    // перегрузка на случай, если необходимо считать неограниченное количество байтов.
    // Вернет false, когда пользователь отключится
    static std::function<bool()> read_bytes_nonblock(int client_socket, char *dst,
                                                     size_t buff_size,
                                                     const std::function<void(size_t)> &on_read);

    // Функция write_bytes_nonblock умеет постепенно записывать определенное количество байт в сокет.
    // На вход необходимо подать сокет, куда будет производиться запись, количество байт, которое необходимо записать суммарно,
    // и функцию, которая будет предоставлять байты для записи в сокет.
    static std::function<bool()> write_bytes_nonblock(
        int client_socket, size_t need_write,
        const std::function<std::pair<const char *, size_t>()> &get_bytes);

   private:
    static std::function<bool()> construct_writer(
        int client_socket, size_t need_write,
        const std::function<std::pair<const char *, size_t>()> &get_bytes);
};
}    // namespace Server

#endif    //LAB5_ABSTRACTHANDLER_H