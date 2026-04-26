#pragma once
#include <glad/glad.h>

namespace Emulator {
    class FrameBuffer {
    public:
        FrameBuffer(int width, int height);

        bool create(int width, int height);
        void resize(int width, int height);
        [[nodiscard]] int getWidth() const { return mWidth; }
        [[nodiscard]] int getHeight() const { return mHeight; }

        void bind() const;
        static void unbind();
        [[nodiscard]] GLuint getTextureId() const { return mTextureId; }
        void destroy() const;

    private:
        GLuint mFrameBufferId;
        GLuint mTextureId;
        GLuint mDepthBufferId;
        int mWidth;
        int mHeight;
    };

}
