#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// AndroidWin32Compat.h
// Shims all Win32 types, macros and inline functions used throughout the game
// so that existing source files compile on Android without modification.
// Included via StdAfx.h when __ANDROID__ is defined.
// ─────────────────────────────────────────────────────────────────────────────
#ifndef __ANDROID__
#  error "This file is Android-only. Include windows.h on Windows."
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <ctype.h>
#include <android/log.h>

// ─────────────────────────────────────────────────────────────────────────────
// Basic types
// ─────────────────────────────────────────────────────────────────────────────
typedef unsigned char       BYTE;
typedef unsigned char       UCHAR;
typedef unsigned short      WORD;
typedef short               SHORT;
typedef unsigned int        DWORD;
typedef unsigned long long  QWORD;
typedef int                 BOOL;
typedef int                 INT;
typedef unsigned int        UINT;
typedef long                LONG;
typedef unsigned long       ULONG;
typedef long long           LONGLONG;
typedef unsigned long long  ULONGLONG;
typedef long long           INT64;
typedef unsigned long long  UINT64;
#ifndef __int8
#  define __int8 char
#endif
#ifndef __int16
#  define __int16 short
#endif
#ifndef __int32
#  define __int32 int
#endif
#ifndef __int64
#  define __int64 long long
#endif
typedef float               FLOAT;
typedef char                CHAR;
typedef wchar_t             WCHAR;
typedef char                TCHAR;
typedef char*               LPSTR;
typedef const char*         LPCSTR;
typedef wchar_t*            LPWSTR;
typedef const wchar_t*      LPCWSTR;
typedef void                VOID;
typedef void*               LPVOID;
typedef const void*         LPCVOID;
typedef void*               PVOID;
typedef size_t              SIZE_T;
typedef size_t*             PSIZE_T;
typedef intptr_t            INT_PTR;
typedef uintptr_t           UINT_PTR;
typedef intptr_t            LONG_PTR;
typedef uintptr_t           ULONG_PTR;
typedef uintptr_t           WPARAM;
typedef intptr_t            LPARAM;
typedef intptr_t            LRESULT;
typedef const TCHAR*        LPCTSTR;
typedef TCHAR*              LPTSTR;
typedef void*               FARPROC;
typedef DWORD*              LPDWORD;
typedef BYTE*               LPBYTE;

#ifndef IN
#  define IN
#endif
#ifndef OUT
#  define OUT
#endif

#ifndef WINDOWMODE
#  define WINDOWMODE
#endif

#ifndef CONST
#  define CONST const
#endif
#ifndef TEXT
#  define TEXT(x) x
#endif
#ifndef _T
#  define _T(x) x
#endif
#ifndef _tprintf
#  define _tprintf printf
#endif

#ifndef _ASSERT
#  define _ASSERT(expr) assert((expr))
#endif

// ─────────────────────────────────────────────────────────────────────────────
// Handle types (opaque pointers)
// ─────────────────────────────────────────────────────────────────────────────
typedef void*  HWND;
typedef void*  HINSTANCE;
typedef void*  HMODULE;
typedef void*  HDC;
typedef void*  HGLRC;
typedef void*  HFONT;
typedef void*  HBITMAP;
typedef void*  HBRUSH;
typedef void*  HPEN;
typedef void*  HICON;
typedef void*  HCURSOR;
typedef void*  HMENU;
typedef void*  HANDLE;
typedef void*  HKEY;
typedef void*  HGDIOBJ;
typedef void*  HRGN;
typedef void*  HPALETTE;
typedef void*  HHOOK;
typedef void*  HINTERNET;
typedef void*  HIMC;
typedef void*  HGLOBAL;
typedef HANDLE HTHREAD;
typedef HANDLE HEVENT;

typedef unsigned short INTERNET_PORT;
#ifndef INTERNET_DEFAULT_FTP_PORT
#  define INTERNET_DEFAULT_FTP_PORT 21
#endif
#ifndef INTERNET_MAX_URL_LENGTH
#  define INTERNET_MAX_URL_LENGTH 2084
#endif
#ifndef INTERNET_MAX_USER_NAME_LENGTH
#  define INTERNET_MAX_USER_NAME_LENGTH 128
#endif
#ifndef INTERNET_MAX_PASSWORD_LENGTH
#  define INTERNET_MAX_PASSWORD_LENGTH 128
#endif

#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#define NULL_HANDLE          nullptr

// ─────────────────────────────────────────────────────────────────────────────
// Boolean
// ─────────────────────────────────────────────────────────────────────────────
#ifndef TRUE
#  define TRUE  1
#endif
#ifndef FALSE
#  define FALSE 0
#endif
#ifndef NULL
#  define NULL  nullptr
#endif

// ─────────────────────────────────────────────────────────────────────────────
// Numeric limits
// ─────────────────────────────────────────────────────────────────────────────
#ifndef MAX_PATH
#  define MAX_PATH 260
#endif
#ifndef _MAX_EXT
#  define _MAX_EXT 256
#endif
#ifndef _MAX_DRIVE
#  define _MAX_DRIVE 3
#endif
#ifndef _MAX_DIR
#  define _MAX_DIR 256
#endif
#ifndef _MAX_FNAME
#  define _MAX_FNAME 256
#endif
#ifndef MAX_ACCOUNT_REG
#  define MAX_ACCOUNT_REG 5
#endif
#ifndef CP_UTF8
#  define CP_UTF8 65001
#endif
#define MAXBYTE   0xFF
#define MAXWORD   0xFFFF
#define MAXDWORD  0xFFFFFFFF
#ifndef RGB
#  define RGB(r,g,b) ((DWORD)(((BYTE)(r)) | (((WORD)(BYTE)(g))<<8) | (((DWORD)(BYTE)(b))<<16)))
#endif

// ─────────────────────────────────────────────────────────────────────────────
// RECT / POINT / SIZE structs
// ─────────────────────────────────────────────────────────────────────────────
typedef struct tagRECT { LONG left, top, right, bottom; } RECT, *PRECT, *LPRECT;
typedef struct tagPOINT { LONG x, y; } POINT, *PPOINT, *LPPOINT;
typedef struct tagSIZE  { LONG cx, cy; } SIZE, *PSIZE, *LPSIZE;
typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

inline BOOL SetRect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom)
{
    if (!lprc) return FALSE;
    lprc->left = xLeft;
    lprc->top = yTop;
    lprc->right = xRight;
    lprc->bottom = yBottom;
    return TRUE;
}

inline BOOL PtInRect(const RECT* lprc, POINT pt)
{
    if (!lprc) return FALSE;
    return (pt.x >= lprc->left && pt.x < lprc->right &&
            pt.y >= lprc->top && pt.y < lprc->bottom) ? TRUE : FALSE;
}

