#ifndef LAB4_BMP_GRAPH_GENERATOR_GRAPHGEN_H
#define LAB4_BMP_GRAPH_GENERATOR_GRAPHGEN_H

#include <graph/Graph.h>
#include <graph/algo/FruchtermanReingold.h>
#include <image/BMP.h>
#include <math/structures.h>

// function for drawing graph
void draw_graph(const std::string& path, const std::vector<Point2D>& positions, const Graph& g,
                size_t width, size_t height, size_t scale);
void generate_graph(const std::string& input_path, const std::string& output);


#endif  //LAB4_BMP_GRAPH_GENERATOR_GRAPHGEN_H
