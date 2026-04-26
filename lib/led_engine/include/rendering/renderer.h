#pragma once

#include "core/pixel.h"
#include "core/color.h"
#include "effects/effect.h"
#include "buffers/frame_buffer.h"
#include "buffers/swap_chain.h"

template<int PIXEL_COUNT, std::size_t MAX_EFFECTS = 8>
class Renderer {
public:
    explicit Renderer(const Pixel* pixels, const float targetFps) : mPixels(pixels) {
        setTargetFps(targetFps);
    }

    void setTargetFps(const float fps) {
        mTargetFps = fps;
        mFixedDeltaTime = fps > 0.0f ? 1.0f / fps : 0.0f;
    }

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

    float getTargetFps() const { return mTargetFps; }
    float getFixedDeltaTime() const { return mFixedDeltaTime; }

    int tick(float now) {
        if (!mInitialized) {
            mLastTickTime = now;
            mInitialized = true;
        }

        if (mFixedDeltaTime <= 0.0f) {
            const float deltaTime = now - mLastTickTime;
            mLastTickTime = now;
            render(deltaTime, mElapsedTime);
            mElapsedTime += deltaTime;
            return 1;
        }

        if (mIsPaused) {
            mLastTickTime = now;
            return 0;
        }

        mAccumulator += now - mLastTickTime;
        mLastTickTime = now;

        const float maxAccumulation = mFixedDeltaTime * 5.0f;
        if (mAccumulator > maxAccumulation) mAccumulator = maxAccumulation;

        int framesProduced = 0;
        while (mAccumulator >= mFixedDeltaTime) {
            render(mFixedDeltaTime, mElapsedTime);
            mElapsedTime += mFixedDeltaTime;
            mAccumulator -= mFixedDeltaTime;
            ++framesProduced;
        }
        return framesProduced;
    }

    void step() {
        const float deltaTime = mFixedDeltaTime > 0.0f ? mFixedDeltaTime : 1.0f / 60.0f;
        render(deltaTime, mElapsedTime);
        mElapsedTime += deltaTime;
    }

    void setPaused(bool paused) {
        mIsPaused = paused;
        if (!paused) mAccumulator = 0.0f;
    }
    bool isPaused() const { return mIsPaused; }

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

    float mTargetFps;
    float mFixedDeltaTime;
    float mAccumulator = 0.0f;
    float mElapsedTime = 0.0f;
    float mLastTickTime = 0.0f;
    bool mInitialized = false;
    bool mIsPaused = false;

    SwapChain<PIXEL_COUNT> mSwapChain;
    Color mScratchBuffer[PIXEL_COUNT];

    Effect* mEffects[MAX_EFFECTS] = {};
    std::size_t mNumberOfEffects = 0;
};