inline BOOL IntersectRect(LPRECT lprcDst, const RECT* lprcSrc1, const RECT* lprcSrc2)
{
    if (!lprcDst || !lprcSrc1 || !lprcSrc2) return FALSE;
    lprcDst->left   = (lprcSrc1->left   > lprcSrc2->left)   ? lprcSrc1->left   : lprcSrc2->left;
    lprcDst->top    = (lprcSrc1->top    > lprcSrc2->top)    ? lprcSrc1->top    : lprcSrc2->top;
    lprcDst->right  = (lprcSrc1->right  < lprcSrc2->right)  ? lprcSrc1->right  : lprcSrc2->right;
    lprcDst->bottom = (lprcSrc1->bottom < lprcSrc2->bottom) ? lprcSrc1->bottom : lprcSrc2->bottom;
    return (lprcDst->right > lprcDst->left && lprcDst->bottom > lprcDst->top) ? TRUE : FALSE;
}

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _RTL_OSVERSIONINFOW {
    ULONG dwOSVersionInfoSize;
    ULONG dwMajorVersion;
    ULONG dwMinorVersion;
    ULONG dwBuildNumber;
    ULONG dwPlatformId;
    WCHAR szCSDVersion[128];
} RTL_OSVERSIONINFOW, *PRTL_OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW;
typedef RTL_OSVERSIONINFOW OSVERSIONINFOW;

// WinMain globals frequently referenced by legacy UI/gameplay code.
extern HDC   g_hDC;
extern HGLRC g_hRC;
extern HFONT g_hFont;
extern HFONT g_hFontBold;
extern HFONT g_hFontBig;
extern HFONT g_hFixFont;
extern bool  Destroy;
extern bool  ashies;
extern int   weather;
extern int   Time_Effect;
extern int   RandomTable[];

// Forward declarations for ANSI text APIs used by _types.h.
// Implementations are provided in Platform/Win32SecondaryStubs.h.
inline BOOL GetTextExtentPoint32A(HDC hdc, const char* str, int len, SIZE* sz);
inline BOOL TextOutA(HDC hdc, int x, int y, const char* str, int len);
inline BOOL GetTextExtentPointA(HDC hdc, const char* str, int len, SIZE* sz)
{
    return GetTextExtentPoint32A(hdc, str, len, sz);
}

inline char* itoa(int value, char* buffer, int radix)
{
    if (!buffer) return nullptr;

    if (radix == 10)
    {
        snprintf(buffer, 32, "%d", value);
        return buffer;
    }
    if (radix == 16)
    {
        snprintf(buffer, 32, "%x", value);
        return buffer;
    }

    // Generic fallback for bases 2..36.
    static const char kDigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (radix < 2 || radix > 36)
    {
        buffer[0] = '\0';
        return buffer;
    }

    unsigned int n = (value < 0 && radix == 10) ? (unsigned int)(-value) : (unsigned int)value;
    char tmp[65];
    int i = 0;
    do
    {
        tmp[i++] = kDigits[n % (unsigned int)radix];
        n /= (unsigned int)radix;
    } while (n && i < (int)sizeof(tmp) - 1);

    int pos = 0;
    if (value < 0 && radix == 10)
        buffer[pos++] = '-';
    while (i > 0)
        buffer[pos++] = tmp[--i];
    buffer[pos] = '\0';
    return buffer;
}

// ─────────────────────────────────────────────────────────────────────────────
// Calling conventions (no-op on ARM)
// ─────────────────────────────────────────────────────────────────────────────
#define WINAPI
#define CALLBACK
#define APIENTRY
#define PASCAL
#define FAR
#define NEAR
#define __cdecl
#define __stdcall
#ifndef __forceinline
#  define __forceinline inline __attribute__((always_inline))
#endif

// ─────────────────────────────────────────────────────────────────────────────
// HRESULT
// ─────────────────────────────────────────────────────────────────────────────
typedef long HRESULT;
#define S_OK           ((HRESULT)0)
#define S_FALSE        ((HRESULT)1)
#define E_FAIL         ((HRESULT)0x80004005L)
#define E_NOTIMPL      ((HRESULT)0x80004001L)
#define E_INVALIDARG   ((HRESULT)0x80070057L)
#define E_OUTOFMEMORY  ((HRESULT)0x8007000EL)
#define SUCCEEDED(hr)  ((HRESULT)(hr) >= 0)
#define FAILED(hr)     ((HRESULT)(hr) < 0)
#define MAKE_HRESULT(s,f,c) ((HRESULT)(((DWORD)(s)<<31)|((DWORD)(f)<<16)|((DWORD)(c))))

#ifndef min
#  define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#  define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

// ─────────────────────────────────────────────────────────────────────────────
// Byte / bit macros
// ─────────────────────────────────────────────────────────────────────────────
#define LOBYTE(w)   ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)   ((BYTE)((DWORD_PTR)(w) >> 8))
#define LOWORD(l)   ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)   ((WORD)((DWORD_PTR)(l) >> 16))
#define MAKELONG(lo,hi) ((LONG)(((WORD)((DWORD_PTR)(lo)&0xffff))|((DWORD)((WORD)((DWORD_PTR)(hi)&0xffff)))<<16))
#define MAKEWORD(lo,hi) ((WORD)(((BYTE)(lo))|(((WORD)((BYTE)(hi)))<<8)))
#define MAKEDWORD(lo,hi) ((DWORD)(((WORD)(lo))|(((DWORD)((WORD)(hi)))<<16)))
typedef uintptr_t DWORD_PTR;

