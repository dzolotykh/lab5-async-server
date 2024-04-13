cmake_minimum_required(VERSION 3.20)
project(lab5)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_SOURCE_DIR build)

include_directories(src/libs/thread-pool)
add_subdirectory(src/libs/thread-pool)

include_directories(src/libs/graph-bmp-generator)
add_subdirectory(src/libs/graph-bmp-generator)

include_directories(src/database)
include_directories(src/server/handlers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(libpqxx REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(GTest REQUIRED)

add_executable(main main.cpp
        src/server/Server.cpp
        src/server/Socket.cpp
        src/server/PollingWrapper.cpp
        src/server/handlers/FileUploadHandler.cpp
        src/database/ConnectionPool.cpp
        src/server/handlers/EndpointHandler.cpp
        src/server/handlers/ResultRequestHandler.cpp
        src/server/handlers/FileDownloadHandler.cpp
        src/server/handlers/AbstractHandler.cpp
)

target_link_libraries(main thread-pool graph-bmp-generator libpqxx::pqxx JsonCpp::JsonCpp GTest::GTest)