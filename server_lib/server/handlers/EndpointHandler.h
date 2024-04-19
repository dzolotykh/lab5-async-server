#ifndef LAB5_ENDPOINTHANDLER_H
#define LAB5_ENDPOINTHANDLER_H

#include <sys/socket.h>
#include <functional>
#include <unordered_map>
#include "../typenames.h"
#include "AbstractHandler.h"
#include "../../socket/Socket.h"

namespace Server {

/// \brief Обработчик, необходимый для того, чтобы выбрать, к какому энпоинту обращается пользователь.
/// В рамках сервера эндпоинт задается первым байтом сообщения.
class EndpointHandler : public AbstractHandler {
   public:
    EndpointHandler(std::unordered_map<char, handler_provider_t>& _handlers, const Socket& _client,
                    changer_t _change_handler);

    bool operator()() override;

    std::string get_response() override;

   private:
    std::unordered_map<char, handler_provider_t>& handlers;
    const Socket& client;
    /// Функция, которая будет вызвана в случае успешного определения эндпоинта.
    changer_t change_handler;
    std::function<bool()> reader = nullptr;
    char endpoint;
    std::string response;
};
}    // namespace Server

#endif    //LAB5_ENDPOINTHANDLER_H