// ─────────────────────────────────────────────────────────────────────────────
// Virtual key codes (mapped to Android AKEYCODE where sensible)
// ─────────────────────────────────────────────────────────────────────────────
#define VK_LBUTTON   0x01
#define VK_RBUTTON   0x02
#define VK_MBUTTON   0x04
#define VK_BACK      0x08
#define VK_TAB       0x09
#define VK_RETURN    0x0D
#define VK_SHIFT     0x10
#define VK_CONTROL   0x11
#define VK_LCONTROL  0xA2
#define VK_RCONTROL  0xA3
#define VK_MENU      0x12   // Alt
#define VK_PAUSE     0x13
#define VK_CAPITAL   0x14   // Caps lock
#define VK_ESCAPE    0x1B
#define VK_SPACE     0x20
#define VK_PRIOR     0x21   // Page Up
#define VK_NEXT      0x22   // Page Down
#define VK_END       0x23
#define VK_HOME      0x24
#define VK_LEFT      0x25
#define VK_UP        0x26
#define VK_RIGHT     0x27
#define VK_DOWN      0x28
#define VK_SNAPSHOT  0x2C
#define VK_DELETE    0x2E
#define VK_INSERT    0x2D
#define VK_F1        0x70
#define VK_F2        0x71
#define VK_F3        0x72
#define VK_F4        0x73
#define VK_F5        0x74
#define VK_F6        0x75
#define VK_F7        0x76
#define VK_F8        0x77
#define VK_F9        0x78
#define VK_F10       0x79
#define VK_F11       0x7A
#define VK_F12       0x7B
#define VK_NUMPAD0   0x60
#define VK_NUMPAD9   0x69
#define VK_MULTIPLY  0x6A
#define VK_ADD       0x6B
#define VK_SUBTRACT  0x6D
#define VK_DECIMAL   0x6E
#define VK_DIVIDE    0x6F
#define VK_OEM_MINUS 0xBD
#define VK_OEM_PLUS  0xBB
// 0-9 and A-Z match ASCII
#define VK_0  0x30
#define VK_9  0x39
#define VK_A  0x41
#define VK_Z  0x5A

// ─────────────────────────────────────────────────────────────────────────────
// Window messages (only constants used in the game code as values)
// ─────────────────────────────────────────────────────────────────────────────
#define WM_NULL             0x0000
#define WM_KEYDOWN          0x0100
#define WM_KEYUP            0x0101
#define WM_CHAR             0x0102
#define WM_LBUTTONDOWN      0x0201
#define WM_LBUTTONUP        0x0202
#define WM_LBUTTONDBLCLK    0x0203
#define WM_RBUTTONDOWN      0x0204
#define WM_RBUTTONUP        0x0205
#define WM_MBUTTONDOWN      0x0207
#define WM_MBUTTONUP        0x0208
#define WM_MOUSEMOVE        0x0200
#define WM_MOUSEWHEEL       0x020A
#define WM_SIZE             0x0005
#define WM_CLOSE            0x0010
#define WM_DESTROY          0x0002
#define WM_QUIT             0x0012
#define WM_TIMER            0x0113
#define WM_PAINT            0x000F
#define WM_ACTIVATE         0x0006
#define WM_SETFOCUS         0x0007
#define WM_KILLFOCUS        0x0008
#define WM_USER             0x0400
#define WHEEL_DELTA         120

// ─────────────────────────────────────────────────────────────────────────────
// GDI / font stubs
// ─────────────────────────────────────────────────────────────────────────────
#define DEFAULT_CHARSET         1
#define ANSI_CHARSET            0
#define GB2312_CHARSET          134
#define HANGUL_CHARSET          129
#define NONANTIALIASED_QUALITY  3
#define ANTIALIASED_QUALITY     4
#define DEFAULT_QUALITY         0
#define FW_NORMAL               400
#define FW_BOLD                 700
#define OUT_DEFAULT_PRECIS      0
#define CLIP_DEFAULT_PRECIS     0
#define DEFAULT_PITCH           0
#define FF_DONTCARE             0
#define TRANSPARENT             1
#define OPAQUE                  2

// Font / GDI stubs — implemented in Platform/Win32SecondaryStubs.h
struct LOGFONTA {
    LONG  lfHeight;
    LONG  lfWidth;
    LONG  lfWeight;
    BYTE  lfItalic;
    BYTE  lfUnderline;
    BYTE  lfCharSet;
    BYTE  lfQuality;
    char  lfFaceName[32];
};
struct TEXTMETRICA {
    LONG tmHeight, tmAscent, tmDescent, tmExternalLeading;
    LONG tmAveCharWidth, tmMaxCharWidth;
};
struct TEXTMETRICW {
    LONG tmHeight, tmAscent, tmDescent, tmExternalLeading;
    LONG tmAveCharWidth, tmMaxCharWidth;
};

// ─────────────────────────────────────────────────────────────────────────────
// BITMAP / DIB stubs
// ─────────────────────────────────────────────────────────────────────────────
struct __attribute__((packed)) BITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
};
struct BITMAPINFOHEADER {
    DWORD biSize, biWidth; LONG biHeight;
    WORD  biPlanes, biBitCount;
    DWORD biCompression, biSizeImage;
    LONG  biXPelsPerMeter, biYPelsPerMeter;
    DWORD biClrUsed, biClrImportant;
};
struct PALETTEENTRY { BYTE peRed, peGreen, peBlue, peFlags; };
struct RGBQUAD { BYTE rgbBlue, rgbGreen, rgbRed, rgbReserved; };
struct BITMAPINFO { BITMAPINFOHEADER bmiHeader; RGBQUAD bmiColors[1]; };
#define DIB_RGB_COLORS 0
#define BI_RGB         0
#define OBJ_FONT       6

// ─────────────────────────────────────────────────────────────────────────────
// Mutex / threading stubs
// ─────────────────────────────────────────────────────────────────────────────
#include <pthread.h>
typedef pthread_mutex_t CRITICAL_SECTION;
inline void InitializeCriticalSection(CRITICAL_SECTION* cs) { pthread_mutex_init(cs, nullptr); }
inline void DeleteCriticalSection(CRITICAL_SECTION* cs)     { pthread_mutex_destroy(cs); }
inline void EnterCriticalSection(CRITICAL_SECTION* cs)      { pthread_mutex_lock(cs); }
inline void LeaveCriticalSection(CRITICAL_SECTION* cs)      { pthread_mutex_unlock(cs); }
inline BOOL TryEnterCriticalSection(CRITICAL_SECTION* cs)   { return pthread_mutex_trylock(cs) == 0; }

// ─────────────────────────────────────────────────────────────────────────────
// Timing stubs
// ─────────────────────────────────────────────────────────────────────────────
#include <time.h>
inline DWORD GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (DWORD)(ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL);
}
inline DWORD timeGetTime() { return GetTickCount(); }
inline void  timeBeginPeriod(UINT) {}
inline void  timeEndPeriod(UINT) {}
inline void  Sleep(DWORD ms)
{
    struct timespec ts{ (time_t)(ms / 1000), (long)((ms % 1000) * 1000000L) };
    nanosleep(&ts, nullptr);
}

typedef struct { LONGLONG QuadPart; } LARGE_INTEGER;
inline BOOL QueryPerformanceCounter(LARGE_INTEGER* lp)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    lp->QuadPart = (LONGLONG)ts.tv_sec * 1000000000LL + ts.tv_nsec;
    return TRUE;
}
inline BOOL QueryPerformanceFrequency(LARGE_INTEGER* lp)
{
    lp->QuadPart = 1000000000LL; // nanoseconds
    return TRUE;
}

