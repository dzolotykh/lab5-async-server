#ifndef LAB5_ABSTRACTHANDLER_H
#define LAB5_ABSTRACTHANDLER_H

#include <string>

namespace Server {
class AbstractHandler {
   public:
    enum class Result {
        OK,
        ERROR,
        PROCESSING,
    };
    virtual bool operator()() = 0;
    virtual std::string get_response() = 0;
    virtual Result get_result() = 0;

    virtual ~AbstractHandler() = default;
};
}    // namespace Server

#endif    //LAB5_ABSTRACTHANDLER_H
