#ifndef LAB5_RESULTREQUESTHANDLER_H
#define LAB5_RESULTREQUESTHANDLER_H

#include <ConnectionPool.h>
#include <sys/socket.h>
#include <pqxx/pqxx>
#include "../typenames.h"
#include "AbstractHandler.h"

namespace Server {
class ResultRequestHandler : public AbstractHandler {
   public:
    ResultRequestHandler(socket_t client, Database::ConnectionPool& _pool);
    bool operator()() override;
    std::string get_response() override;

   private:
    socket_t client;
    std::array<char, 32> buff;
    size_t bytes_read = 0;
    Database::ConnectionPool& pool;
    std::string generation_token;
    std::string response;

    bool read_token();
    bool save_request_to_db();
    constexpr static const char* token_query =
        "SELECT requests.id, token, input_path, name FROM requests INNER JOIN status ON status.id "
        "= status_id WHERE token = $1;";

    enum class State { ERROR, READING_TOKEN, PROCESSING, FINISHED } state = State::READING_TOKEN;
};
}    // namespace Server

#endif    //LAB5_RESULTREQUESTHANDLER_H