// ─────────────────────────────────────────────────────────────────────────────
// String functions
// ─────────────────────────────────────────────────────────────────────────────
#include <string.h>
inline int lstrcmpi(const char* a, const char* b)  { return strcasecmp(a, b); }
inline int lstrcmp(const char* a, const char* b)   { return strcmp(a, b); }
inline char* lstrcpy(char* d, const char* s)       { return strcpy(d, s); }
inline char* lstrcat(char* d, const char* s)       { return strcat(d, s); }
inline int   lstrlen(const char* s)                { return (int)strlen(s); }
inline int   _mbclen(const unsigned char* s)       { return (s && *s) ? 1 : 0; }
inline HRESULT StringCchCopyA(char* dst, size_t cchDst, const char* src)
{
    if (!dst || cchDst == 0) return E_INVALIDARG;
    if (!src) { dst[0] = '\0'; return S_OK; }

    strncpy(dst, src, cchDst - 1);
    dst[cchDst - 1] = '\0';
    return S_OK;
}
inline HRESULT StringCchCopyW(wchar_t* dst, size_t cchDst, const wchar_t* src)
{
    if (!dst || cchDst == 0) return E_INVALIDARG;
    if (!src) { dst[0] = L'\0'; return S_OK; }

    wcsncpy(dst, src, cchDst - 1);
    dst[cchDst - 1] = L'\0';
    return S_OK;
}
#ifndef StringCchCopy
#  define StringCchCopy StringCchCopyA
#endif
inline char* _strupr(char* s)
{
    if (!s) return s;
    for (char* p = s; *p; ++p)
        *p = (char)toupper((unsigned char)*p);
    return s;
}
#define ZeroMemory(p,n) memset((p),0,(n))
#define FillMemory(p,n,v) memset((p),(v),(n))
#define CopyMemory(d,s,n) memcpy((d),(s),(n))
#define MoveMemory(d,s,n) memmove((d),(s),(n))
#define sscanf_s(buf, fmt, ...) sscanf((buf), (fmt), ##__VA_ARGS__)
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define stricmp  strcasecmp
#define strnicmp strncasecmp

// ─────────────────────────────────────────────────────────────────────────────
// File I/O
// ─────────────────────────────────────────────────────────────────────────────
#include <sys/stat.h>
#include <unistd.h>
#define _access access
#define _MAX_PATH MAX_PATH

inline void _splitpath(const char* path, char* drive, char* dir, char* fname, char* ext)
{
    if (drive) drive[0] = '\0';
    if (dir) dir[0] = '\0';
    if (fname) fname[0] = '\0';
    if (ext) ext[0] = '\0';
    if (!path) return;

    const char* slash1 = strrchr(path, '/');
    const char* slash2 = strrchr(path, '\\');
    const char* lastSlash = (slash1 && slash2) ? (slash1 > slash2 ? slash1 : slash2) : (slash1 ? slash1 : slash2);
    const char* lastDot = strrchr(path, '.');

    if (lastDot && (!lastSlash || lastDot > lastSlash))
    {
        if (ext) strcpy(ext, lastDot);
    }
    else
    {
        lastDot = nullptr;
    }

    const char* nameBegin = lastSlash ? (lastSlash + 1) : path;
    if (fname)
    {
        size_t nameLen = lastDot ? (size_t)(lastDot - nameBegin) : strlen(nameBegin);
        memcpy(fname, nameBegin, nameLen);
        fname[nameLen] = '\0';
    }

    if (dir && lastSlash)
    {
        size_t dirLen = (size_t)(lastSlash - path + 1);
        memcpy(dir, path, dirLen);
        dir[dirLen] = '\0';
    }
}

inline BOOL CreateDirectoryA(const char* path, void*)
{
    if (!path || !path[0]) return FALSE;
    char tmp[1024];
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    for (char* p = tmp; *p; ++p)
    {
        if (*p == '\\') *p = '/';
    }
    if (mkdir(tmp, 0755) == 0) return TRUE;
    return (errno == EEXIST) ? TRUE : FALSE;
}
inline BOOL CreateDirectory(const char* path, void* secAttr) { return CreateDirectoryA(path, secAttr); }
inline BOOL RemoveDirectoryA(const char* path)
{
    if (!path) return FALSE;
    return (rmdir(path) == 0) ? TRUE : FALSE;
}
inline BOOL RemoveDirectory(const char* path)
{
    return RemoveDirectoryA(path);
}
inline BOOL DeleteFileA(const char* path)
{
    if (!path) return FALSE;
    return (unlink(path) == 0) ? TRUE : FALSE;
}
inline BOOL DeleteFile(const char* path)
{
    return DeleteFileA(path);
}
inline BOOL SetFileAttributesA(const char* path, DWORD attrs)
{
    (void)attrs;
    if (!path) return FALSE;
    struct stat st;
    if (stat(path, &st) != 0) return FALSE;
    // Android/Linux does not expose Win32 attributes; keep existing mode.
    return TRUE;
}
inline BOOL SetFileAttributes(const char* path, DWORD attrs)
{
    return SetFileAttributesA(path, attrs);
}
inline DWORD GetCurrentDirectoryA(DWORD nBufferLength, char* lpBuffer)
{
    if (!lpBuffer || nBufferLength == 0) return 0;

    char cwd[PATH_MAX > 0 ? PATH_MAX : 4096];
    if (!getcwd(cwd, sizeof(cwd)))
    {
        lpBuffer[0] = '\0';
        return 0;
    }

    const size_t len = strlen(cwd);
    if (len + 1 > nBufferLength)
    {
        lpBuffer[0] = '\0';
        return (DWORD)(len + 1);
    }

    memcpy(lpBuffer, cwd, len + 1);
    return (DWORD)len;
}
inline DWORD GetCurrentDirectory(DWORD nBufferLength, char* lpBuffer)
{
    return GetCurrentDirectoryA(nBufferLength, lpBuffer);
}

// ─────────────────────────────────────────────────────────────────────────────
// Debug output → Android log
// ─────────────────────────────────────────────────────────────────────────────
inline void OutputDebugStringA(const char* msg)
{
    __android_log_print(ANDROID_LOG_DEBUG, "MUAndroid", "%s", msg);
}
inline void OutputDebugString(const char* msg) { OutputDebugStringA(msg); }

