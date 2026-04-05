#pragma once

#include <led_engine.h>

class GlareEffect : public Effect {
public:
    GlareEffect() {
        setBlendMode(BlendMode::Alpha);
    }

    void onUpdate(float /*dt*/, float elapsed, const Pixel* pixels, Color* colors, std::size_t count) override {
        const float speed = 3.0f;
        const float range = 35.0f;
        const float centerX = std::sin(elapsed * speed) * range * 0.5f + range * 0.5f;

        const float radius = 4.0f;

        for (std::size_t i = 0; i < count; ++i) {
            const Point& pos = pixels[i].position;
            const float dx = pos.x - centerX;
            const float dz = pos.z;
            const float dist = std::sqrt(dx * dx + dz * dz);
            const float t = Math::clamp(1.0f - dist / radius);
            colors[i] = Color(255, 255, 255, static_cast<uint8_t>(Math::clamp(t * 300, 0, 255)));
        }
    }
};