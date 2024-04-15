#ifndef LAB5_REQUESTGENERATIONHANDLER_H
#define LAB5_REQUESTGENERATIONHANDLER_H

#include <AbstractHandler.h>
#include <ConnectionPool.h>
#include "../Exceptions.h"
#include "../typenames.h"

namespace Server {
class RequestGenerationHandler : public AbstractHandler {
   public:
    RequestGenerationHandler(socket_t _client, Database::ConnectionPool& _pool);
    bool operator()() override;
    std::string get_response() override;

   private:
    std::vector<char> buff;
    std::function<bool()> token_reader = nullptr;
    socket_t client;
    std::string response;
    Database::ConnectionPool& pool;
    constexpr static const char* input_data_token_query = "select * from files where token=$1";
    constexpr static const char* input_data_query =
        "insert into requests(token, input_path, status_id) values($1, $2, 0)";
    constexpr static const char* check_token_query = "select * from files where token=$1";
};
}    // namespace Server

#endif    //LAB5_REQUESTGENERATIONHANDLER_H