// ─────────────────────────────────────────────────────────────────────────────
// Window / message stubs (no-op; game logic doesn't need real Win32 here)
// ─────────────────────────────────────────────────────────────────────────────
inline BOOL PostMessage(HWND, UINT, DWORD, DWORD)    { return TRUE; }
inline BOOL SendMessage(HWND, UINT, DWORD, DWORD)    { return TRUE; }
inline BOOL SetTimer(HWND, UINT, UINT, void*)        { return TRUE; }
inline BOOL KillTimer(HWND, UINT)                    { return TRUE; }
inline BOOL ShowWindow(HWND, int)                    { return TRUE; }
inline BOOL UpdateWindow(HWND)                       { return TRUE; }
inline BOOL DestroyWindow(HWND)                      { return TRUE; }
inline BOOL InvalidateRect(HWND, const RECT*, BOOL)  { return TRUE; }
inline HWND GetFocus()                               { return nullptr; }
inline HWND SetFocus(HWND h)                         { return h; }
inline BOOL IsWindow(HWND h)                         { return h != nullptr; }
inline BOOL IsBadReadPtr(const void* p, UINT_PTR)   { return p == nullptr; }

#define GWL_WNDPROC  (-4)
#define GWL_USERDATA (-21)

inline LONG_PTR SetWindowLongPtrW(HWND, int, LONG_PTR) { return 0; }
inline LONG_PTR GetWindowLongPtrW(HWND, int) { return 0; }
inline LONG SetWindowLongW(HWND hWnd, int index, LONG value)
{
    return (LONG)SetWindowLongPtrW(hWnd, index, (LONG_PTR)value);
}
inline LONG GetWindowLongW(HWND hWnd, int index)
{
    return (LONG)GetWindowLongPtrW(hWnd, index);
}
inline LRESULT CallWindowProcW(WNDPROC, HWND, UINT, WPARAM, LPARAM) { return 0; }

#define CF_TEXT 1
inline BOOL OpenClipboard(HWND) { return TRUE; }
inline BOOL CloseClipboard() { return TRUE; }
inline HANDLE GetClipboardData(UINT) { return nullptr; }
inline LPVOID GlobalLock(HGLOBAL hMem) { return hMem; }
inline BOOL GlobalUnlock(HGLOBAL) { return TRUE; }

typedef struct _STARTUPINFOA {
    DWORD cb;
    char* lpReserved;
    char* lpDesktop;
    char* lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD  wShowWindow;
    WORD  cbReserved2;
    BYTE* lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFO, STARTUPINFOA, *LPSTARTUPINFOA;

typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD  dwProcessId;
    DWORD  dwThreadId;
} PROCESS_INFORMATION, *LPPROCESS_INFORMATION;

inline BOOL CreateProcessA(
    const char*,
    char*,
    void*,
    void*,
    BOOL,
    DWORD,
    void*,
    const char*,
    STARTUPINFOA*,
    PROCESS_INFORMATION*)
{
    errno = ENOSYS;
    return FALSE;
}
inline BOOL CreateProcess(
    const char* appName,
    char* cmdLine,
    void* procAttr,
    void* threadAttr,
    BOOL inheritHandles,
    DWORD creationFlags,
    void* env,
    const char* currentDir,
    STARTUPINFOA* startupInfo,
    PROCESS_INFORMATION* processInfo)
{
    return CreateProcessA(appName, cmdLine, procAttr, threadAttr, inheritHandles, creationFlags, env, currentDir, startupInfo, processInfo);
}

inline int  MessageBoxA(HWND, const char* text, const char* cap, UINT) {
    __android_log_print(ANDROID_LOG_WARN, "MUAndroid", "MessageBox: [%s] %s", cap ? cap : "", text ? text : "");
    return 1; // IDOK
}
inline int  MessageBox(HWND h, const char* t, const char* c, UINT f) { return MessageBoxA(h, t, c, f); }
#define MB_OK        0
#define MB_YESNO     4
#define MB_ICONERROR 0x10
#define MB_ICONINFO  0x40
#define IDOK         1
#define IDYES        6
#define IDNO         7
#define SW_SHOW      5
#define ERROR_ALREADY_EXISTS 183
#ifndef MCI_SEQ_MAPPER
#  define MCI_SEQ_MAPPER 0xFFFFFFFFu
#endif
#define WM_IME_CONTROL            0x0283
#define IMC_SETCOMPOSITIONWINDOW  0x000C
#define CFS_DEFAULT               0x0000
#define CFS_RECT                  0x0001
#define CFS_POINT                 0x0002
#define IME_CMODE_NATIVE 0x0001
#define IME_CMODE_ALPHANUMERIC 0x0000
#define IME_SMODE_NONE   0x0000
#define IME_SMODE_AUTOMATIC 0x0008

typedef struct tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *LPCOMPOSITIONFORM;

inline HIMC ImmGetContext(HWND) { return nullptr; }
inline HWND ImmGetDefaultIMEWnd(HWND hWnd) { return hWnd; }
inline BOOL ImmReleaseContext(HWND, HIMC) { return TRUE; }
inline BOOL ImmGetConversionStatus(HIMC, DWORD* lpfdwConversion, DWORD* lpfdwSentence)
{
    if (lpfdwConversion) *lpfdwConversion = 0;
    if (lpfdwSentence) *lpfdwSentence = 0;
    return TRUE;
}
inline BOOL ImmSetConversionStatus(HIMC, DWORD, DWORD) { return TRUE; }

// ─────────────────────────────────────────────────────────────────────────────
// GDI object stubs
// ─────────────────────────────────────────────────────────────────────────────
inline HFONT   CreateFontA(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,const char*) { return (HFONT)(void*)1; }
inline HFONT   CreateFont(int h,int w,int e,int o,int wt,DWORD i,DWORD u,DWORD so,DWORD cs,DWORD op,DWORD cp,DWORD q,DWORD p,const char* f)
               { return CreateFontA(h,w,e,o,wt,i,u,so,cs,op,cp,q,p,f); }
inline BOOL    DeleteObject(HGDIOBJ) { return TRUE; }
inline HGDIOBJ SelectObject(HDC, HGDIOBJ o) { return o; }
inline BOOL    GetTextMetricsW(HDC, TEXTMETRICW* tm) { if(tm) memset(tm,0,sizeof(*tm)); return TRUE; }
inline BOOL    GetTextMetrics(HDC h, TEXTMETRICW* tm) { return GetTextMetricsW(h, tm); }
inline DWORD   GetFontData(HDC, DWORD, DWORD, void*, DWORD) { return 0; }
inline int     SetBkMode(HDC, int) { return 0; }
inline DWORD   SetBkColor(HDC, DWORD c) { return c; }
inline DWORD   SetTextColor(HDC, DWORD) { return 0; }
inline HDC     GetDC(HWND) { return nullptr; }
inline int     ReleaseDC(HWND, HDC) { return 1; }
inline HGDIOBJ GetCurrentObject(HDC, UINT) { return (HGDIOBJ)g_hFont; }
inline BOOL    SwapBuffers(HDC) { return TRUE; }

