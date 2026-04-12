#ifdef __ANDROID__
#include "AndroidEglWindow.h"
#include <android/log.h>

#define LOG_TAG "MURender"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

AndroidEglWindow::AndroidEglWindow(ANativeWindow* window)
    : m_window(window)
{}

AndroidEglWindow::~AndroidEglWindow()
{
    Destroy();
}

bool AndroidEglWindow::Create()
{
    // 1. Get display
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_display == EGL_NO_DISPLAY)
    {
        LOGE("eglGetDisplay failed");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(m_display, &major, &minor))
    {
        LOGE("eglInitialize failed");
        return false;
    }
    LOGI("EGL %d.%d initialized", major, minor);

    // 2. Choose config — RGBA8888, depth 24, OpenGL ES 3.0
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_DEPTH_SIZE,      24,
        EGL_STENCIL_SIZE,    8,
        EGL_NONE
    };

    EGLint numConfigs;
    if (!eglChooseConfig(m_display, attribs, &m_config, 1, &numConfigs) || numConfigs < 1)
    {
        LOGE("eglChooseConfig failed");
        return false;
    }

    // 3. Set native window buffer format to match EGL config
    EGLint format;
    eglGetConfigAttrib(m_display, m_config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(m_window, 0, 0, format);

    // 4. Create window surface
    m_surface = eglCreateWindowSurface(m_display, m_config, m_window, nullptr);
    if (m_surface == EGL_NO_SURFACE)
    {
        LOGE("eglCreateWindowSurface failed: 0x%x", eglGetError());
        return false;
    }

    // 5. Create context — request OpenGL ES 3.0
    const EGLint ctxAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, ctxAttribs);
    if (m_context == EGL_NO_CONTEXT)
    {
        LOGE("eglCreateContext failed: 0x%x", eglGetError());
        return false;
    }

    // 6. Make current
    if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context))
    {
        LOGE("eglMakeCurrent failed: 0x%x", eglGetError());
        return false;
    }

    // 7. Query actual surface dimensions
    eglQuerySurface(m_display, m_surface, EGL_WIDTH,  &m_width);
    eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_height);
    LOGI("EGL surface: %dx%d", m_width, m_height);

    // 8. Enable vsync (1 = 60fps cap)
    eglSwapInterval(m_display, 1);

    return true;
}

void AndroidEglWindow::Destroy()
{
    if (m_display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_context != EGL_NO_CONTEXT) eglDestroyContext(m_display, m_context);
        if (m_surface != EGL_NO_SURFACE) eglDestroySurface(m_display, m_surface);
        eglTerminate(m_display);
    }
    m_display = EGL_NO_DISPLAY;
    m_context = EGL_NO_CONTEXT;
    m_surface = EGL_NO_SURFACE;
}

void AndroidEglWindow::SwapBuffers()
{
    if (m_display != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE)
        eglSwapBuffers(m_display, m_surface);
}

#endif // __ANDROID__
