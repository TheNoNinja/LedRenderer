#pragma once

#include <cstdint>

namespace Math {
    inline float lerp(const float a, const float b, const float t) {
        return a + (b - a) * t;
    }

    inline float clamp(const float v, const float l = 0.0f, const float h = 1.0f) {
        return v < l ? l : v > h ? h : v;
    }

    inline float fract(const float v) {
        return v - static_cast<float>(static_cast<int>(v));
    }

    inline float remap(const float v, const float il, const float ih, const float ol, const float oh) {
        const float t = (v - il) / (ih - il);
        return lerp(ol, oh, t);
    }

    inline uint8_t lerpByte(const uint8_t a, const uint8_t b, const float t) {
        return static_cast<uint8_t>(lerp(a, b, clamp(t)));
    }

}