// ─────────────────────────────────────────────────────────────────────────────
// Misc Windows stubs
// ─────────────────────────────────────────────────────────────────────────────
#include <dlfcn.h>
inline BOOL ShellExecuteA(HWND, const char*, const char*, const char*, const char*, int) { return TRUE; }
inline BOOL ShellExecute(HWND h, const char* op, const char* f, const char* p, const char* d, int sw) { return ShellExecuteA(h,op,f,p,d,sw); }
inline HMODULE LoadLibraryA(const char* path) { return path ? (HMODULE)dlopen(path, RTLD_NOW) : nullptr; }
inline HMODULE LoadLibrary(const char* path) { return LoadLibraryA(path); }
inline HMODULE LoadLibraryW(const wchar_t* path)
{
    if (!path) return nullptr;
    char utf8Path[1024] = {0};
    if (wcstombs(utf8Path, path, sizeof(utf8Path) - 1) == (size_t)-1)
        return nullptr;
    return LoadLibraryA(utf8Path);
}
inline BOOL FreeLibrary(HMODULE module)
{
    if (!module) return FALSE;
    return dlclose((void*)module) == 0 ? TRUE : FALSE;
}
inline FARPROC GetProcAddress(HMODULE module, const char* name)
{
    if (!module || !name) return nullptr;
    return (FARPROC)dlsym((void*)module, name);
}
inline HMODULE GetModuleHandleA(const char*) { return nullptr; }
inline HMODULE GetModuleHandleW(const wchar_t*) { return nullptr; }
inline void ExitProcess(UINT) { exit(0); }

// ─────────────────────────────────────────────────────────────────────────────
// MSVC safe-string functions → POSIX equivalents
// ─────────────────────────────────────────────────────────────────────────────
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

inline size_t _muObjectSize(const char* buf)
{
#if defined(__GNUC__) || defined(__clang__)
    size_t sz = __builtin_object_size(buf, 0);
    return (sz == (size_t)-1) ? 4096u : sz;
#else
    (void)buf;
    return 4096u;
#endif
}

template <typename... Args>
inline int _muSprintfS(char* buf, size_t size, const char* fmt, Args... args)
{
    if (!buf || !fmt || size == 0)
        return -1;
    return snprintf(buf, size, fmt, args...);
}

inline int _muSprintfS(char* buf, size_t size, const char* text)
{
    if (!buf || !text || size == 0)
        return -1;
    return snprintf(buf, size, "%s", text);
}

template <typename... Args>
inline int _muSprintfS(char* buf, const char* fmt, Args... args)
{
    return _muSprintfS(buf, _muObjectSize(buf), fmt, args...);
}

inline int _muSprintfS(char* buf, const char* text)
{
    return _muSprintfS(buf, _muObjectSize(buf), text);
}

inline int _muVsprintfS(char* buf, size_t size, const char* fmt, va_list va)
{
    if (!buf || !fmt || size == 0)
        return -1;
    return vsnprintf(buf, size, fmt, va);
}

inline int _muVsprintfS(char* buf, const char* fmt, va_list va)
{
    return _muVsprintfS(buf, _muObjectSize(buf), fmt, va);
}

template <typename... Args>
inline int _muSnprintfS(char* buf, size_t size, size_t /*count*/, const char* fmt, Args... args)
{
    if (!buf || !fmt || size == 0)
        return -1;
    return snprintf(buf, size, fmt, args...);
}

inline int _muVsnprintfS(char* buf, size_t size, size_t /*count*/, const char* fmt, va_list va)
{
    if (!buf || !fmt || size == 0)
        return -1;
    return vsnprintf(buf, size, fmt, va);
}

#ifndef _TRUNCATE
#  define _TRUNCATE ((size_t)-1)
#endif

inline int _muStrncpyS(char* dst, size_t dstSize, const char* src, size_t count)
{
    if (!dst || !src || dstSize == 0)
        return -1;

    size_t n = count;
    if (count == _TRUNCATE || count >= dstSize)
        n = dstSize - 1;

    const size_t srcLen = strlen(src);
    if (n > srcLen)
        n = srcLen;

    memcpy(dst, src, n);
    dst[n] = '\0';
    return 0;
}

inline int _muStrncpyS(char* dst, const char* src, size_t count)
{
    return _muStrncpyS(dst, _muObjectSize(dst), src, count);
}

inline int _muStrncatS(char* dst, size_t dstLimit, const char* src, size_t count)
{
    if (!dst || !src)
        return -1;

    const size_t total = _muObjectSize(dst);
    const size_t len = strlen(dst);
    if (len >= total)
        return -1;

    const size_t available = total - len - 1;
    size_t appendLimit = (dstLimit < available) ? dstLimit : available;

    size_t n = appendLimit;
    if (count != _TRUNCATE && count < n)
        n = count;

    strncat(dst, src, n);
    return 0;
}

inline int _muStrcpyS(char* dst, size_t dstSize, const char* src)
{
    return _muStrncpyS(dst, dstSize, src, _TRUNCATE);
}

inline int _muStrcpyS(char* dst, const char* src)
{
    return _muStrcpyS(dst, _muObjectSize(dst), src);
}

inline int _muUltoaS(unsigned long value, char* buf, size_t size, int base)
{
    if (!buf || size == 0) return -1;
    const char* fmt = (base == 16) ? "%lx" : "%lu";
    return snprintf(buf, size, fmt, value);
}

inline int _muUltoaS(unsigned long value, char* buf, int base)
{
    return _muUltoaS(value, buf, _muObjectSize(buf), base);
}

#ifndef sprintf_s
#  define sprintf_s(...) _muSprintfS(__VA_ARGS__)
#endif
#ifndef vsprintf_s
#  define vsprintf_s(...) _muVsprintfS(__VA_ARGS__)
#endif
#ifndef _snprintf_s
#  define _snprintf_s(...) _muSnprintfS(__VA_ARGS__)
#endif
#ifndef strcpy_s
#  define strcpy_s(...) _muStrcpyS(__VA_ARGS__)
#endif
#ifndef strcat_s
#  define strcat_s(dst, sz, src)  strncat((dst),(src),(sz)-strlen(dst)-1)
#endif
#ifndef strncpy_s
#  define strncpy_s(...) _muStrncpyS(__VA_ARGS__)
#endif
#ifndef strncat_s
#  define strncat_s(dst, sz, src, cnt) _muStrncatS((dst),(sz),(src),(cnt))
#endif
#ifndef _itoa_s
#  define _itoa_s(val, buf, sz, base) (snprintf((buf),(sz),((base)==16?"%x":"%d"),(val)),(void)0)
#endif
#ifndef _ultoa_s
#  define _ultoa_s(...) _muUltoaS(__VA_ARGS__)
#endif
// wsprintf (Win32 ANSI sprintf, no size arg) → snprintf with large buffer size
#ifndef wsprintf
#  define wsprintf(buf, fmt, ...) snprintf((buf), 4096, (fmt), ##__VA_ARGS__)
#endif
// _vsnprintf_s → vsnprintf
#ifndef _vsnprintf_s
#  define _vsnprintf_s(...) _muVsnprintfS(__VA_ARGS__)
#endif

