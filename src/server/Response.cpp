#include <server/Response.h>
namespace Server {
    Response Server::create_response(const std::string &message) {
        return Response{message};
    }

    Response Server::not_found_response() {
        return create_response("not found");
    }

    Response ok_response() {
        return create_response("ok");
    }

    Response error_response() {
        return create_response("internal error");
    }

    Response bad_request_response() {
        return create_response("bad request");
    }

    Response empty_response() {
        return create_response("");
    }

}