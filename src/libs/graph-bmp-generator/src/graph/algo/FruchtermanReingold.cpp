#include "FruchtermanReingold.h"
#include <cmath>
#include <limits>

FruchtermanReingold::FruchtermanReingold(const Graph& g, size_t width, size_t height, double k)
    : g_(g), k_(k), temp_(static_cast<double>(g.size())), offset_(g_.size()), positions_(g.size()),
      width_(width), height_(height)
{
    double angle = 2.0 * M_PI / static_cast<double>(g.size());
    for (Graph::vertexT v_id = 0; v_id < g.size(); v_id++)
    {
        positions_[v_id].x = cos(static_cast<double>(v_id) * angle);
        positions_[v_id].y = sin(static_cast<double>(v_id) * angle);
    }
}

inline double FruchtermanReingold::fa(double x) const noexcept
{
    return x * x / k_;
}

inline double FruchtermanReingold::fr(double x) const noexcept
{
    return k_ * k_ / x;
}

std::vector<Point2D> FruchtermanReingold::get_positions() const noexcept
{
    return positions_;
}

void FruchtermanReingold::scale() noexcept
{
    // find current width and height
    double x_min = std::numeric_limits<double>::max();
    double x_max = std::numeric_limits<double>::lowest();
    double y_min = std::numeric_limits<double>::max();
    double y_max = std::numeric_limits<double>::lowest();

    for (Graph::vertexT v_id = 0; v_id < g_.size(); v_id++)
    {
        x_min = std::min(x_min, positions_[v_id].x);
        x_max = std::max(x_max, positions_[v_id].x);
        y_min = std::min(y_min, positions_[v_id].y);
        y_max = std::max(y_max, positions_[v_id].y);
    }

    double cur_width = x_max - x_min;
    double cur_height = y_max - y_min;

    // compute scale factor
    double x_scale = static_cast<double>(width_) / cur_width;
    double y_scale = static_cast<double>(height_) / cur_height;
    double scale = 0.9 * std::min(x_scale, y_scale);

    // apply scale
    Vector2D center(x_max + x_min, y_max + y_min);
    Vector2D offset = center / 2.0 * scale - Vector2D{ static_cast<double>(width_) / 2.0,
                                                       static_cast<double>(height_) / 2.0 };
    for (Graph::vertexT v_id = 0; v_id < g_.size(); v_id++)
    {
        positions_[v_id] = (Point2D)((Vector2D)positions_[v_id] * scale - offset);
    }
}

void FruchtermanReingold::run_iter() noexcept
{
    std::fill(offset_.begin(), offset_.end(), Vector2D(0, 0));  // clear offsets

    // compute repulsive forces
    for (Graph::vertexT v = 0; v < g_.size(); ++v)
    {
        for (Graph::vertexT u = v + 1; u < g_.size(); ++u)
        {
            auto delta = positions_[v] - positions_[u];
            offset_[v] += (delta * fr(delta.size()) / delta.size());
            offset_[u] -= (delta * fr(delta.size()) / delta.size());
        }
    }

    // compute attractive forces
    for (Graph::vertexT v = 0; v < g_.size(); ++v)
    {
        for (Graph::vertexT u : g_.get_neighbours(v))
        {
            if (u > v)
                continue;
            auto delta = positions_[v] - positions_[u];
            offset_[v] -= (delta * fa(delta.size()) / delta.size());
            offset_[u] += (delta * fa(delta.size()) / delta.size());
        }
    }

    // apply offsets
    for (Graph::vertexT v = 0; v < g_.size(); ++v)
    {
        positions_[v] += (offset_[v] * std::min(temp_, offset_[v].size()) / offset_[v].size());
    }

    // cooling down the temperature
    temp_ *= 0.99;
    if (temp_ < 0.001)
    {
        temp_ = 0.001;
    }
}
