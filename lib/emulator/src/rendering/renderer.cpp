#include "rendering/renderer.h"

#include <algorithm>
#include <cmath>
#include <glad/glad.h>

#define POINT_SIZE 8.0f

namespace Emulator {
    Renderer::Renderer(const int viewportWidth, const int viewportHeight) {
        mViewportWidth = viewportWidth;
        mViewportHeight = viewportHeight;

        const float aspectRatio = mViewportHeight != 0 ? static_cast<float>(mViewportWidth) / static_cast<float>(mViewportHeight) : 16.0 / 9.0f;
        mCamera = Camera(45.0f, aspectRatio, 0.1f, 1000.0f, 150.0f);
        glViewport(0, 0, viewportWidth, viewportHeight);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::resizeViewport(const int viewportWidth, const int viewportHeight) {
        mViewportWidth = viewportWidth;
        mViewportHeight = viewportHeight;
        const float aspectRatio = mViewportHeight != 0 ? static_cast<float>(mViewportWidth) / static_cast<float>(mViewportHeight) : 16.0 / 9.0f;
        mCamera.setAspectRatio(aspectRatio);
        glViewport(0, 0, viewportWidth, viewportHeight);
    }

    void Renderer::onEvent(const SDL_Event &event) {
        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            resizeViewport(event.window.data1, event.window.data2);
        }

        mCamera.onEvent(event);
    }

