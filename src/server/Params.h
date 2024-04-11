#ifndef LAB5_PARAMS_H
#define LAB5_PARAMS_H

#include <functional>
#include "typenames.h"

namespace Server {

/// \brief Структура, хранящая параметры сервера.
/// \property port Порт, на котором будет запущен сервер.
/// \property logger Функция для логирования сообщений.
/// \property max_connections_in_queue Максимальное количество соединений в очереди.
/// \property working_threads Максимальное количество одновременно обрабатываемых соединений.
struct Params {
    Params(unsigned int port, logger_t logger, unsigned int max_connections_in_queue,
           unsigned int working_threads)
        : port(port),
          logger(std::move(logger)),
          max_connections_in_queue(max_connections_in_queue),
          working_threads(working_threads) {}

    Params(const Params &other) = default;

    Params &operator=(const Params &other) = default;

    Params(Params &&other) = default;

    Params &operator=(Params &&other) = default;

    unsigned int port;
    logger_t logger;
    unsigned int max_connections_in_queue;
    unsigned int working_threads;
};
}    // namespace Server

#endif    //LAB5_PARAMS_H
