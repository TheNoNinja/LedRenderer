#pragma once

#include <led_engine.h>

class ColorWaveEffect : public Effect {
public:
    void onUpdate(float /*dt*/, const float elapsed, const Pixel* pixels, Color* colors, const std::size_t count) override {
        for (std::size_t i = 0; i < count; ++i) {
            const Point& pos = pixels[i].position;
            const float wave = std::sin((pos.x + pos.z + pos.y) * 0.25f + elapsed * 9.0f) * 0.5f + 0.5f;
            const float r = std::sin(wave * 3.14159f + 0.0f)   * 0.5f + 0.5f;
            const float g = std::sin(wave * 3.14159f + 2.094f) * 0.5f + 0.5f;
            const float b = std::sin(wave * 3.14159f + 4.189f) * 0.5f + 0.5f;
            colors[i] = Color(
                static_cast<uint8_t>(r * 255),
                static_cast<uint8_t>(g * 255),
                static_cast<uint8_t>(b * 255)
            );
        }
    }
};