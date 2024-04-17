# Лабораторная работа №5. Многопоточный сервер

В качестве реализации многопоточного сервера была разработана библиотека, позволяющая в удобном формате запускать сервер и обрабатывать различные эндпоинты.

## Как взаимодействовать с библиотекой?
Библиотека предоставляет несколько классов.
1. `Server::Server` – Как нетрудно догадаться по названию, этот класс реализует логику работы сервера. Для того чтобы запустить сервер, необходимо создать объект этого класса и запустить метод `start()`. 
2. `Server::Params` – объект которого необходимо передать в качестве параметров для конструктора сервера. Он содержит в себе информацию о количестве потоков, которые будет использовать сервер, порте, на котором он будет работать, максимальном количестве подключений, ждущих в очереди на обработку и функтор-логгер.
3. `Server::AbstractHandler` – базовый класс для всех кастомных обработчиков. Он предоставляет интерфейс взаимодействия, а также полезные для работы методы `write_bytes_nonblock` и `read_bytes_nonblock`.

### Считывание данных
Так как стандартный метод `recv` не обязан считывать все доступные и вмещающиеся в буффер байты, а также из-за того, что при работе с буферами часто приходится писать много шаблонного кода, класс `Server::AbstractHandler` предоставляет своим наследникам метод `read_bytes_nonblock`, имеющий две сигнатуры.
1. `read_bytes_nonblock(int socket, size_t need_read, char* dst, size_t buff_size, const std::function<void(size_t)> &on_read)`: в качестве первого параметра необходимо передать файловый дескриптор сокета, в качестве второго параметра – количество байт, которое необходимо считать с сокета. Третий и четвертый параметр отвечают за указатель на буфер и его размер соответственно. 
Пятым параметром необходимо передать функцию, которая будет вызываться каждый раз, когда было прочитано определенное количество байт. Прочитанные байты помещаются в буфер, а количество записанных байтов передается в функцию.
2. `read_bytes_nonblock(int socket, char* dst, size_t buff_size, const std::function<void(size_t)> &on_read)`: в этом случае все то же самое, за исключением того, что функция будет читать байты до того момента, пока клиент не отключится.

Метод `read_bytes_nonblock` возвращает `std::function<bool()>`. При каждом вызове возвращенной функции будет происходить попытка чтения данных из сокета. В первом случае, `false` будет возвращено тогда, когда будут считаны все `need_read` байтов. Во втором случае, `false` вернется тогда, когда пользователь отключится.
   
**Важно: в случае если пользователь отключился во время считывания определенного числа байтов, будет выброшено исключение.**

### Отправка данных
Аналогично считыванию данных, класс `Server::AbstractHandler` предоставляет метод `write_bytes_nonblock`. Он используется для записи в сокет, при это также поддерживает возможность постепенной записи данных из буфера.

В качестве параметров необходимо передать файловый дескриптор сокета, на котором ведется работа с клиентом, количество байтов для записи, а также функтор типа `std::function<std::pair<const char*, size_t>()>`. Этот функтор нужен для получения данных для записи: он должен возвращать указатель на буфер и количество байтов, которые в него записаны. Далее, когда байты в буфере закончатся, этот функтор будет снова вызван, и так пока все байты не будут записаны.

Как и в прошлом пункте, метод возвращает функтор, при вызове которого будет происходить запись какого-то (заранее неизвестного) числа байт в сокет. 

### Создание кастомных обработчиков
Для того чтобы создать кастомный обработчик, необходимо унаследоваться от класса `Server::AbstractHandler`. Он обязывает классы-наследники переопределить два метода: `AbstractHandler::operator()() -> bool` и `AbstractHandler::get_response() -> std::string`. Подробнее о методах:

`AbstractHandler::operator()` вызывается каждый раз, когда есть что считывать. Для считывания данных удобно воспользоваться описанными выше функциями `read_bytes_nonblock` и `write_bytes_nonblock`. Если работа с клиентом завершена, необходимо вернуть `false`. Если же работа не завершена, то необходимо вернуть `true`. Если работа с клиентом длительная, то имеет смысл разбивать ее на несколько этапов, чтобы дать время потоку поработать и с другими клиентами.

`AbstractHandler::get_response()` должен возвращать строку-ответ, которая будет отправлена клиенту после того, как оператор `()` вернет `false`. Перед отправкой, в начало ответа будет добавлено 4 байта: длина ответа. Если строка пустая, то клиенту ничего не отправится. Если же во время любого вызова оператора `()` было выброшено исключение, то в качестве ответа вернется строка `ERROR|Internal error`. 

Как было видно раньше, методы для чтения и записи принимают первым аргументом файловый дескриптор сокета. Но откуда обработчик его получит? Все очень просто: необходимо написать конструктор, который в качестве первого аргумента принимает как раз этот файловый дескриптор. Тогда, во время того, как понадобится обработать клиента, в конструктор обработчика будет передан сокет, на котором ведется вся работа с клиентом.

Чтобы зарегистрировать свой кастомный обработчик в сервере, необходимо у объекта класса `Server::Server` вызвать шаблонный метод `add_endpoint<Endpoint>(char, ...)`. В качестве шаблонного параметра необходимо передать класс обработчика, в качестве первого параметра типа char – байт, который отвечает за эндпоинт (в текущей реализации эндпоинт определяется первым байтом в запросе). Остальные параметры опциональны, и будут переданы в том же порядке в конструктор объекта обработчика.

### Пример использования
Ниже приведен пример реализации эхо-сервера с помощью библиотеки.
```c++
#include <Server.h>
#include <functional>
#include <array>
#include <iostream>

class EchoHandler: public Server::AbstractHandler {
public:
    EchoHandler(Server::socket_t _client): client(_client) {
        reader = Server::AbstractHandler::read_bytes_nonblock(client, buff.data(), buff.size(), [this](size_t read){
            written_in_buffer = read;
        });
        writer = Server::AbstractHandler::write_bytes_nonblock(client, 0, [this](){
            return std::make_pair(buff.data(), 0);
        });
    }

    std::string get_response() final {return "";}

    bool operator()() final {
        if (!writer()) {
            bool is_disconnected = !reader();
            if (is_disconnected) {
                return false;
            }
            writer = Server::AbstractHandler::write_bytes_nonblock(client, written_in_buffer, [this](){
                return std::make_pair(buff.data(), written_in_buffer);
            });
        }
        writer();
        return true;
    }
private:
    Server::socket_t client;
    std::function<bool()> reader;
    std::function<bool()> writer;
    std::array<char, 1024> buff;
    size_t written_in_buffer;
};

int main() {
    const int port = 8082;
    const int max_connections_in_queue = 1000;
    const int working_threads = 6;
    auto logger = [](const std::string& s) {
        std::cout << s << std::endl;
    };
    Server::Params params(port, logger, max_connections_in_queue, working_threads);
    Server::Server serv(params);
    serv.add_endpoint<EchoHandler>('e');
    serv.start();
}
```
