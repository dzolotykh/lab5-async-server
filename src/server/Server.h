#ifndef LAB5_SERVER_H
#define LAB5_SERVER_H

#include <Pool.h>
#include <atomic>
#include <csignal>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <poll.h>

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

    Server(const ServerParams& params);
    ~Server();
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void start();
    void hard_shutdown();
    void gentle_shutdown();

  private:
    std::string start_message() const;
    void        prepare_listener_socket();
    void        use_logger(const std::string& message);
    void        select_endpoint(socket_t connection_socket);
    static void set_nonblock(socket_t socket);
    void process_listener(pollfd listener);

    std::unordered_map<std::string, std::function<void()>> endpoints;
    ServerParams                                           params;
    socket_t                                               listener_socket;
    std::atomic<bool>                                      is_running = false;
    std::atomic<bool>                                      hard_shutdown_flag = false;
    ThreadPool::Pool                                       pool;
    std::unique_ptr<char*>                                 buff;
    std::mutex                                             logger_mtx;
    std::mutex                                             new_connection_mtx;
    std::condition_variable                                new_connection_cv;
    std::vector<Connection>                                conn_sockets;
    std::vector<pollfd>                                    conn_sockets_pollfd;
};

#endif  //LAB5_SERVER_H
