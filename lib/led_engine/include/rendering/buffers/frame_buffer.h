#pragma once

#include "core/color.h"

template<int SIZE>
class FrameBuffer {
public:
    FrameBuffer() = default;

    static bool isIndexInBounds(const int index) {
        return index >= 0 && index < SIZE;
    }

    void clear(const Color& color = Color::Black()) {
        for (int i = 0; i < SIZE; ++i) {
            mPixels[i] = color;
        }
    }

    void setPixel(const int index, const Color& color) {
        if (!isIndexInBounds(index))return;
        mPixels[index] = color;
    }

    void getPixel(const int index, Color& color) const {
        if (!isIndexInBounds(index))return;
        color = mPixels[index];
    }

    void blendPixel(const int index, const Color& color, const BlendMode mode) {
        if (!isIndexInBounds(index))return;
        mPixels[index].applyBlend(mode, color);
    }

    [[nodiscard]] const Color* getPixels() const { return mPixels; }

private:
    Color mPixels[SIZE];
};