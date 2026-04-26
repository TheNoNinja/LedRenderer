#include "rendering/framebuffer.h"

#include <glad/glad.h>

namespace Emulator {
    FrameBuffer::FrameBuffer(const int width, const int height) : mWidth(width), mHeight(height), mFrameBufferId(0), mTextureId(0), mDepthBufferId(0) {
        create(width, height);
    }

    bool FrameBuffer::create(const int width, const int height) {
        glGenFramebuffers(1, &mFrameBufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);

        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureId, 0);

        glGenRenderbuffers(1, &mDepthBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBufferId);
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    void FrameBuffer::resize(const int width, const int height) {
        destroy();
        create(width, height);
    }

    void FrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void FrameBuffer::destroy() const {
        glDeleteFramebuffers(1, &mFrameBufferId);
        glDeleteTextures(1, &mTextureId);
        glDeleteRenderbuffers(1, &mDepthBufferId);
    }

}

