#pragma once
#include <SDL3/SDL.h>

namespace UserInterface {
    bool init(SDL_Window* window, SDL_GLContext* glContext);
    bool onEvent(const SDL_Event& event);
    void render();
    void shutdown();
}
