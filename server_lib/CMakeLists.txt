cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES server/Server.cpp
                 server/PollingWrapper.cpp
                 server/handlers/AbstractHandler.cpp
                 server/handlers/EndpointHandler.cpp
                 server/thread-pool/Pool.cpp
)

add_library(async-server STATIC ${SOURCE_FILES})


target_include_directories(async-server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/server)