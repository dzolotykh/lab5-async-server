#ifndef ASYNC_SERVER_EXAMPLE_RESPONSE_H
#define ASYNC_SERVER_EXAMPLE_RESPONSE_H

#include <string>

namespace Server {
    struct Response {
        std::string message;
    };

    Response create_response(const std::string& message);

    Response not_found_response();

    Response ok_response();

    Response error_response();

    Response bad_request_response();

    Response empty_response();
}

#endif //ASYNC_SERVER_EXAMPLE_RESPONSE_H
