#include "pixel_mapping/pixel_mapper.h"

#include <algorithm>

PixelMapper::PixelMapper(Pixel *pixels, const size_t numberOfPixels)
    : mPixels(pixels), mNumberOfPixels(numberOfPixels) {
    reset();
}

void PixelMapper::reset() {
    for (size_t i = 0; i < mNumberOfPixels; ++i) {
        mPixels[i] = Pixel();
    }
    mIndex = 0;
    mLedStrip = 1;
}

void PixelMapper::setLedStrip(const int ledStrip) {
    mLedStrip = ledStrip;
}

bool PixelMapper::mapPixel(const Point &position, const Point &normal) {
    if (!canWrite(1)) return false;
    mPixels[mIndex++] = Pixel(mLedStrip, mIndex, position, normal);
    return true;
}

bool PixelMapper::mapLine(const Point &start, const Point& step, const int count, const Point &normal) {
    if (!canWrite(count)) return false;
    for (int i = 0; i < count; ++i) {
        const Point position = start + step * i;
        if (!mapPixel(position, normal)) return false;
    }
    return true;
}

bool PixelMapper::mapMatrix(const Point &start, int columns, int rows, const Point &columnDelta, const Point &rowDelta, const Point &normal, bool hasZigZagPattern) {
    const size_t total = columns * rows;
    if (!canWrite(total)) return false;
    for (int row = 0; row < rows; ++row) {
        const bool shouldBeReversed = hasZigZagPattern && (row & 1);
        for (int column = 0; column < columns; ++column) {
            const int mappedCol = shouldBeReversed ? columns - 1 - column : column;
            const Point position = start + columnDelta * mappedCol + rowDelta * row;
            if (!mapPixel(position, normal)) return false;
        }
    }
    return true;
}

bool PixelMapper::canWrite(const std::size_t count) const {
    return mPixels && mIndex + count <= mNumberOfPixels;
}
