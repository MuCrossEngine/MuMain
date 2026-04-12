#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// RenderBackend.cpp
// Thin facade over OpenGLESRenderBackend that exposes the public RenderBackend
// interface declared in RenderBackend.h.
// Called from android_main.cpp; game code never calls this directly.
// ─────────────────────────────────────────────────────────────────────────────

#include "RenderBackend.h"
#include "OpenGLESRenderBackend.h"

namespace RenderBackend
{

bool Init(int screenWidth, int screenHeight)
{
    return GLESFF::Init(screenWidth, screenHeight);
}

void Shutdown()
{
    GLESFF::Shutdown();
}

void BeginFrame()
{
    GLESFF::BeginFrame();
}

void EndFrame()
{
    GLESFF::EndFrame();
}

void SetScreenSize(int w, int h)
{
    GLESFF::SetScreenSize(w, h);
}

} // namespace RenderBackend

#endif // __ANDROID__
