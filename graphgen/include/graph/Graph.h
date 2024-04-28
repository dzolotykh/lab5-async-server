#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <string>
#include <vector>

// simple wrapper for graph structure
class Graph {
   public:
    using vertexT = size_t;
    size_t size() const noexcept;

    // returns vector with vertices that are adjacent to the specified one
    const std::vector<vertexT>& get_neighbours(vertexT vertex_num) const noexcept;

    // construct graph from provided file. Throws exception if it could not open the file
    explicit Graph(const std::string& path);

   private:
    using containerT = std::vector<std::vector<size_t>>;

    containerT g_;
};

#endif  // GRAPH_H
