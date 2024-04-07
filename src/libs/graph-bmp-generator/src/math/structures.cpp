#include "structures.h"
#include <cmath>
#include <limits>

Vector2D operator+(const Vector2D& l, const Vector2D& r) noexcept {
    return Vector2D(l.x + r.x, l.y + r.y);
}

Vector2D operator-(const Vector2D& l, const Vector2D& r) noexcept {
    return Vector2D(l.x - r.x, l.y - r.y);
}

Vector2D operator*(const Vector2D& l, double scalar) noexcept {
    return Vector2D(l.x * scalar, l.y * scalar);
}

Vector2D operator*(double scalar, const Vector2D& r) noexcept {
    return Vector2D(r.x * scalar, r.y * scalar);
}

Vector2D operator/(const Vector2D& l, double scalar) noexcept {
    if (scalar == 0) {
        double _x = std::numeric_limits<double>::max();
        if (l.x < 0)
            _x *= -1;
        double _y = std::numeric_limits<double>::max();
        if (l.y < 0)
            _y *= -1;
        return Vector2D(_x, _y);
    }

    return Vector2D(l.x / scalar, l.y / scalar);
}

Vector2D& Vector2D::operator+=(const Vector2D& other) noexcept {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& other) noexcept {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2D& Vector2D::operator*=(double scalar) noexcept {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2D& Vector2D::operator/=(double scalar) noexcept {
    if (scalar == 0) {
        double _x = std::numeric_limits<double>::max();
        if (x < 0)
            _x *= -1;
        double _y = std::numeric_limits<double>::max();
        if (y < 0)
            _y *= -1;
        x = _x;
        y = _y;
    } else {
        x /= scalar;
        y /= scalar;
    }
    return *this;
}

double Vector2D::size() const noexcept {
    return std::sqrt(x * x + y * y);
}

Point2D operator+(const Point2D& l, const Vector2D& s) noexcept {
    return Point2D(l.x + s.x, l.y + s.y);
}

Point2D operator-(const Point2D& l, const Vector2D& s) noexcept {
    return Point2D(l.x - s.x, l.y - s.y);
}

Vector2D operator-(const Point2D& l, const Point2D& s) noexcept {
    return Vector2D(l.x - s.x, l.y - s.y);
}

Point2D::operator Vector2D() const noexcept {
    return Vector2D(x, y);
}

Point2D& Point2D::operator+=(const Vector2D& other) noexcept {
    x += other.x;
    y += other.y;
    return *this;
}

Point2D& Point2D::operator-=(const Vector2D& other) noexcept {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2D::operator Point2D() const noexcept {
    return Point2D(x, y);
}