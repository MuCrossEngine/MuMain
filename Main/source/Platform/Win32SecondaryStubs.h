#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// Win32SecondaryStubs.h
// Stubs for secondary GDI/Win32 functions used in the UI and font systems.
// TextOut and GetTextExtentPoint32W are routed to AndroidTextRenderer.
// ─────────────────────────────────────────────────────────────────────────────
#include "AndroidWin32Compat.h"

// Forward — implemented in AndroidTextRenderer.cpp
namespace AndroidTextRenderer {
    void TextOut(HDC hdc, int x, int y, const char* str, int len);
    void TextOutW(HDC hdc, int x, int y, const wchar_t* str, int len);
    bool GetTextExtentPoint32(HDC hdc, const char* str, int len, SIZE* sz);
    bool GetTextExtentPoint32W(HDC hdc, const wchar_t* str, int len, SIZE* sz);
}

// ── DIB / bitmap stubs ───────────────────────────────────────────────────────
struct DIBSECTION { BITMAPINFOHEADER dsBm; };

// Creates an off-screen RGBA bitmap; ppvBits receives the pixel buffer pointer.
// On Android this allocates a plain byte array (uploaded to GL texture later).
inline HBITMAP CreateDIBSection(HDC, const BITMAPINFO* pbmi, UINT,
                                  void** ppvBits, HANDLE, DWORD)
{
    if (!pbmi || !ppvBits) return nullptr;
    int w = pbmi->bmiHeader.biWidth;
    int h = abs((int)pbmi->bmiHeader.biHeight);
    int bpp = pbmi->bmiHeader.biBitCount / 8;
    size_t sz = (size_t)w * h * bpp;
    *ppvBits = calloc(1, sz);
    // Return the buffer pointer itself as the HBITMAP handle
    return (HBITMAP)*ppvBits;
}

// DeleteObject for a DIB frees the pixel buffer
// (overrides the no-op in AndroidWin32Compat.h when called via Win32SecondaryStubs)
inline BOOL DeleteDIBSection(HBITMAP hbm)
{
    if (hbm) free(hbm);
    return TRUE;
}

// BitBlt stub (DIBs are uploaded to GL directly; BitBlt is not needed)
inline BOOL BitBlt(HDC, int, int, int, int, HDC, int, int, DWORD) { return TRUE; }
inline BOOL StretchBlt(HDC, int, int, int, int, HDC, int, int, int, int, DWORD) { return TRUE; }
inline BOOL PatBlt(HDC, int, int, int, int, DWORD) { return TRUE; }

// ── TextOut → AndroidTextRenderer ───────────────────────────────────────────
inline BOOL TextOutA(HDC hdc, int x, int y, const char* str, int len)
{
    AndroidTextRenderer::TextOut(hdc, x, y, str, len);
    return TRUE;
}
inline BOOL TextOutW(HDC hdc, int x, int y, const wchar_t* str, int len)
{
    AndroidTextRenderer::TextOutW(hdc, x, y, str, len);
    return TRUE;
}
#define TextOut TextOutA

inline BOOL GetTextExtentPoint32A(HDC hdc, const char* str, int len, SIZE* sz)
{
    return AndroidTextRenderer::GetTextExtentPoint32(hdc, str, len, sz) ? TRUE : FALSE;
}
inline BOOL GetTextExtentPoint32W(HDC hdc, const wchar_t* str, int len, SIZE* sz)
{
    return AndroidTextRenderer::GetTextExtentPoint32W(hdc, str, len, sz) ? TRUE : FALSE;
}
#define GetTextExtentPoint32 GetTextExtentPoint32A

// ── HDC management stubs ─────────────────────────────────────────────────────
inline HDC  CreateCompatibleDC(HDC)                   { return (HDC)(void*)1; }
inline BOOL DeleteDC(HDC)                             { return TRUE; }
inline BOOL SelectObject_bitmap(HDC, HBITMAP)         { return TRUE; }
inline int  FillRect(HDC, const RECT*, HBRUSH)        { return 1; }
inline HBRUSH GetStockObject(int)                     { return (HBRUSH)(void*)1; }
inline HBRUSH CreateSolidBrush(DWORD)                 { return (HBRUSH)(void*)1; }

#endif // __ANDROID__
