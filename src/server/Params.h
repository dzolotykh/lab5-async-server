#ifndef LAB5_PARAMS_H
#define LAB5_PARAMS_H

#include <functional>

namespace Server {
using logger_t = std::function<void(const std::string &)>;

struct Params {
    Params(unsigned int port, logger_t logger, unsigned int max_connections_in_queue,
           unsigned int max_process, unsigned int buff_size)
        : port(port),
          logger(std::move(logger)),
          max_connections_in_queue(max_connections_in_queue),
          max_process(max_process),
          buff_size(buff_size) {}

    Params(const Params &other) = default;

    Params &operator=(const Params &other) = default;

    Params(Params &&other) = default;

    Params &operator=(Params &&other) = default;

    unsigned int port;
    logger_t logger;
    unsigned int max_connections_in_queue;
    unsigned int max_process;
    unsigned int buff_size;
};
}    // namespace Server

#endif    //LAB5_PARAMS_H