// ─────────────────────────────────────────────────────────────────────────────
// System information stubs
// ─────────────────────────────────────────────────────────────────────────────
struct SYSTEM_INFO {
    WORD  wProcessorArchitecture;
    WORD  wReserved;
    DWORD dwPageSize;
    void* lpMinimumApplicationAddress;
    void* lpMaximumApplicationAddress;
    DWORD* dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD  wProcessorLevel;
    WORD  wProcessorRevision;
};
inline void GetSystemInfo(SYSTEM_INFO* si) {
    if (si) { memset(si, 0, sizeof(*si)); si->dwNumberOfProcessors = 1; }
}
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *LPSYSTEMTIME;

inline void GetLocalTime(LPSYSTEMTIME st)
{
    if (!st) return;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tmv;
    localtime_r(&ts.tv_sec, &tmv);
    st->wYear = (WORD)(tmv.tm_year + 1900);
    st->wMonth = (WORD)(tmv.tm_mon + 1);
    st->wDayOfWeek = (WORD)tmv.tm_wday;
    st->wDay = (WORD)tmv.tm_mday;
    st->wHour = (WORD)tmv.tm_hour;
    st->wMinute = (WORD)tmv.tm_min;
    st->wSecond = (WORD)tmv.tm_sec;
    st->wMilliseconds = (WORD)(ts.tv_nsec / 1000000);
}
inline BOOL GetVolumeInformation(const char*,void*,DWORD,DWORD* sn,DWORD*,DWORD*,void*,DWORD)
{
    if (sn) *sn = 0x12345678;
    return TRUE;
}

// ─────────────────────────────────────────────────────────────────────────────
// Cursor / mouse shims — read from the game's global MouseX/MouseY
// (defined in ZzzOpenglUtil.cpp, declared in ZzzOpenglUtil.h)
// ─────────────────────────────────────────────────────────────────────────────
extern float MouseX;
extern float MouseY;

inline BOOL GetCursorPos(POINT* pt)
{
    if (pt) { pt->x = (LONG)MouseX; pt->y = (LONG)MouseY; }
    return TRUE;
}
// On Android, touch coords are already in client (window) space
inline BOOL ScreenToClient(HWND, POINT*) { return TRUE; }
inline BOOL ClientToScreen(HWND, POINT*) { return TRUE; }
inline BOOL SetCursorPos(int x, int y)
{
    MouseX = (float)x; MouseY = (float)y; return TRUE;
}
inline HCURSOR SetCursor(HCURSOR) { return nullptr; }
inline HCURSOR LoadCursor(HINSTANCE, const char*) { return nullptr; }
inline int     ShowCursor(BOOL) { return 0; }
inline DWORD   GetDoubleClickTime() { return 500; }  // 500 ms
inline SHORT   GetAsyncKeyState(int) { return 0; }
inline BOOL IsDebuggerPresent() { return FALSE; }
inline DWORD GetCurrentThreadId() { return (DWORD)(uintptr_t)pthread_self(); }
inline DWORD GetCurrentProcessId() { return (DWORD)getpid(); }
inline void _tzset() { tzset(); }

// ─────────────────────────────────────────────────────────────────────────────
// Themida / Nprotect macros → no-op on Android
// ─────────────────────────────────────────────────────────────────────────────
#define VM_START
#define VM_END
#define VIRTUAL_START
#define VIRTUAL_END
#define CODECRYPT_START
#define CODECRYPT_END

// ─────────────────────────────────────────────────────────────────────────────
// Win32 File I/O shims
// CreateFile/ReadFile/WriteFile/CloseHandle → fopen/fread/fwrite/fclose
// HANDLE is void*; we store FILE* cast to void*.
// ─────────────────────────────────────────────────────────────────────────────
#include <fcntl.h>
#include <sys/stat.h>

// Access / share flags (values match Windows but are unused by our POSIX impl)
#define GENERIC_READ              0x80000000u
#define GENERIC_WRITE             0x40000000u
#define FILE_SHARE_READ           0x00000001u
#define FILE_SHARE_WRITE          0x00000002u
#define FILE_SHARE_DELETE         0x00000004u
#define FILE_APPEND_DATA          0x00000004u

// Creation disposition
#define CREATE_NEW                1
#define CREATE_ALWAYS             2
#define OPEN_EXISTING             3
#define OPEN_ALWAYS               4
#define TRUNCATE_EXISTING         5

// Attributes
#define FILE_ATTRIBUTE_READONLY   0x01
#define FILE_ATTRIBUTE_HIDDEN     0x02
#define FILE_ATTRIBUTE_SYSTEM     0x04
#define FILE_ATTRIBUTE_DIRECTORY  0x10
#define FILE_ATTRIBUTE_ARCHIVE    0x20
#define FILE_ATTRIBUTE_NORMAL     0x80

// SetFilePointer origins (map to SEEK_ values)
#define FILE_BEGIN   SEEK_SET
#define FILE_CURRENT SEEK_CUR
#define FILE_END     SEEK_END

// GDI_ERROR (returned by GetFontData on failure)
#define GDI_ERROR    ((DWORD)0xFFFFFFFF)

static inline HANDLE CreateFileA(
    const char* path, DWORD access, DWORD /*share*/, void* /*sa*/,
    DWORD creation, DWORD /*attrs*/, HANDLE /*hTpl*/)
{
    const char* mode = "rb";
    if ((access & GENERIC_WRITE) && (access & GENERIC_READ))
        mode = (creation == OPEN_EXISTING) ? "r+b" : "w+b";
    else if (access & GENERIC_WRITE)
        mode = (creation == OPEN_ALWAYS || creation == OPEN_EXISTING) ? "ab" :
               (creation == CREATE_ALWAYS || creation == CREATE_NEW)  ? "wb" : "ab";
    else if (access & FILE_APPEND_DATA)
        mode = "ab";
    // else GENERIC_READ
    FILE* f = fopen(path, mode);
    if (!f)
    {
        if (creation == OPEN_ALWAYS || creation == CREATE_ALWAYS)
        {
            // Try creating parent dirs and retry
            f = fopen(path, "wb");
        }
        if (!f) return INVALID_HANDLE_VALUE;
    }
    return (HANDLE)f;
}
#define CreateFile  CreateFileA
#define CreateFileW CreateFileA  // wide-char version — not used in this codebase

