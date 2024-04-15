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
    std::vector<char> buff;
    Database::ConnectionPool& pool;
    std::string response;
    std::function<bool()> token_reader;

    std::string save_request_to_db();

    constexpr static const char* token_query =
        "SELECT requests.id, token, input_path, name FROM requests INNER JOIN status ON status.id "
        "= status_id WHERE token = $1;";
};
}    // namespace Server

#endif    //LAB5_RESULTREQUESTHANDLER_H
