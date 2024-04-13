#ifndef LAB5_REQUESTGENERATIONHANDLER_H
#define LAB5_REQUESTGENERATIONHANDLER_H

#include <AbstractHandler.h>
#include <ConnectionPool.h>
#include "../typenames.h"
#include "../Exceptions.h"

namespace Server {
    class RequestGenerationHandler: public AbstractHandler {
    public:
        RequestGenerationHandler(socket_t _client, Database::ConnectionPool& _pool);
        bool operator()() override;
        std::string get_response() override;
        Result get_result() override;
    private:
        socket_t client;
        std::string response;
        std::array<char, 32> buff;
        Database::ConnectionPool& pool;
        Result result = Result::PROCESSING;
        constexpr static const char* input_data_token_query = "select * from files where token=$1";
        constexpr static const char* input_data_query = "insert into requests(token, input_path, status_id) values($1, $2, 0)";
    };
}


#endif //LAB5_REQUESTGENERATIONHANDLER_H
