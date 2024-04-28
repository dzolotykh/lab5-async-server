#ifndef STRUCTURES_H
#define STRUCTURES_H

struct Point2D;

struct Vector2D {
    double x, y;

    explicit Vector2D(double _x = 0, double _y = 0) noexcept : x(_x), y(_y) {}

    explicit operator Point2D() const noexcept;

    Vector2D& operator+=(const Vector2D& other) noexcept;

    Vector2D& operator-=(const Vector2D& other) noexcept;

    Vector2D& operator*=(double scalar) noexcept;

    Vector2D& operator/=(double scalar) noexcept;

    double size() const noexcept;
};

Vector2D operator+(const Vector2D& l, const Vector2D& r) noexcept;

Vector2D operator-(const Vector2D& l, const Vector2D& r) noexcept;

Vector2D operator*(const Vector2D& l, double scalar) noexcept;

Vector2D operator*(double scalar, const Vector2D& r) noexcept;
Vector2D operator/(const Vector2D& l, double scalar) noexcept;

struct Point2D {
    double x, y;

    explicit Point2D(double _x = 0, double _y = 0) : x(_x), y(_y) {}

    explicit operator Vector2D() const noexcept;

    Point2D& operator+=(const Vector2D& other) noexcept;

    Point2D& operator-=(const Vector2D& other) noexcept;
};

Point2D operator+(const Point2D& l, const Vector2D& s) noexcept;

Point2D operator-(const Point2D& l, const Vector2D& s) noexcept;

Vector2D operator-(const Point2D& l, const Point2D& s) noexcept;

#endif  // STRUCTURES_H
