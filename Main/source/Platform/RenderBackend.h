#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// RenderBackend.h
// Public interface to the GLES3 render backend.
// Called from android_main.cpp to initialize and per-frame.
// ─────────────────────────────────────────────────────────────────────────────

namespace RenderBackend
{
    // Initialize shader programs and VBOs. Call after EGL context is ready.
    bool Init(int screenWidth, int screenHeight);
    void Shutdown();

    // Called at the beginning of each frame (sets viewport, clears).
    void BeginFrame();

    // Called at the end of each frame (flushes pending draws).
    void EndFrame();

    void SetScreenSize(int w, int h);
}

#endif
