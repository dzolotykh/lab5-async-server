cmake_minimum_required(VERSION 3.20)
project(graph-bmp-generator)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_LIB
        src/graph/algo/FruchtermanReingold.cpp
        src/graph/Graph.cpp
        src/image/BMP.cpp
        src/math/structures.cpp
)

add_library(graph-bmp-generator STATIC ${SOURCE_LIB})
