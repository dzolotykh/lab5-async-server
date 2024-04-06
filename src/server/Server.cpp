#include "Server.h"
#include "log.h"
#include <sys/poll.h>

void Server::use_logger(const std::string& message)
{
    std::lock_guard<std::mutex> lock(logger_mtx);
    params.logger(message);
}

std::string Server::start_message() const
{
    std::stringstream hello;
    hello << "\n✅ Сервер запущен. Параметры: \n";
    hello << "⚡️ Порт: " << params.port << "\n";
    hello << "⚡️ Максимальное количество подключений в очереди: " << params.max_connections_in_queue
          << "\n";
    hello << "⚡️ Максимальное количество одновременно работающих процессов: " << params.max_process
          << "\n";
    return hello.str();
}

Server::Server(const ServerParams& _params)
    : params(_params), pool(_params.max_process),
      buff(std::make_unique<char*>(new char[_params.buff_size]))
{}

void Server::set_nonblock(Server::socket_t socket)
{

    const int result = ioctl(socket, FIONBIO, 1);
    if (result < 0)
    {

        throw std::runtime_error(
            "Ошибка при установке неблокирующего режима для сокета. Код ошибки: " +
            std::to_string(errno) );
    }
}

void Server::prepare_listener_socket()
{
    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(params.port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    listener_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (listener_socket < 0)
    {
        throw std::runtime_error("Ошибка при создании сокета. Код ошибки: " +
                                 std::to_string(errno));
    }


    int bind_status = bind(listener_socket, (sockaddr*)&socket_address, sizeof(socket_address));

    if (bind_status < 0)
    {
        throw std::runtime_error("Произошла ошибка при связывании сокета с адресом. Код ошибки: " +
                                 std::to_string(errno));
    }

    set_nonblock(listener_socket);


    int listen_status = listen(listener_socket, params.max_connections_in_queue);

    if (listen_status < 0)
    {
        throw std::runtime_error("Произошла ошибка при прослушивании сокета. Код ошибки: " +
                                 std::to_string(errno));
    }
}

void Server::select_endpoint(int connection_socket)
{
    use_logger("Подключился.... ");
    set_nonblock(connection_socket);
    while (true)
    {
        ssize_t bytes_read = recv(connection_socket, buff.get(), params.buff_size, 0);
        std::cout << bytes_read << std::endl;
        if (bytes_read <= 0)
            break;
        send(connection_socket, buff.get(), bytes_read, 0);
    }
    use_logger("Отключился");
    shutdown(connection_socket, SHUT_WR);
    close(connection_socket);
}

void Server::gentle_shutdown()
{
    is_running = false;
    new_connection_cv.notify_one();
}

void Server::process_listener(pollfd listener) {
    while (true) {
        sockaddr_in peer{};
        socklen_t peer_size = sizeof(peer);
        socket_t channel = accept(listener_socket, (sockaddr*)&peer, &peer_size);
        if (channel < 0) {
            if (errno == EWOULDBLOCK) {
                break;
            } else {
                throw std::runtime_error("Ошибка при подключении к сокету. Код ошибки: " + std::to_string(errno));
            }
        }
        use_logger("Подключился новый клиент");
    }
}

void Server::start()
{
    is_running = true;

    prepare_listener_socket();
    // set_nonblock(listener_socket);
    conn_sockets.push_back({listener_socket, SocketType::listener});
    conn_sockets_pollfd.push_back({
        .fd = listener_socket,
        .events = POLLIN,
    });
    use_logger(start_message());
    
    while (is_running) {
        poll(conn_sockets_pollfd.data(), conn_sockets_pollfd.size(), -1);
        process_listener(conn_sockets_pollfd.back());
    }

    use_logger(WAIT_FINISH_MSG);
    pool.wait_all();
    use_logger(SERVER_STOPPED_MSG);
    shutdown(listener_socket, SHUT_RDWR);
}

Server::~Server()
{
    if (is_running)
    {
        shutdown(listener_socket, SHUT_RDWR);
    }
    pool.detach_all();
}