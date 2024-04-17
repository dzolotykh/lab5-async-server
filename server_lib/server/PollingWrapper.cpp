#include "PollingWrapper.h"

namespace Server {

void PollingWrapper::add_connection(const std::vector<socket_t> &sockets) {
    for (auto socket : sockets) {
        add_connection(socket);
    }
}

void PollingWrapper::add_connection(Server::socket_t socket) {
    connections.push_back(socket);
    pollfd pollfd = {.fd = socket, .events = POLLHUP | POLLIN | POLLERR};
    pollfds.push_back(pollfd);
}

void PollingWrapper::remove_disconnected() {
    std::vector<socket_t> filtered_connections;
    std::vector<pollfd> filtered_pollfds;
    for (size_t i = 0; i < connections.size(); ++i) {
        if (connections[i] != -1) {
            filtered_connections.push_back(connections[i]);
            filtered_pollfds.push_back(pollfds[i]);
        }
    }
    connections = std::move(filtered_connections);
    pollfds = std::move(filtered_pollfds);
}

std::pair<std::vector<socket_t>, std::vector<pollfd>> PollingWrapper::get() const {
    return std::make_pair(get_connections(), get_pollfds());
}

std::vector<pollfd> PollingWrapper::get_pollfds() const {
    std::vector<pollfd> res = pollfds;
    pollfd listener_pollfd = {.fd = listener_socket, .events = POLLIN};
    res.push_back(listener_pollfd);
    return res;
}

std::vector<socket_t> PollingWrapper::get_connections() const {
    std::vector<socket_t> res = connections;
    res.push_back(listener_socket);
    return res;
}

PollingWrapper::PollingWrapper() : listener_socket(-1) {}

PollingWrapper::PollingWrapper(std::vector<socket_t> _connections, std::vector<pollfd> _pollfds)
    : connections(std::move(_connections)), pollfds(std::move(_pollfds)) {
    listener_socket = connections.back();
    connections.pop_back();
}

PollingWrapper::PollingWrapper(Server::socket_t listener_socket)
    : listener_socket(listener_socket) {}

}    // namespace Server