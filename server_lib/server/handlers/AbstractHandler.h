#ifndef LAB5_ABSTRACTHANDLER_H
#define LAB5_ABSTRACTHANDLER_H

#include <sys/socket.h>
#include <functional>
#include <string>
#include "../Exceptions.h"
#include "../../socket/Socket.h"

namespace Server {
class AbstractHandler {
   public:
    virtual bool operator()() = 0;

    virtual std::string get_response() = 0;

    virtual ~AbstractHandler() = default;
};
}    // namespace Server

#endif    //LAB5_ABSTRACTHANDLER_H
