#ifndef ASYNC_SERVER_EXAMPLE_IHANDLER_H
#define ASYNC_SERVER_EXAMPLE_IHANDLER_H

#include <server/Response.h>
namespace Server::Handlers {
    class IHandler {
    public:
        virtual Response handle() = 0;
        virtual ~IHandler() = default;
    };
}

#endif //ASYNC_SERVER_EXAMPLE_IHANDLER_H
