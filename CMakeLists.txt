cmake_minimum_required(VERSION 3.20)
project(lab5)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_SOURCE_DIR build)

include_directories(src/libs/thread-pool)
add_subdirectory(src/libs/thread-pool)

include_directories(src/libs/graph-bmp-generator)
add_subdirectory(src/libs/graph-bmp-generator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(libpqxx REQUIRED)
find_package(libconfig REQUIRED)


add_executable(main main.cpp
        src/server/Server.h
        src/server/Server.cpp
        src/server/Socket.cpp
        src/server/PollingWrapper.cpp
        src/server/handlers/FileUploadHandler.cpp
)

target_link_libraries(main thread-pool graph-bmp-generator libpqxx::pqxx)