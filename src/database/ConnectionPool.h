#ifndef LAB5_CONNECTIONPOOL_H
#define LAB5_CONNECTIONPOOL_H

#include <mutex>
#include <pqxx/pqxx>
#include <queue>

namespace Database {
class Connection;

/// \brief Класс, реализующий потокобезопасный пул соединений с базой данных.
/// \details Пул соединений реализован в виде очереди, в которой хранятся указатели на объекты соединений.
/// При запросе соединения из пула, извлекается указатель на соединение из начала очереди. Если очередь пуста,
/// поток ожидает появления нового соединения в очереди.
class ConnectionPool final {
    friend class Connection;

   public:
    /// \brief Конструктор пула соединений. Принимает на вход количество соединений и строку для подключения к БД.
    /// \param _num_connections Количество соединений в пуле.
    /// \param _connection_string Строка для подключения к БД.
    /// \throws pqxx::broken_connection в случае ошибки подключения к БД.
    ConnectionPool(size_t _num_connections, std::string _connection_string);
    ~ConnectionPool();

    /// \brief Получить соединение из пула.
    /// \details Если в пуле нет доступных соединений, поток ожидает появления нового соединения в пуле.
    /// \return Указатель на объект соединения с БД.
    Connection get_connection();

    /// \brief Вернуть соединение в пул.
    /// \param connection Указатель на объект соединения с БД.
    /// \details После возврата соединения в пул, оно становится доступным для других потоков.

    ConnectionPool(const ConnectionPool& other) = delete;
    ConnectionPool& operator=(const ConnectionPool& other) = delete;
    ConnectionPool(ConnectionPool&& other) noexcept = delete;
    ConnectionPool& operator=(ConnectionPool&& other) noexcept = delete;

   private:
    size_t num_connections;
    std::string connection_string;
    std::queue<pqxx::connection> connections;
    std::mutex connections_mtx;
    std::condition_variable connections_cv;

    void return_connection(pqxx::connection conn);
};

class Connection final {
   public:
    Connection(pqxx::connection _connection, ConnectionPool& _parent_pool);
    ~Connection();
    Connection& operator=(Connection&& other) noexcept = delete;
    Connection(Connection&& other) noexcept = delete;
    Connection(const Connection& other) = delete;
    Connection& operator=(const Connection& other) = delete;

    pqxx::connection& get_connection() noexcept;

   private:
    pqxx::connection owned_connection;
    ConnectionPool& parent_pool;
};

}    // namespace Database

#endif    //LAB5_CONNECTIONPOOL_H
