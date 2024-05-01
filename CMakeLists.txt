cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

project(async-server)

include_directories(include)

add_subdirectory(graphgen)
include_directories(graphgen)

set(INCLUDES
        include/socket/ClientSocket.h
        include/socket/ListenerSocket.h

        include/exceptions/SocketExceptions.h
        include/exceptions/GenerationExceptions.h

        include/server/handlers/IHandler.h
        include/server/Response.h
        include/server/Server.h
        include/server/handlers/EchoHandler.h
        include/server/handlers/UploadHandler.h
        include/server/handlers/GenerationHandler.h
        include/server/handlers/AmogusHandler.h

        include/files/FileManager.h

        include/logging/Logger.h

        include/multithreading/ThreadPool.h
)

set(SOURCES
        src/socket/Socket.cpp
        src/socket/ListenerSocket.cpp
        src/socket/ClientSocket.cpp
        examples/main.cpp
        src/server/Server.cpp
        src/server/handlers/EchoHandler.cpp
        src/server/Response.cpp
        src/logging/Logger.cpp
        src/files/FileManager.cpp
        src/server/handlers/UploadHandler.cpp
        include/server/handlers/DownloadHandler.h
        src/server/handlers/DownloadHandler.cpp
        src/multithreading/ThreadPool.cpp
        src/server/handlers/GenerationHandler.cpp
        src/server/handlers/GenerationHandler.cpp
        src/server/handlers/AmogusHandler.cpp
)

add_executable(async-server ${INCLUDES} ${SOURCES})

target_link_libraries(async-server graphgen)

