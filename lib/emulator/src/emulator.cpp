#include "emulator.h"

#include <cstdio>
#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "rendering/framebuffer.h"
#include "rendering/renderer.h"
#include "user_interface/user_interface.h"

namespace Emulator {
    static bool ShouldQuit = false;
    static SDL_Window* Window = nullptr;
    static SDL_GLContext GlContext = nullptr;
    static Pixel* Pixels = nullptr;
    static Color* Colors = nullptr;
    static size_t NumberOfPixels = 0;

    static Renderer* renderer = nullptr;
    static Emulator::FrameBuffer* frameBuffer = nullptr;

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

    bool init(const int windowWidth, const int windowHeight) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            logSdlError("SDL_Init failed");
            cleanPartialInit();
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
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

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            fprintf(stderr, "gladLoadGLLoader failed\n");
            cleanPartialInit();
            return false;
        }

        if (!UserInterface::init(Window, &GlContext)) {
            fprintf(stderr, "UserInterface::init() failed");
            cleanPartialInit();
            return false;
        }

        renderer = new Renderer(windowWidth, windowHeight);
        frameBuffer = new Emulator::FrameBuffer(windowWidth, windowHeight);
        return true;
    }

    void render() {
        if (!renderer) return;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                ShouldQuit = true;
                return;
            }

            if (UserInterface::onEvent(e)) continue;

            renderer->onEvent(e);
        }

        const ImVec2 viewportSize = UserInterface::beginFrame();

        const int targetWidth = std::max(1, static_cast<int>(viewportSize.x));
        const int targetHeight = std::max(1, static_cast<int>(viewportSize.y));
        if (targetWidth != frameBuffer->getWidth() || targetHeight != frameBuffer->getHeight()) {
            frameBuffer->resize(targetWidth, targetHeight);
            renderer->resizeViewport(targetWidth, targetHeight);
        }

        frameBuffer->bind();
        renderer->render(Pixels, Colors, NumberOfPixels);
        Emulator::FrameBuffer::unbind();
        UserInterface::endFrame(frameBuffer->getTextureId());

        SDL_GL_SwapWindow(Window);
    }

    void shutdown() {
        if (frameBuffer) {
            frameBuffer->destroy();
            delete frameBuffer;
            frameBuffer = nullptr;
        }
        delete renderer;
        renderer = nullptr;
        UserInterface::shutdown();
        SDL_GL_DestroyContext(GlContext);
        SDL_DestroyWindow(Window);
        SDL_Quit();
    }

    int run(const int windowWidth, const int windowHeight, Pixel* pixels, Color* colors, const size_t numberOfPixels) {
        Pixels = pixels;
        NumberOfPixels = numberOfPixels;
        Colors = colors;

        setup();
        if (!init(windowWidth, windowHeight)) return 1;

        while (!ShouldQuit) {
            loop();
            render();
        }

        shutdown();
        return 0;
    }
}
