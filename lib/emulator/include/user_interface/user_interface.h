#pragma once
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui.h>

namespace UserInterface {
    bool init(SDL_Window* window, SDL_GLContext* glContext);
    bool onEvent(const SDL_Event& event);
    ImVec2 beginFrame();
    void endFrame(GLuint viewportTextureId);
    void shutdown();
}
