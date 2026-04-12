#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// LegacyClientRuntime.h
// Provides the global Win32 state that Winmain.cpp normally sets up:
//   g_hWnd, g_hDC, RandomTable, etc.
// On Android these are managed here instead.
// ─────────────────────────────────────────────────────────────────────────────
#include "AndroidWin32Compat.h"
#include <android/native_window.h>

// ── Globals declared in Winmain.cpp that need Android equivalents ────────────
// These are extern'd by many game files; we define them here for Android.
extern HWND    g_hWnd;    // points to ANativeWindow* cast to void*
extern HDC     g_hDC;     // unused on Android (always nullptr)
extern int     WindowSizeX;
extern int     WindowSizeY;

namespace LegacyClientRuntime
{
    // Set the native window pointer (called from android_main on APP_CMD_INIT_WINDOW)
    void SetWindow(ANativeWindow* window);

    // Fill RandomTable[65536] with pseudo-random bytes (done once at startup)
    void InitRandomTable();

    // Called on APP_CMD_DESTROY
    void Destroy();
}

#endif // __ANDROID__
