#pragma once

#include "core/pixel.h"

#include <cstddef>

class PixelMapper {
public:
    PixelMapper(Pixel* pixels, size_t numberOfPixels);

    void reset();
    void setLedStrip(int ledStrip);

    bool mapPixel(const Point& position, const Point& normal = Point::Zero());
    bool mapLine(const Point& start, const Point& step, int count, const Point& normal = Point::Zero());
    bool mapMatrix(const Point& start, int columns, int rows, const Point& columnDelta, const Point& rowDelta, const Point& normal = Point::Zero(), bool hasZigZagPattern = true);

private:
    bool canWrite(std::size_t count) const;

    Pixel* mPixels = nullptr;
    size_t mNumberOfPixels = 0;
    size_t mIndex = 0;
    int mLedStrip = 1;
};