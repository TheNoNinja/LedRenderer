#pragma once

#include <led_engine.h>
#include <cstdint>
#include <SDL3/SDL.h>

namespace Emulator {
    extern bool ShouldQuit;
    bool init(int windowWidth, int windowHeight, Pixel* pixels, std::size_t numberOfPixels);
    void render(const Color* colors);
    void shutdown();
}

inline float millis() {
    return static_cast<float>(SDL_GetTicks()) / 1000.0f;
}
