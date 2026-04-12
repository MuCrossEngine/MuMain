#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// AndroidEglWindow.h
// Manages the EGL context (display, surface, context) for OpenGL ES 3.0.
// Replaces the PIXELFORMATDESCRIPTOR + wglCreateContext sequence in Winmain.cpp.
// ─────────────────────────────────────────────────────────────────────────────
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/native_window.h>

class AndroidEglWindow
{
public:
    explicit AndroidEglWindow(ANativeWindow* window);
    ~AndroidEglWindow();

    // Create EGL display, config, surface and context. Returns false on failure.
    bool Create();

    // Destroy surface and context (called on APP_CMD_TERM_WINDOW).
    void Destroy();

    // Swap front/back buffers — equivalent to SwapBuffers(hDC) on Windows.
    void SwapBuffers();

    // Query drawable size (may differ from window size on high-DPI screens).
    int  GetWidth()  const { return m_width; }
    int  GetHeight() const { return m_height; }

    bool IsValid() const { return m_surface != EGL_NO_SURFACE; }

private:
    ANativeWindow* m_window  = nullptr;
    EGLDisplay     m_display = EGL_NO_DISPLAY;
    EGLContext     m_context = EGL_NO_CONTEXT;
    EGLSurface     m_surface = EGL_NO_SURFACE;
    EGLConfig      m_config  = nullptr;
    int            m_width   = 0;
    int            m_height  = 0;
};

#endif // __ANDROID__
