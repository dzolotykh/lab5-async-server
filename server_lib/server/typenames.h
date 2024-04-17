#ifndef LAB5_TYPENAMES_H
#define LAB5_TYPENAMES_H

#include <functional>
#include <memory>
#include "handlers/AbstractHandler.h"

namespace Server {
using socket_t = int;
using logger_t = std::function<void(const std::string &)>;
using changer_t = std::function<void(std::unique_ptr<AbstractHandler> handler)>;
using handler_provider_t = std::function<std::unique_ptr<AbstractHandler>(socket_t)>;
}    // namespace Server

#endif    //LAB5_TYPENAMES_H
