#include "graphgen.h"

// function for drawing graph
void draw_graph(const std::string& path, const std::vector<Point2D>& positions, const Graph& g,
                size_t width, size_t height, size_t scale) {
    BMP image(width, height);  // create image
    for (auto i : positions) {
        image.draw_circle(static_cast<int>(i.x), static_cast<int>(i.y), 10);  // draws vertex
    }

    // draw edges
    for (int i = 0; i < g.size(); i++) {
        for (auto j : g.get_neighbours(i)) {
            if (j > i)
                continue;
            image.draw_segment(static_cast<int>(positions[i].x), static_cast<int>(positions[i].y),
                               static_cast<int>(positions[j].x), static_cast<int>(positions[j].y));
        }
    }

    // draw number of vertex width more than 1 pixel
    for (Graph::vertexT v = 0; v < g.size(); ++v) {
        auto i = positions[v];
        image.draw_number(static_cast<int>(i.x) + 15, static_cast<int>(i.y), v, scale);
    }
    image.save(path);
}

void generate_graph(const std::string& input_path, const std::string& output) {
    size_t width = 2048;
    size_t height = 2048;
    size_t scale = 2;
    Graph g(input_path);
    FruchtermanReingold f(g, width, height);
    int iter = 1000;
    f.run(iter);
    draw_graph(output, f.get_positions(), g, width, height, scale);
}
