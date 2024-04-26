cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(async-server-example)

include_directories(include)

add_executable(async-server-example
        src/socket/Socket.cpp
        src/socket/ListenerSocket.cpp
        src/socket/ClientSocket.cpp
        examples/main.cpp
        include/socket/ClientSocket.h
        include/exceptions/SocketExceptions.h
        include/socket/ListenerSocket.h
        include/server/handlers/IHandler.h
        include/server/Response.h
        include/server/Server.h
        src/server/Server.cpp
        include/server/handlers/EchoHandler.h
        src/server/handlers/EchoHandler.cpp
        src/server/Response.cpp
        include/logging/Logger.h
        src/logging/Logger.cpp
)
target_link_libraries(async-server-example)