static inline BOOL ReadFile(HANDLE h, void* buf, DWORD n, DWORD* nRead, void* /*ol*/)
{
    if (h == INVALID_HANDLE_VALUE || !h) { if (nRead) *nRead = 0; return FALSE; }
    size_t r = fread(buf, 1, n, (FILE*)h);
    if (nRead) *nRead = (DWORD)r;
    return r > 0 || n == 0 ? TRUE : FALSE;
}

static inline BOOL WriteFile(HANDLE h, const void* buf, DWORD n, DWORD* nWritten, void* /*ol*/)
{
    if (h == INVALID_HANDLE_VALUE || !h) { if (nWritten) *nWritten = 0; return FALSE; }
    size_t w = fwrite(buf, 1, n, (FILE*)h);
    if (nWritten) *nWritten = (DWORD)w;
    return (w == n) ? TRUE : FALSE;
}

static inline BOOL CloseHandle(HANDLE h)
{
    if (h && h != INVALID_HANDLE_VALUE) fclose((FILE*)h);
    return TRUE;
}

static inline DWORD GetFileSize(HANDLE h, DWORD* /*high*/)
{
    if (!h || h == INVALID_HANDLE_VALUE) return 0xFFFFFFFF;
    FILE* f = (FILE*)h;
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, pos, SEEK_SET);
    return (DWORD)sz;
}

static inline DWORD SetFilePointer(HANDLE h, LONG dist, LONG* /*high*/, DWORD method)
{
    if (!h || h == INVALID_HANDLE_VALUE) return 0xFFFFFFFF;
    fseek((FILE*)h, (long)dist, (int)method);
    return (DWORD)ftell((FILE*)h);
}

static inline BOOL SetEndOfFile(HANDLE) { return TRUE; } // stub

static inline DWORD GetLastError()
{
    return (DWORD)errno;
}

static inline DWORD GetFileAttributes(const char* path)
{
    struct stat st;
    if (stat(path, &st) != 0) return 0xFFFFFFFF; // INVALID_FILE_ATTRIBUTES
    if (S_ISDIR(st.st_mode)) return FILE_ATTRIBUTE_DIRECTORY;
    return FILE_ATTRIBUTE_NORMAL;
}

// WaitForSingleObject on file handles is a no-op (threads use std::thread)
#define WAIT_TIMEOUT   0x00000102
#define WAIT_OBJECT_0  0
#define WAIT_FAILED    0xFFFFFFFF
#define INFINITE       0xFFFFFFFF
static inline DWORD WaitForSingleObject(HANDLE, DWORD) { return WAIT_OBJECT_0; }
static inline BOOL  TerminateThread(HANDLE, DWORD)      { return TRUE; }
static inline BOOL  GetExitCodeThread(HANDLE, DWORD* c) { if (c) *c = 0; return TRUE; }

// ─────────────────────────────────────────────────────────────────────────────
// INI file functions (GetPrivateProfileInt/String / WritePrivateProfileString)
// Implemented as a minimal POSIX INI reader — no section+key caching, but
// sufficient for the ~25 call sites in this codebase.
// ─────────────────────────────────────────────────────────────────────────────
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "GameAssetPath.h"

static inline bool _AndroidINI_FindKey(
    const char* section, const char* key, const char* filename,
    char* outBuf, int outLen)
{
    // Resolve path through GameAssetPath (handles backslash → forward slash)
    std::string fullPath;
    if (filename && filename[0] == '.')
        fullPath = GameAssetPath::Resolve(filename + 2); // strip leading ".\"
    else if (filename)
        fullPath = GameAssetPath::Resolve(filename);
    else
        return false;

    FILE* f = fopen(fullPath.c_str(), "r");
    if (!f) return false;

    char line[512];
    char curSection[128] = "";
    bool inSection = false;

    while (fgets(line, sizeof(line), f))
    {
        // Trim trailing newline
        char* nl = strrchr(line, '\n'); if (nl) *nl = '\0';
        nl = strrchr(line, '\r'); if (nl) *nl = '\0';

        // Skip comments and blank lines
        if (line[0] == ';' || line[0] == '#' || line[0] == '\0') continue;

        // Section header
        if (line[0] == '[')
        {
            char* end = strchr(line, ']');
            if (end)
            {
                *end = '\0';
                strncpy(curSection, line + 1, sizeof(curSection) - 1);
                curSection[sizeof(curSection)-1] = '\0';
                inSection = (strcasecmp(curSection, section) == 0);
            }
            continue;
        }

        if (!inSection) continue;

        // key=value
        char* eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        // Trim key
        char* k = line;
        while (*k == ' ' || *k == '\t') k++;
        char* ke = eq - 1;
        while (ke > k && (*ke == ' ' || *ke == '\t')) *ke-- = '\0';

        if (strcasecmp(k, key) != 0) continue;

        // Trim value
        char* v = eq + 1;
        while (*v == ' ' || *v == '\t') v++;
        strncpy(outBuf, v, outLen - 1);
        outBuf[outLen - 1] = '\0';
        fclose(f);
        return true;
    }
    fclose(f);
    return false;
}

inline UINT GetPrivateProfileInt(
    const char* section, const char* key, int defaultVal, const char* filename)
{
    char buf[64] = "";
    if (_AndroidINI_FindKey(section, key, filename, buf, sizeof(buf)))
        return (UINT)atoi(buf);
    return (UINT)defaultVal;
}

inline DWORD GetPrivateProfileString(
    const char* section, const char* key, const char* defaultVal,
    char* returnBuf, DWORD returnBufSize, const char* filename)
{
    char buf[512] = "";
    const char* result = defaultVal ? defaultVal : "";
    if (_AndroidINI_FindKey(section, key, filename, buf, sizeof(buf)))
        result = buf;
    if (returnBuf && returnBufSize > 0)
    {
        strncpy(returnBuf, result, returnBufSize - 1);
        returnBuf[returnBufSize - 1] = '\0';
    }
    return (DWORD)strlen(returnBuf ? returnBuf : "");
}

inline BOOL WritePrivateProfileString(
    const char* /*section*/, const char* /*key*/, const char* /*value*/, const char* /*filename*/)
{
    // TODO: implement write-back if config persistence is needed
    return TRUE; // no-op
}
