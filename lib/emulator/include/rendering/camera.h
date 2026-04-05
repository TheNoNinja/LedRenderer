#pragma once

#include <SDL3/SDL_events.h>
#include <array>

namespace Emulator {
    using Mat4 = std::array<float, 16>;

    class Camera {
    public:
        explicit Camera(float fovDegrees = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f, float radius = 5.0f);

        void orbit(float deltaYaw, float deltaPitch);
        void zoom(float delta);
        void setAspectRatio(float aspectRatio);
        void setFieldOfView(float fovDegrees);
        void setTarget(float x, float y, float z);
        void onEvent(const SDL_Event& event);

        void getEyePosition(float&x, float&y, float&z) const;
        [[nodiscard]] Mat4 getViewMatrix() const;
        [[nodiscard]] Mat4 getProjectionMatrix() const;

    private:
        float mFieldOfView;
        float mAspectRatio;
        float mNearPlane;
        float mFarPlane;

        float mRadius;
        float mYaw;
        float mPitch;

        float mTargetX = 0.0f;
        float mTargetY = 0.0f;
        float mTargetZ = 0.0f;

        bool mIsMouseDragging = false;
    };
}