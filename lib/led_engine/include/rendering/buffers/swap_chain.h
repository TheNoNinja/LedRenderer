#pragma once

#include "frame_buffer.h"
template<int SIZE>
class SwapChain {
public:
    FrameBuffer<SIZE>& getFrontBuffer() { return mFrameBuffers[mCurrentBuffer]; }
    const FrameBuffer<SIZE>& getFrontBuffer() const { return mFrameBuffers[mCurrentBuffer]; }
    FrameBuffer<SIZE>& getBackBuffer() { return mFrameBuffers[1 - mCurrentBuffer]; }
    const FrameBuffer<SIZE>& getBackBuffer() const { return mFrameBuffers[1 - mCurrentBuffer]; }

    void swapBuffers() { mCurrentBuffer = 1 - mCurrentBuffer; }

private:
    FrameBuffer<SIZE> mFrameBuffers[2];
    int mCurrentBuffer = 0;
};