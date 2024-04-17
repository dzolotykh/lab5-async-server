cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(async-server-example)

add_subdirectory(server_lib)

add_executable(async-server-example main.cpp
        EchoHandler.cpp
        LongRequestHandler.cpp)

target_link_libraries(async-server-example async-server)

