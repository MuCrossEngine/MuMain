#ifdef __ANDROID__
#include "LegacyClientRuntime.h"
#include <stdlib.h>
#include <time.h>
#include <android/log.h>

#define LOG_TAG "MUAndroid"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ─────────────────────────────────────────────────────────────────────────────
// Definitions of globals that are normally set by WinMain / WINHANDLE
// These are externed throughout the game code.
// ─────────────────────────────────────────────────────────────────────────────
HWND g_hWnd     = nullptr;
HDC  g_hDC      = nullptr;
HGLRC g_hRC     = nullptr;
HFONT g_hFont   = nullptr;
HFONT g_hFontBold = nullptr;
HFONT g_hFontBig  = nullptr;
HFONT g_hFixFont  = nullptr;
bool  Destroy   = false;
bool  ashies    = false;
int   weather   = 0;
int   Time_Effect = 0;
char  m_ID[6][11] = {{0}};
char  m_Psz[6][11] = {{0}};
char  m_Version[11] = {0};
int   m_SaveAccount = 0;
int   m_SoundOnOff = 1;
int   m_MusicOnOff = 1;
int   m_Resolution = 0;
int   m_nColorDepth = 32;
int  WindowSizeX = 1280;
int  WindowSizeY = 720;

// RandomTable is used pervasively for game randomness.
// On Windows it is filled in WinMain with a specific algorithm.
int RandomTable[100] = {0};

namespace LegacyClientRuntime
{
    void SetWindow(ANativeWindow* window)
    {
        g_hWnd = (HWND)(void*)window;
        if (window)
        {
            WindowSizeX = ANativeWindow_getWidth(window);
            WindowSizeY = ANativeWindow_getHeight(window);
            LOGI("SetWindow: %dx%d", WindowSizeX, WindowSizeY);
        }
    }

    void InitRandomTable()
    {
        // Replicate Winmain.cpp behavior.
        srand((unsigned)time(nullptr));
        for (int i = 0; i < 100; i++)
            RandomTable[i] = rand() % 360;
        LOGI("RandomTable initialized");
    }

    void Destroy()
    {
        g_hWnd = nullptr;
        LOGI("LegacyClientRuntime destroyed");
    }
}

#endif // __ANDROID__