    void Renderer::render(const Pixel* pixels, const Color* colors, const size_t numberOfPixels) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!pixels || !colors || numberOfPixels == 0) return;

        float lowestY = std::numeric_limits<float>::max();
        float x = 0.0f, y = 0.0f, z = 0.0f;
        for (size_t i = 0; i < numberOfPixels; ++i) {
            x += pixels[i].position.x;
            y += pixels[i].position.y;
            z += pixels[i].position.z;
            lowestY = std::min(lowestY, pixels[i].position.y);
        }
        x /= numberOfPixels;
        y /= numberOfPixels;
        z /= numberOfPixels;

        mCamera.setTarget(x, y, z);

        float ex, ey, ez;
        mCamera.getEyePosition(ex, ey, ez);

        const Mat4 proj = mCamera.getProjectionMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(proj.data());

        const Mat4 view = mCamera.getViewMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(view.data());

        constexpr int kHalfExtent = 50;
        constexpr int kStep = 5;

        glDepthMask(GL_FALSE);
        glBegin(GL_LINES);
        for (int i = -kHalfExtent; i <= kHalfExtent; i += kStep) {
            constexpr float kBrightColor = 0.20f;
            constexpr float kDimColor = 0.15f;
            const bool isMajor = i % (kStep * 2) == 0;
            const float c = isMajor ? kBrightColor : kDimColor;
            glColor3f(c, c, c);

            glVertex3f(static_cast<float>(i) + x, lowestY - 1.0f, static_cast<float>(-kHalfExtent) + z);
            glVertex3f(static_cast<float>(i) + x, lowestY - 1.0f, static_cast<float>(kHalfExtent) + z);

            glVertex3f(static_cast<float>(-kHalfExtent) + x, lowestY - 1.0f, static_cast<float>(i) + z);
            glVertex3f(static_cast<float>(kHalfExtent) + x, lowestY - 1.0f, static_cast<float>(i) + z);
        }
        {
            Pixel lastPixel = pixels[0];
            for (size_t i = 1; i < numberOfPixels; ++i) {
                auto& pixel = pixels[i];

                float dx = ex - pixel.position.x;
                float dy = ey - pixel.position.y;
                float dz = ez - pixel.position.z;
                if (const float len = std::sqrt(dx*dx + dy*dy + dz*dz); len > 1e-6f) {dx /= len; dy /= len; dz /= len;}

                const float dot = pixel.normal == Point::Zero() ? 1.0f : pixel.normal.x * dx + pixel.normal.y * dy + pixel.normal.z * dz + 0.25f;
                const float brightness = Math::lerp(1.0f, 0.0f, std::clamp(dot, 0.0f, 1.0f));

                if (pixel.ledStrip == lastPixel.ledStrip) {
                    if (pixel.index == lastPixel.index + 1) {
                        glColor4f(0, 0, brightness, brightness);
                        glVertex3f(lastPixel.position.x, lastPixel.position.y, lastPixel.position.z);
                        glVertex3f(pixel.position.x, pixel.position.y, pixel.position.z);
                    }
                }

                lastPixel = pixel;
            }
        }
        glEnd();


        glPointSize(POINT_SIZE * 1.2f);
        glBegin(GL_POINTS);

        Pixel lastPixel = Pixel(-1, -1, Point::Zero(), pixels[0].normal);
        for (size_t i = 0; i < numberOfPixels; ++i) {
            auto& pixel = pixels[i];

            if (pixel.ledStrip != lastPixel.ledStrip) {
                if (lastPixel.ledStrip != -1){
                    float dx = ex - lastPixel.position.x;
                    float dy = ey - lastPixel.position.y;
                    float dz = ez - lastPixel.position.z;
                    if (const float len = std::sqrt(dx*dx + dy*dy + dz*dz); len > 1e-6f) {dx /= len; dy /= len; dz /= len;}

                    const float dot = lastPixel.normal == Point::Zero() ? 1.0f : lastPixel.normal.x * dx + lastPixel.normal.y * dy + lastPixel.normal.z * dz + 0.25f;
                    const float brightness = Math::lerp(1.0f, 0.0f, std::clamp(dot, 0.0f, 1.0f));
                    glColor4f(brightness, 0, 0, brightness);
                    glVertex3f(lastPixel.position.x, lastPixel.position.y, lastPixel.position.z);
                }

                {
                    float dx = ex - pixel.position.x;
                    float dy = ey - pixel.position.y;
                    float dz = ez - pixel.position.z;
                    if (const float len = std::sqrt(dx*dx + dy*dy + dz*dz); len > 1e-6f) {dx /= len; dy /= len; dz /= len;}

                    const float dot = pixel.normal == Point::Zero() ? 1.0f : pixel.normal.x * dx + pixel.normal.y * dy + pixel.normal.z * dz + 0.25f;
                    const float brightness = Math::lerp(1.0f, 0.0f, std::clamp(dot, 0.0f, 1.0f));
                    glColor4f(0, brightness, 0, brightness);
                    glVertex3f(pixel.position.x, pixel.position.y, pixel.position.z);
                }
            }

            lastPixel = pixel;
        }

        {
            lastPixel = pixels[numberOfPixels - 1];
            float dx = ex - lastPixel.position.x;
            float dy = ey - lastPixel.position.y;
            float dz = ez - lastPixel.position.z;
            if (const float len = std::sqrt(dx*dx + dy*dy + dz*dz); len > 1e-6f) {dx /= len; dy /= len; dz /= len;}

            const float dot = lastPixel.normal == Point::Zero() ? 1.0f : lastPixel.normal.x * dx + lastPixel.normal.y * dy + lastPixel.normal.z * dz + 0.25f;
            const float brightness = Math::lerp(1.0f, 0.0f, std::clamp(dot, 0.0f, 1.0f));
            glColor4f(brightness, 0, 0, brightness);
            glVertex3f(lastPixel.position.x, lastPixel.position.y, lastPixel.position.z);
        }

        glEnd();
        glDepthMask(GL_TRUE);
        glPointSize(POINT_SIZE);
        glBegin(GL_POINTS);

        for (size_t i = 0; i < numberOfPixels; ++i) {
            const Pixel& pixel = pixels[i];
            const Color& color = colors[i];

            float dx = ex - pixel.position.x;
            float dy = ey - pixel.position.y;
            float dz = ez - pixel.position.z;
            if (const float len = std::sqrt(dx*dx + dy*dy + dz*dz); len > 1e-6f) {dx /= len; dy /= len; dz /= len;}

            const float dot = pixel.normal == Point::Zero() ? 1.0f : pixel.normal.x * dx + pixel.normal.y * dy + pixel.normal.z * dz + 0.25f;
            const float brightness = std::clamp(dot, 0.25f, 1.0f);

            glColor3f(color.r / 255.0f * brightness, color.g / 255.0f * brightness, color.b / 255.0f * brightness);
            glVertex3f(pixel.position.x, pixel.position.y, pixel.position.z);
        }

        glEnd();
    }
}

