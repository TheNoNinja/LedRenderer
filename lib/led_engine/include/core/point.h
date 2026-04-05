#pragma once
#include <cmath>

struct Point {
    float x, y, z;

    explicit Point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z) {}

    static Point Zero()     { return Point( 0,  0,  0); }
    static Point One()      { return Point( 1,  1,  1); }
    static Point Left()     { return Point(-1,  0,  0); }
    static Point Right()    { return Point( 1,  0,  0); }
    static Point Up()       { return Point( 0,  1,  0); }
    static Point Down()     { return Point( 0, -1,  0); }
    static Point Forward()  { return Point( 0,  0,  1); }
    static Point Backward() { return Point( 0,  0, -1); }

    [[nodiscard]] float dot(const Point& other) const { return x * other.x + y * other.y + z * other.z; }
    [[nodiscard]] float length() const { return std::sqrt(this->dot(*this)); }
    [[nodiscard]] float distance(const Point& other) const { return (*this - other).length(); }

    bool operator==(const Point& other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!=(const Point& other) const { return !(*this == other); }

    Point operator*(const float scalar) const { return Point(x * scalar, y * scalar, z * scalar); }
    Point operator*(const int scalar) const { return this->operator*(static_cast<float>(scalar)); }

    Point operator+(const Point& other) const { return Point(x + other.x, y + other.y, z + other.z); }
    Point operator-(const Point& other) const { return Point(x - other.x, y - other.y, z - other.z); }
};
