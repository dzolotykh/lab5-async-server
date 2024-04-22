cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(async-server-example)

add_subdirectory(server_lib)
include_directories(handlers)

add_executable(async-server-example main.cpp
        handlers/EchoHandler.cpp
        handlers/LongRequestHandler.cpp
        handlers/UploadHandler.cpp
)

target_link_libraries(async-server-example async-server)

