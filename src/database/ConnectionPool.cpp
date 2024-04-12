#include "ConnectionPool.h"

#include <utility>

Database::ConnectionPool::ConnectionPool(size_t _num_connections, std::string _connection_string)
    : num_connections(_num_connections), connection_string(std::move(_connection_string)) {
    for (size_t i = 0; i < num_connections; ++i) {
        pqxx::connection c(connection_string);
        connections.push(std::move(c));
    }
}

Database::Connection Database::ConnectionPool::get_connection() {
    std::unique_lock<std::mutex> lock(connections_mtx);
    if (connections.empty()) {
        connections_cv.wait(lock, [this] { return !connections.empty(); });
    }
    pqxx::connection connection = std::move(connections.front());
    connections.pop();
    return {std::move(connection), *this};
}

void Database::ConnectionPool::return_connection(pqxx::connection connection) {
    std::lock_guard<std::mutex> lock(connections_mtx);
    connections.push(std::move(connection));
    connections_cv.notify_one();
}

Database::ConnectionPool::ConnectionPool(Database::ConnectionPool &&other) noexcept
    : num_connections(other.num_connections),
      connection_string(std::move(other.connection_string)),
      connections(std::move(other.connections)) {
    other.num_connections = 0;
}

Database::ConnectionPool &Database::ConnectionPool::operator=(
    Database::ConnectionPool &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    num_connections = other.num_connections;
    connection_string = std::move(other.connection_string);
    connections = std::move(other.connections);
    other.num_connections = 0;
    return *this;
}

#include <iostream>

Database::Connection::~Connection() {
    parent_pool.return_connection(std::move(owned_connection));
}

Database::Connection::Connection(pqxx::connection _connection, ConnectionPool& _parent_pool)
    : owned_connection(std::move(_connection)), parent_pool(_parent_pool) {}

pqxx::connection &Database::Connection::get_connection() noexcept {
    return owned_connection;
}