#pragma once

#include "point.h"

struct Pixel {
    int index;
    int ledStrip = 1;
    Point position;
    Point normal;

    explicit Pixel(const int index = -1, const Point& position = Point::Zero(), const Point& normal = Point::Zero()) : index(index), position(position), normal(normal){}
    explicit Pixel(const int ledStrip, const int index, const Point& position = Point::Zero(), const Point& normal = Point::Zero()) : index(index), ledStrip(ledStrip), position(position), normal(normal){}
};