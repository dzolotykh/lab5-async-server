#include "Graph.h"
#include <fstream>
#include <iostream>

// construct graph from provided file. Throws exception if it could not open the file
Graph::Graph(const std::string& path)
{
    std::ifstream fs(path);
    if (!fs.is_open())
    {
        throw std::runtime_error("Cannot open the file " + path);
    }
    size_t vertex_num, edges_num;
    fs >> vertex_num >> edges_num;
    g_.resize(vertex_num);
    for (size_t i = 0; i < edges_num; ++i)
    {
        Graph::vertexT u, v;
        fs >> u >> v;
        g_[u].push_back(v);
        g_[v].push_back(u);
    }
}

// returns vector with vertices that are adjacent to the specified one
const std::vector<Graph::vertexT>& Graph::get_neighbours(Graph::vertexT vertex_num) const noexcept
{
    return g_[vertex_num];
}

size_t Graph::size() const noexcept
{
    return g_.size();
}