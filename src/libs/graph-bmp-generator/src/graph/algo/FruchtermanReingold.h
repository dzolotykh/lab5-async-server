#ifndef FRUCHTERMANREINGOLD_H
#define FRUCHTERMANREINGOLD_H

#include <type_traits>
#include "../../math/structures.h"
#include "../Graph.h"

// class that executes Fruchterman&Reingold algorithm with the provided graph
// you can read about it here: https://reingold.co/force-directed.pdf
class FruchtermanReingold {
   public:
    // width and height are the parameters of the image on which the graph will be
    // drawn k is hyperparameter before algorithm run all vertices are positioned
    // in circle with radius 1
    FruchtermanReingold(const Graph& g, size_t width, size_t height, double k = 15.0);

    // callback function is used to provide information than the iteration with
    // the number i completed. after all iterations are completed, all positions
    // are scaled to the width * height of the image
    template <typename callbackT = void(int)>
    void run(
        size_t iter, callbackT callback = [](int) {}) noexcept {
        for (int i = 0; i < iter; ++i) {
            run_iter();
            callback(i);
        }
        scale();
    }

    // returns positions of all points
    std::vector<Point2D> get_positions() const noexcept;

   private:
    const Graph& g_;
    const double k_;
    double temp_;
    const size_t width_;
    const size_t height_;
    std::vector<Vector2D> offset_;
    std::vector<Point2D> positions_;

    inline double fa(double x) const noexcept;    // technical function for the algorithm
    inline double fr(double x) const noexcept;    // technical functions for the algorithm
    void run_iter() noexcept;                     // runs iteration of algorithm
    void scale() noexcept;                        // scales all positions with image size
};

#endif    // FRUCHTERMANREINGOLD_H
