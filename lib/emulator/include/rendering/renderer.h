#pragma once

#include "led_engine.h"
#include <SDL3/SDL_events.h>

#include "camera.h"

namespace Emulator {
    class Renderer {
    public:
        explicit Renderer(int viewportWidth = 1280, int viewportHeight = 720);
        void resizeViewport(int viewportWidth, int viewportHeight);
        void onEvent(const SDL_Event &event);
        void render(const Pixel *pixels, const Color *colors, size_t numberOfPixels);

    private:
        int mViewportWidth, mViewportHeight;
        Camera mCamera;
    };
}
