#include "ConnectionPool.h"

Database::ConnectionPool::ConnectionPool(size_t _num_connections, std::string _connection_string)
    : num_connections(_num_connections), connection_string(std::move(_connection_string)) {
    for (size_t i = 0; i < num_connections; ++i) {
        connections.push(std::make_unique<pqxx::connection>(connection_string));
    }
}

std::unique_ptr<pqxx::connection> Database::ConnectionPool::get_connection() {
    std::unique_lock<std::mutex> lock(connections_mtx);
    if (connections.empty()) {
        connections_cv.wait(lock, [this] { return !connections.empty(); });
    }
    auto connection = std::move(connections.front());
    connections.pop();
    return connection;
}

void Database::ConnectionPool::return_connection(std::unique_ptr<pqxx::connection>& connection) {
    std::lock_guard<std::mutex> lock(connections_mtx);
    connections.push(std::move(connection));
    connections_cv.notify_one();
}
