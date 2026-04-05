
#include "rendering/camera.h"

#include <algorithm>
#include <cmath>

#define ORBIT_SPEED 0.3f
#define ZOOM_SPEED 0.15f

namespace Emulator {
    Camera::Camera(const float fovDegrees, const float aspectRatio, const float nearPlane, const float farPlane, const float radius)
        : mFieldOfView(fovDegrees), mAspectRatio(aspectRatio), mNearPlane(nearPlane), mFarPlane(farPlane), mRadius(radius), mYaw(20.0f), mPitch(25.0f) {}

    void Camera::orbit(const float deltaYaw, const float deltaPitch) {
        mYaw += deltaYaw;
        mPitch += deltaPitch;
        mPitch = std::clamp(mPitch, -89.0f, 89.0f);
    }

    void Camera::zoom(const float delta) {
        mRadius -= delta;
        mRadius = std::max(mRadius, 0.1f);
    }

    void Camera::setAspectRatio(const float aspectRatio) {
        mAspectRatio = aspectRatio;
    }

    void Camera::setFieldOfView(const float fovDegrees) {
        mFieldOfView = fovDegrees;
    }

    void Camera::setTarget(const float x, const float y, const float z) {
        mTargetX = x;
        mTargetY = y;
        mTargetZ = z;
    }

    void Camera::onEvent(const SDL_Event &event) {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            mIsMouseDragging = true;
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) {
            mIsMouseDragging = false;
        } else if (event.type == SDL_EVENT_MOUSE_MOTION && mIsMouseDragging) {
            orbit(-event.motion.xrel * ORBIT_SPEED, event.motion.yrel * ORBIT_SPEED);
        } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            zoom(event.wheel.y * ZOOM_SPEED * mRadius);
        }
    }

    void Camera::getEyePosition(float&x, float&y, float&z) const {
        const float pitch = mPitch * (M_PI / 180.0f);
        const float yaw   = mYaw   * (M_PI / 180.0f);

        x = mTargetX + mRadius * std::cos(pitch) * std::sin(yaw);
        y = mTargetY + mRadius * std::sin(pitch);
        z = mTargetZ + mRadius * std::cos(pitch) * std::cos(yaw);
    }

    Mat4 Camera::getViewMatrix() const {
        float ex, ey, ez;
        getEyePosition(ex, ey, ez);

        float fx = mTargetX - ex;
        float fy = mTargetY - ey;
        float fz = mTargetZ - ez;
        const float flen = std::sqrt(fx*fx + fy*fy + fz*fz);
        fx /= flen; fy /= flen; fz /= flen;

        constexpr float worldUpX = 0.0f, worldUpY = -1.0f, worldUpZ = 0.0f;
        float rx = worldUpY * fz - worldUpZ * fy;
        float ry = worldUpZ * fx - worldUpX * fz;
        float rz = worldUpX * fy - worldUpY * fx;
        const float rlen = std::sqrt(rx*rx + ry*ry + rz*rz);
        rx /= rlen; ry /= rlen; rz /= rlen;

        const float ux = ry * fz - rz * fy;
        const float uy = rz * fx - rx * fz;
        const float uz = rx * fy - ry * fx;

        return {
            rx, ux, -fx, 0.0f,
            ry, uy, -fy, 0.0f,
            rz, uz, -fz, 0.0f,
            -(rx*ex + ry*ey + rz*ez), -(ux*ex + uy*ey + uz*ez), (fx*ex + fy*ey + fz*ez), 1.0f
        };
    }

    Mat4 Camera::getProjectionMatrix() const {
        const float tanHalfFov = std::tan(mFieldOfView * (M_PI / 180.0f) / 2.0f);
        const float range = mFarPlane - mNearPlane;
        return {
            1.0f / (mAspectRatio * tanHalfFov), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
            0.0f, 0.0f, -(mFarPlane + mNearPlane) / range, -1.0f,
            0.0f, 0.0f, -(2.0f * mFarPlane * mNearPlane) / range, 0.0f
        };
    }
}
