#ifndef LAB5_POLLINGWRAPPER_H
#define LAB5_POLLINGWRAPPER_H

#include <poll.h>
#include <vector>

namespace Server {
/*
 * Класс-обертка для удобной работы с поллингом
 * Умеет:
 * добавлять сокеты в массив для поллинга
 * удалять уже ненужны сокеты из массива (они должны быть помечены как -1)
 * возвращать массив сокетов для поллинга. Последний элемент массива это сокет-слушатель
 * отдельно хранится главный сокет-слушатель, который не должен быть удален
*/
using socket_t = int;

class PollingWrapper {
   private:
    std::vector<pollfd> pollfds;
    std::vector<socket_t> connections;
    socket_t listener_socket;

   public:
    explicit PollingWrapper(socket_t listener_socket);

    PollingWrapper(std::vector<socket_t> _connections, std::vector<pollfd> _pollfds);

    /* В качестве дескриптора сокета в конструкторе по умолчанию устанавливается -1 */
    PollingWrapper();

    [[nodiscard]] std::vector<socket_t> get_connections() const;

    [[nodiscard]] std::vector<pollfd> get_pollfds() const;

    /* Возвращает пару: массив сокетов, которые необходимо прослушивать и массив pollfd */
    [[nodiscard]] std::pair<std::vector<socket_t>, std::vector<pollfd>> get() const;

    void remove_disconnected();

    void add_connection(socket_t socket);

    void add_connection(const std::vector<socket_t> &sockets);

    [[nodiscard]] size_t size() const { return connections.size(); }
};
}    // namespace Server

#endif    //LAB5_POLLINGWRAPPER_H
