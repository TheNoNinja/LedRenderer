#include "emulator.h"

#include <cstdio>
#include <SDL3/SDL.h>

#include "rendering/renderer.h"

namespace Emulator {
    bool ShouldQuit = false;
    static SDL_Window* Window = nullptr;
    static SDL_GLContext GlContext = nullptr;
    static Pixel* Pixels = nullptr;
    static size_t NumberOfPixels = 0;

    Renderer renderer;

    static void logSdlError(const char* message) {
        fprintf(stderr, "%s: %s\n", message, SDL_GetError());
    }

    static void cleanPartialInit() {
        if (GlContext) {
            SDL_GL_DestroyContext(GlContext);
            GlContext = nullptr;
        }

        if (Window) {
            SDL_DestroyWindow(Window);
            Window = nullptr;
        }

        SDL_Quit();
    }

    bool init(const int windowWidth, const int windowHeight, Pixel* pixels, const size_t numberOfPixels) {
        Pixels = pixels;
        NumberOfPixels = numberOfPixels;

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            logSdlError("SDL_Init failed");
            cleanPartialInit();
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        Window = SDL_CreateWindow("LED Glasses Emulator", windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        if (!Window) {
            logSdlError("SDL_CreateWindow failed");
            cleanPartialInit();
            return false;
        }

        GlContext = SDL_GL_CreateContext(Window);

        if (!GlContext) {
            logSdlError("SDL_GL_CreateContext failed");
            cleanPartialInit();
            return false;
        }

        if (!SDL_GL_MakeCurrent(Window, GlContext)) {
            logSdlError("SDL_GL_MakeCurrent failed");
            cleanPartialInit();
            return false;
        }

        if (!SDL_GL_SetSwapInterval(1)) {
            logSdlError("SDL_GL_SetSwapInterval failed");
        }

        renderer = Renderer(windowWidth, windowHeight);
        return true;
    }

    void render(const Color* colors) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                ShouldQuit = true;
                return;
            }

            renderer.onEvent(e);
        }

        renderer.render(Pixels, colors, NumberOfPixels);

        SDL_GL_SwapWindow(Window);
    }

    void shutdown() {
        SDL_GL_DestroyContext(GlContext);
        SDL_DestroyWindow(Window);
        SDL_Quit();
    }
}
