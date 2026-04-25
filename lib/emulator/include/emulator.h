#pragma once

#include <led_engine.h>
#include <cstdint>
#include <SDL3/SDL.h>

void setup();
void loop();

namespace Emulator {
    int run(int windowWidth, int windowHeight, Pixel* pixels, Color* colors, std::size_t numberOfPixels);
}

inline float millis() {
    return static_cast<float>(SDL_GetTicks());
}
