#pragma once

#include "core/pixel.h"
#include "core/color.h"
#include "effects/effect.h"
#include "buffers/frame_buffer.h"
#include "buffers/swap_chain.h"

template<int PIXEL_COUNT, std::size_t MAX_EFFECTS = 8>
class Renderer {
public:
    explicit Renderer(const Pixel* pixels) : mPixels(pixels) {}

    bool addEffect(Effect* effect) {
        if (mNumberOfEffects >= MAX_EFFECTS) return false;
        mEffects[mNumberOfEffects++] = effect;
        effect->onStart();
        return true;
    }

    void removeEffect(Effect* effect) {
        for (std::size_t i = 0; i < mNumberOfEffects; ++i) {
            if (mEffects[i] == effect) {
                for (std::size_t j = i; j < mNumberOfEffects - 1; ++j) {
                    mEffects[j] = mEffects[j + 1];
                }
                mEffects[--mNumberOfEffects] = nullptr;
                return;
            }
        }
    }

    void clearEffects() {
        for (std::size_t i = 0; i < mNumberOfEffects; ++i) {
            mEffects[i] = nullptr;
        }
        mNumberOfEffects = 0;
    }

    Effect* getEffect(std::size_t index) const {
        if (index >= mNumberOfEffects) return nullptr;
        return mEffects[index];
    }

    std::size_t getNumberOfEffects() const {
        return mNumberOfEffects;
    }


    void render(float deltaTime, float elapsedTime) {
        auto& backBuffer = mSwapChain.getBackBuffer();
        backBuffer.clear();

        for (std::size_t e = 0; e < mNumberOfEffects; ++e) {
            Effect* effect = mEffects[e];
            if (!effect || !effect->isEnabled()) continue;
            for (int i = 0; i < PIXEL_COUNT; ++i) {
                mScratchBuffer[i] = Color::Black();
            }

            effect->onUpdate(deltaTime, elapsedTime, mPixels, mScratchBuffer, PIXEL_COUNT);

            const float brightness = effect->getBrightness();
            const BlendMode blendMode = effect->getBlendMode();

            for (int i = 0; i < PIXEL_COUNT; ++i) {
                Color effectColor = mScratchBuffer[i];
                if (brightness < 1.0f) {
                    effectColor.applyBlend(BlendMode::Alpha, Color(255, 255, 255, static_cast<uint8_t>(255 * brightness)));
                }
                backBuffer.blendPixel(i, effectColor, blendMode);
            }
        }

        mSwapChain.swapBuffers();

    }

    const FrameBuffer<PIXEL_COUNT>& getFrontBuffer() const {
        return mSwapChain.getFrontBuffer();
    }
    void copyFrontBuffer(Color* colorOutput) const {
        auto& frontBuffer = mSwapChain.getFrontBuffer();
        for (int i = 0; i < PIXEL_COUNT; ++i) {
            frontBuffer.getPixel(i, colorOutput[i]);
        }
    }

    void sendCommand(std::size_t index, uint8_t param, float value) {
        if (index < mNumberOfEffects && mEffects[index]) {
            mEffects[index]->onCommand(param, value);
        }
    }

private:
    const Pixel* mPixels = nullptr;

    SwapChain<PIXEL_COUNT> mSwapChain;
    Color mScratchBuffer[PIXEL_COUNT];

    Effect* mEffects[MAX_EFFECTS] = {};
    std::size_t mNumberOfEffects = 0;
};
