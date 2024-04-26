#include "server/handlers/EchoHandler.h"

Server::Response Server::Handlers::EchoHandler::handle() {
    while (true) {
        try {
            char byte = client.read_byte();
            client.send_byte(byte);
            auto message = client.read_bytes(client.ready_to_read());
            client.send_bytes(message.data(), message.size());
        } catch (Server::Exceptions::ClientDisconnectedException& e) {
            return empty_response();
        } catch (...) {
            return error_response();
        }
    }
}
