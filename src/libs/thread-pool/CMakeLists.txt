cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(thread-pool)

set(SOURCE_LIB Pool.cpp)

add_library(thread-pool STATIC ${SOURCE_LIB})
