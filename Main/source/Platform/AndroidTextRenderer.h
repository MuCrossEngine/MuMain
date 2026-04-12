#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// AndroidTextRenderer.h
// Text rendering via stb_truetype (already available in ImGui dependencies).
// Replaces the GDI + GetFontData() path inside CGMFontLayer.cpp.
// Also provides the TextOut / GetTextExtentPoint32 implementations that
// Win32SecondaryStubs.h routes to.
// ─────────────────────────────────────────────────────────────────────────────
#include "AndroidWin32Compat.h"

namespace AndroidTextRenderer
{
    // Must be called once after EGL is ready.
    // Loads system fonts (NotoSans / Roboto) from /system/fonts/.
    bool Init();
    void Shutdown();

    // ── Font handles ─────────────────────────────────────────────────────────
    // Returns an opaque handle (cast to HFONT) for a given size/weight.
    HFONT CreateFont(int heightPx, bool bold);

    // ── GDI-compatible text output ───────────────────────────────────────────
    // Renders text into the pixel buffer associated with hdc.
    // The DC's HBITMAP is the target; pixels are written as BGRA.
    void TextOut(HDC hdc, int x, int y, const char* str, int len);
    void TextOutW(HDC hdc, int x, int y, const wchar_t* str, int len);

    // Measures rendered text extent.
    bool GetTextExtentPoint32(HDC hdc, const char* str, int len, SIZE* sz);
    bool GetTextExtentPoint32W(HDC hdc, const wchar_t* str, int len, SIZE* sz);

    // ── Direct GL upload ─────────────────────────────────────────────────────
    // Upload the DC bitmap as a GL texture and return the texture ID.
    // Called by GlobalBitmap / ZzzTexture after text has been rendered.
    unsigned int UploadTextBitmap(HDC hdc, int width, int height);

    // ── DC management ────────────────────────────────────────────────────────
    // Each DC has an associated bitmap buffer + current font.
    void  SetDCFont(HDC hdc, HFONT font);
    void  SetDCBitmap(HDC hdc, HBITMAP hbm, int width, int height, void* bits);
    void* GetDCBits(HDC hdc);
}

#endif // __ANDROID__
