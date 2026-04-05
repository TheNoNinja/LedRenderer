#pragma once
#include <string>

class Effect {
public:
    virtual ~Effect() = default;
    virtual void onStart() {};
    virtual void onUpdate(float deltaTime, float elapsedTime, const Pixel* pixels, Color* buffer, size_t bufferSize) = 0;

    void setBlendMode(const BlendMode mode) { mBlendMode = mode; }
    [[nodiscard]] BlendMode getBlendMode() const { return mBlendMode; }

    void setBrightness(const float brightness) { mBrightness = brightness; }
    [[nodiscard]] float getBrightness() const { return mBrightness; }

    void setEnabled(const bool enabled) { mIsEnabled = enabled; }
    [[nodiscard]] bool isEnabled() const { return mIsEnabled; }

    virtual void onCommand(const uint8_t param, const float value) {(void)param; (void)value;}

private:
    BlendMode mBlendMode = BlendMode::Replace;
    float mBrightness = 1.0f;
    bool mIsEnabled = true;

};
