cmake_minimum_required(VERSION 3.22)
project(graphgen)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set (SOURCE_FILES
        src/graph/algo/FruchtermanReingold.cpp
        src/graph/Graph.cpp
        src/image/BMP.cpp
        src/math/structures.cpp
        graphgen.cpp
        resources/resourses.h
)

add_library(graphgen STATIC ${SOURCE_FILES})

target_include_directories(graphgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

