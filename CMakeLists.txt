cmake_minimum_required(VERSION 3.20)
project(lab5)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/libs/thread-pool)
add_subdirectory(src/libs/thread-pool)

include_directories(src/libs/graph-bmp-generator)
add_subdirectory(src/libs/graph-bmp-generator)

add_executable(main main.cpp)

target_link_libraries(main thread-pool graph-bmp-generator)