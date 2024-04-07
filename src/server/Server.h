#ifndef LAB5_SERVER_H
#define LAB5_SERVER_H

#include <iostream> // for debugging

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

struct ServerParams
{
    using logger_t = std::function<void(const std::string&)>;
    ServerParams(unsigned int port,
                 logger_t     logger,
                 unsigned int max_connections_in_queue,
                 unsigned int max_process,
                 unsigned int buff_size)
        : port(port), logger(std::move(logger)), max_connections_in_queue(max_connections_in_queue),
          max_process(max_process), buff_size(buff_size)
    {
    }

    ServerParams(const ServerParams& other) = default;
    ServerParams& operator=(const ServerParams& other) = default;
    ServerParams(ServerParams&& other) = default;
    ServerParams& operator=(ServerParams&& other) = default;

    unsigned int port;
    logger_t     logger;
    unsigned int max_connections_in_queue;
    unsigned int max_process;
    unsigned int buff_size;
};

class Server
{
  public:
    enum class SocketType
    {
        listener,
        client,
    };

    using logger_t = ServerParams::logger_t;
    using socket_t = int;

    struct Connection
    {
        socket_t   socket;
        SocketType type;
    };

    explicit Server(ServerParams  params);
    ~Server();
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void start();

  private:
    [[nodiscard]] std::string             start_message() const;
    void                    prepare_listener_socket();
    void                    use_logger(const std::string& message);
    static void             set_nonblock(socket_t socket);
    std::vector<Connection> process_listener(pollfd listener);
    bool                    process_client(pollfd fd, socket_t client);
    std::string             read_from_socket(socket_t socket);

    ServerParams            params;
    socket_t                listener_socket{};
    std::mutex              logger_mtx;
    std::vector<Connection> conn_sockets;
    std::vector<pollfd>     conn_sockets_pollfd;
};

#endif  //LAB5_SERVER_H
