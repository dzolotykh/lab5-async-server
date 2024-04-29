cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

project(async-server-example)

include_directories(include)

add_subdirectory(graphgen)
include_directories(graphgen)

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
        include/files/FileManager.h
        src/files/FileManager.cpp
        include/server/handlers/UploadHandler.h
        src/server/handlers/UploadHandler.cpp
        include/server/handlers/DownloadHandler.h
        src/server/handlers/DownloadHandler.cpp
        include/server/handlers/GenerationHandler.h
        include/multithreading/ThreadPool.h
        src/multithreading/ThreadPool.cpp
        src/server/handlers/GenerationHandler.cpp
        src/server/handlers/GenerationHandler.cpp
        include/exceptions/GenerationExceptions.h
        include/server/handlers/AmogusHandler.h
        src/server/handlers/AmogusHandler.cpp
)
target_link_libraries(async-server-example graphgen)

