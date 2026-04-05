#pragma once

#include <algorithm>
#include <cstdint>

#include "math.h"

enum class BlendMode : uint8_t {
    Replace,
    Add,
    Alpha
};

struct Color {
    uint8_t r, g, b, a;

    explicit Color(const uint8_t r = 0, const uint8_t g = 0, const uint8_t b = 0, const uint8_t a = 255) : r(r), g(g), b(b), a(a) {}

    static Color Black()   { return Color(0,   0,   0  ); }
    static Color Red()     { return Color(255, 0,   0  ); }
    static Color Green()   { return Color(0,   255, 0  ); }
    static Color Blue()    { return Color(0,   0,   255); }
    static Color Yellow()  { return Color(255, 255, 0  ); }
    static Color Cyan()    { return Color(0,   255, 255); }
    static Color Magenta() { return Color(255, 0,   255); }
    static Color White()   { return Color(255, 255, 255); }

    void applyBlend(const BlendMode mode, const Color& other) {
        switch (mode) {
            case BlendMode::Add:
                 *this = this->operator+(other);
                break;
            case BlendMode::Alpha: {
                const float t = static_cast<float>(other.a) / 255.0f;
                 *this = lerp(other, t);
                break;
            }
            default:
            case BlendMode::Replace:
                *this = other;
                break;
        }
    }

    [[nodiscard]] Color lerp(const Color& other, const float t) const { return Color(Math::lerpByte(r, other.r, t), Math::lerpByte(g, other.g, t), Math::lerpByte(b, other.b, t), Math::lerpByte(a, other.a, t)); }

    bool operator==(const Color& other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }
    bool operator!=(const Color& other) const { return !(*this == other); }

    Color operator+(const Color& other) const {
        return Color(
            static_cast<uint8_t>(std::min(255, r + other.r)),
            static_cast<uint8_t>(std::min(255, g + other.g)),
            static_cast<uint8_t>(std::min(255, b + other.b))
        );
    }
};
