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
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
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
typedef BYTE*               PBYTE;
typedef BOOL*               LPBOOL;

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
#ifndef RtlSecureZeroMemory
#  define RtlSecureZeroMemory(dst, len) memset((dst), 0, (len))
#endif
#ifndef SecureZeroMemory
#  define SecureZeroMemory(dst, len) memset((dst), 0, (len))
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
#ifndef INTERNET_DEFAULT_HTTP_PORT
#  define INTERNET_DEFAULT_HTTP_PORT 80
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
#ifndef ERROR_SUCCESS
#  define ERROR_SUCCESS 0
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
#ifndef CP_ACP
#  define CP_ACP 0
#endif
#ifndef _atoi64
#  define _atoi64 atoll
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

struct AndroidCompatWindowState
{
    std::wstring className;
    std::wstring text;
    LONG_PTR wndProc;
    LONG_PTR userData;
    DWORD style;
    UINT textLimit;
    bool visible;
};

inline std::unordered_map<HWND, AndroidCompatWindowState>& AndroidCompatWindowMap()
{
    static std::unordered_map<HWND, AndroidCompatWindowState> s_windowMap;
    return s_windowMap;
}

inline std::mutex& AndroidCompatWindowMutex()
{
    static std::mutex s_windowMutex;
    return s_windowMutex;
}

inline std::unordered_set<HANDLE>& AndroidCompatFileHandleSet()
{
    static std::unordered_set<HANDLE> s_fileHandles;
    return s_fileHandles;
}

inline std::mutex& AndroidCompatFileHandleMutex()
{
    static std::mutex s_fileHandleMutex;
    return s_fileHandleMutex;
}

inline HWND& AndroidCompatFocusedWindow()
{
    static HWND s_focusedWindow = nullptr;
    return s_focusedWindow;
}

void AndroidShowSoftKeyboard();
void AndroidHideSoftKeyboard();

inline AndroidCompatWindowState* AndroidCompatGetWindowState(HWND hWnd)
{
    if (!hWnd)
    {
        return nullptr;
    }

    auto& windowMap = AndroidCompatWindowMap();
    auto it = windowMap.find(hWnd);
    if (it == windowMap.end())
    {
        return nullptr;
    }

    return &it->second;
}

inline int AndroidCompatCountLines(const std::wstring& text)
{
    if (text.empty())
    {
        return 1;
    }

    int lineCount = 1;
    for (wchar_t ch : text)
    {
        if (ch == L'\n')
        {
            ++lineCount;
        }
    }

    return lineCount;
}

inline size_t AndroidCompatMinSize(size_t lhs, size_t rhs)
{
    return (lhs < rhs) ? lhs : rhs;
}

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

inline BOOL OffsetRect(LPRECT lprc, int dx, int dy)
{
    if (!lprc)
    {
        return FALSE;
    }

    lprc->left += dx;
    lprc->right += dx;
    lprc->top += dy;
    lprc->bottom += dy;
    return TRUE;
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
#define WM_SYSKEYDOWN       0x0104
#define WM_SETTEXT          0x000C
#define WM_GETTEXT          0x000D
#define WM_GETTEXTLENGTH    0x000E
#define WM_ERASEBKGND       0x0014
#define WM_SETFONT          0x0030
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

#define WS_CHILD             0x40000000L
#define WS_VISIBLE           0x10000000L
#define WS_VSCROLL           0x00200000L
#define WS_POPUP             0x80000000L
#define WS_OVERLAPPED        0x00000000L
#define WS_OVERLAPPEDWINDOW  0x00CF0000L
#define WS_CAPTION           0x00C00000L
#define WS_SYSMENU           0x00080000L
#define WS_MINIMIZEBOX       0x00020000L
#define WS_BORDER            0x00800000L
#define WS_CLIPCHILDREN      0x02000000L
#define WS_CLIPSIBLINGS      0x04000000L

#define ES_MULTILINE         0x0004L
#define ES_PASSWORD          0x0020L
#define ES_AUTOHSCROLL       0x0080L
#define ES_AUTOVSCROLL       0x0040L

#define EM_GETLINECOUNT      0x00BA
#define EM_SETSEL            0x00B1
#define EM_SCROLL            0x00B5
#define EM_LINESCROLL        0x00B6
#define EM_SETLIMITTEXT      0x00C5

#define SB_LINEUP            0
#define SB_LINEDOWN          1
#define SB_PAGEUP            2
#define SB_PAGEDOWN          3
#define SB_VERT              1

#define SWP_NOSIZE           0x0001
#define SWP_NOMOVE           0x0002
#define SWP_NOZORDER         0x0004
#define SWP_NOACTIVATE       0x0010
#define SWP_SHOWWINDOW       0x0040
#define SWP_HIDEWINDOW       0x0080

#define HTTP_QUERY_CONTENT_LENGTH 5
#define HTTP_QUERY_STATUS_CODE    19
#define HTTP_STATUS_OK            200

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _WIN32_FIND_DATAA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    DWORD    dwReserved0;
    DWORD    dwReserved1;
    char     cFileName[MAX_PATH];
    char     cAlternateFileName[14];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

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
inline size_t AndroidCompatStrnlen(const char* s, size_t maxLen)
{
    if (!s) return 0;
    size_t n = 0;
    while (n < maxLen && s[n] != '\0') ++n;
    return n;
}
inline size_t AndroidCompatWcsnlen(const wchar_t* s, size_t maxLen)
{
    if (!s) return 0;
    size_t n = 0;
    while (n < maxLen && s[n] != L'\0') ++n;
    return n;
}
inline HRESULT StringCchLengthA(const char* src, size_t cchMax, size_t* pcch)
{
    if (!src || !pcch) return E_INVALIDARG;
    size_t len = AndroidCompatStrnlen(src, cchMax);
    *pcch = len;
    return (len == cchMax && cchMax > 0 && src[cchMax - 1] != '\0') ? E_FAIL : S_OK;
}
inline HRESULT StringCchLengthW(const wchar_t* src, size_t cchMax, size_t* pcch)
{
    if (!src || !pcch) return E_INVALIDARG;
    size_t len = AndroidCompatWcsnlen(src, cchMax);
    *pcch = len;
    return (len == cchMax && cchMax > 0 && src[cchMax - 1] != L'\0') ? E_FAIL : S_OK;
}
inline HRESULT StringCchPrintfA(char* dst, size_t cchDst, const char* fmt, ...)
{
    if (!dst || cchDst == 0 || !fmt) return E_INVALIDARG;
    va_list args;
    va_start(args, fmt);
    int rc = vsnprintf(dst, cchDst, fmt, args);
    va_end(args);
    dst[cchDst - 1] = '\0';
    return (rc < 0 || (size_t)rc >= cchDst) ? E_FAIL : S_OK;
}
inline HRESULT StringCchPrintfW(wchar_t* dst, size_t cchDst, const wchar_t* fmt, ...)
{
    if (!dst || cchDst == 0 || !fmt) return E_INVALIDARG;
    va_list args;
    va_start(args, fmt);
    int rc = vswprintf(dst, cchDst, fmt, args);
    va_end(args);
    dst[cchDst - 1] = L'\0';
    return (rc < 0 || (size_t)rc >= cchDst) ? E_FAIL : S_OK;
}
inline HRESULT StringCchVPrintfA(char* dst, size_t cchDst, const char* fmt, va_list args)
{
    if (!dst || cchDst == 0 || !fmt) return E_INVALIDARG;
    int rc = vsnprintf(dst, cchDst, fmt, args);
    dst[cchDst - 1] = '\0';
    return (rc < 0 || (size_t)rc >= cchDst) ? E_FAIL : S_OK;
}
inline HRESULT StringCchVPrintfW(wchar_t* dst, size_t cchDst, const wchar_t* fmt, va_list args)
{
    if (!dst || cchDst == 0 || !fmt) return E_INVALIDARG;
    int rc = vswprintf(dst, cchDst, fmt, args);
    dst[cchDst - 1] = L'\0';
    return (rc < 0 || (size_t)rc >= cchDst) ? E_FAIL : S_OK;
}
#ifndef StringCchLength
#  define StringCchLength StringCchLengthA
#endif
#ifndef StringCchPrintf
#  define StringCchPrintf StringCchPrintfA
#endif
#ifndef StringCchVPrintf
#  define StringCchVPrintf StringCchVPrintfA
#endif

inline bool AndroidCompatDecodeUtf8Char(const char* s, size_t len, size_t* consumed, uint32_t* codepoint)
{
    if (!s || len == 0 || !consumed || !codepoint)
    {
        return false;
    }

    const unsigned char b0 = (unsigned char)s[0];
    if ((b0 & 0x80u) == 0u)
    {
        *consumed = 1;
        *codepoint = b0;
        return true;
    }

    if ((b0 & 0xE0u) == 0xC0u && len >= 2)
    {
        const unsigned char b1 = (unsigned char)s[1];
        if ((b1 & 0xC0u) == 0x80u)
        {
            *consumed = 2;
            *codepoint = ((uint32_t)(b0 & 0x1Fu) << 6) | (uint32_t)(b1 & 0x3Fu);
            return true;
        }
    }
    else if ((b0 & 0xF0u) == 0xE0u && len >= 3)
    {
        const unsigned char b1 = (unsigned char)s[1];
        const unsigned char b2 = (unsigned char)s[2];
        if ((b1 & 0xC0u) == 0x80u && (b2 & 0xC0u) == 0x80u)
        {
            *consumed = 3;
            *codepoint = ((uint32_t)(b0 & 0x0Fu) << 12) |
                         ((uint32_t)(b1 & 0x3Fu) << 6) |
                         (uint32_t)(b2 & 0x3Fu);
            return true;
        }
    }
    else if ((b0 & 0xF8u) == 0xF0u && len >= 4)
    {
        const unsigned char b1 = (unsigned char)s[1];
        const unsigned char b2 = (unsigned char)s[2];
        const unsigned char b3 = (unsigned char)s[3];
        if ((b1 & 0xC0u) == 0x80u && (b2 & 0xC0u) == 0x80u && (b3 & 0xC0u) == 0x80u)
        {
            *consumed = 4;
            *codepoint = ((uint32_t)(b0 & 0x07u) << 18) |
                         ((uint32_t)(b1 & 0x3Fu) << 12) |
                         ((uint32_t)(b2 & 0x3Fu) << 6) |
                         (uint32_t)(b3 & 0x3Fu);
            return true;
        }
    }

    *consumed = 1;
    *codepoint = (uint32_t)'?';
    return false;
}

inline int AndroidCompatEncodeUtf8Char(uint32_t cp, char out[4])
{
    if (!out) return 0;
    if (cp <= 0x7Fu)
    {
        out[0] = (char)cp;
        return 1;
    }
    if (cp <= 0x7FFu)
    {
        out[0] = (char)(0xC0u | (cp >> 6));
        out[1] = (char)(0x80u | (cp & 0x3Fu));
        return 2;
    }
    if (cp <= 0xFFFFu)
    {
        out[0] = (char)(0xE0u | (cp >> 12));
        out[1] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
        out[2] = (char)(0x80u | (cp & 0x3Fu));
        return 3;
    }
    if (cp <= 0x10FFFFu)
    {
        out[0] = (char)(0xF0u | (cp >> 18));
        out[1] = (char)(0x80u | ((cp >> 12) & 0x3Fu));
        out[2] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
        out[3] = (char)(0x80u | (cp & 0x3Fu));
        return 4;
    }
    out[0] = '?';
    return 1;
}

inline int MultiByteToWideChar(UINT codePage, DWORD, const char* src, int cbMultiByte, wchar_t* dst, int cchWideChar)
{
    if (!src) return 0;
    if (cbMultiByte == 0) return 0;

    const bool sourceIsNullTerminated = (cbMultiByte < 0);
    const size_t srcLen = sourceIsNullTerminated ? strlen(src) : (size_t)cbMultiByte;
    int outCount = 0;

    auto appendWChar = [&](wchar_t wc) -> bool
    {
        if (dst)
        {
            if (outCount >= cchWideChar)
            {
                return false;
            }
            dst[outCount] = wc;
        }
        ++outCount;
        return true;
    };

    if (codePage == CP_UTF8 || codePage == CP_ACP)
    {
        size_t i = 0;
        while (i < srcLen)
        {
            size_t consumed = 0;
            uint32_t cp = 0;
            AndroidCompatDecodeUtf8Char(src + i, srcLen - i, &consumed, &cp);
            i += consumed;

#if WCHAR_MAX <= 0xFFFF
            if (cp > 0xFFFFu)
            {
                cp -= 0x10000u;
                if (!appendWChar((wchar_t)(0xD800u + (cp >> 10)))) return 0;
                if (!appendWChar((wchar_t)(0xDC00u + (cp & 0x3FFu)))) return 0;
            }
            else
#endif
            {
                if (!appendWChar((wchar_t)cp)) return 0;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < srcLen; ++i)
        {
            if (!appendWChar((wchar_t)(unsigned char)src[i])) return 0;
        }
    }

    if (sourceIsNullTerminated)
    {
        if (!appendWChar(L'\0')) return 0;
    }

    return outCount;
}

inline int WideCharToMultiByte(UINT codePage, DWORD, const wchar_t* src, int cchWideChar, char* dst, int cbMultiByte, const char*, BOOL* lpUsedDefaultChar)
{
    if (!src) return 0;
    if (cchWideChar == 0) return 0;

    if (lpUsedDefaultChar) *lpUsedDefaultChar = FALSE;

    const bool sourceIsNullTerminated = (cchWideChar < 0);
    const size_t srcLen = sourceIsNullTerminated ? wcslen(src) : (size_t)cchWideChar;
    int outCount = 0;

    auto appendByte = [&](char ch) -> bool
    {
        if (dst)
        {
            if (outCount >= cbMultiByte)
            {
                return false;
            }
            dst[outCount] = ch;
        }
        ++outCount;
        return true;
    };

    if (codePage == CP_UTF8 || codePage == CP_ACP)
    {
        size_t i = 0;
        while (i < srcLen)
        {
            uint32_t cp = (uint32_t)src[i++];
#if WCHAR_MAX <= 0xFFFF
            if (cp >= 0xD800u && cp <= 0xDBFFu && i < srcLen)
            {
                const uint32_t low = (uint32_t)src[i];
                if (low >= 0xDC00u && low <= 0xDFFFu)
                {
                    ++i;
                    cp = 0x10000u + (((cp - 0xD800u) << 10) | (low - 0xDC00u));
                }
            }
#endif
            char utf8[4];
            int utf8Len = AndroidCompatEncodeUtf8Char(cp, utf8);
            for (int n = 0; n < utf8Len; ++n)
            {
                if (!appendByte(utf8[n])) return 0;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < srcLen; ++i)
        {
            wchar_t wc = src[i];
            char ch = (wc >= 0 && wc <= 0xFF) ? (char)wc : '?';
            if (wc < 0 || wc > 0xFF)
            {
                if (lpUsedDefaultChar) *lpUsedDefaultChar = TRUE;
            }
            if (!appendByte(ch)) return 0;
        }
    }

    if (sourceIsNullTerminated)
    {
        if (!appendByte('\0')) return 0;
    }

    return outCount;
}

#ifndef _tcschr
#  define _tcschr strchr
#endif
#ifndef _tcsrchr
#  define _tcsrchr strrchr
#endif
#ifndef _tcsstr
#  define _tcsstr strstr
#endif
#ifndef _tcsncpy
#  define _tcsncpy strncpy
#endif
#ifndef _tcslwr
inline char* _tcslwr(char* s)
{
    if (!s) return s;
    for (char* p = s; *p; ++p)
    {
        *p = (char)tolower((unsigned char)*p);
    }
    return s;
}
#endif
#ifndef _tcscat_s
#  define _tcscat_s(dst, sz, src) strcat_s((dst), (sz), (src))
#endif
#ifndef _tcsncpy_s
#  define _tcsncpy_s(dst, sz, src, cnt) strncpy_s((dst), (sz), (src), (cnt))
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

inline DWORD GetModuleFileNameA(HMODULE /*hModule*/, char* lpFilename, DWORD nSize)
{
    if (!lpFilename || nSize == 0)
    {
        return 0;
    }

    const ssize_t pathLen = readlink("/proc/self/exe", lpFilename, nSize - 1);
    if (pathLen < 0)
    {
        lpFilename[0] = '\0';
        return 0;
    }

    lpFilename[pathLen] = '\0';
    return (DWORD)pathLen;
}

inline DWORD GetModuleFileNameW(HMODULE hModule, wchar_t* lpFilename, DWORD nSize)
{
    if (!lpFilename || nSize == 0)
    {
        return 0;
    }

    char path[PATH_MAX > 0 ? PATH_MAX : 4096];
    const DWORD pathLen = GetModuleFileNameA(hModule, path, (DWORD)sizeof(path));
    if (pathLen == 0)
    {
        lpFilename[0] = L'\0';
        return 0;
    }

    const size_t wideLen = mbstowcs(lpFilename, path, nSize - 1);
    if (wideLen == (size_t)-1)
    {
        lpFilename[0] = L'\0';
        return 0;
    }

    lpFilename[wideLen] = L'\0';
    return (DWORD)wideLen;
}

inline DWORD GetModuleFileName(HMODULE hModule, char* lpFilename, DWORD nSize)
{
    return GetModuleFileNameA(hModule, lpFilename, nSize);
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
inline HWND CreateWindowW(const wchar_t* className, const wchar_t* windowName, DWORD style,
                          int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID)
{
    AndroidCompatWindowState state{};
    state.className = className ? className : L"";
    state.text = windowName ? windowName : L"";
    state.wndProc = 0;
    state.userData = 0;
    state.style = style;
    state.textLimit = 0x7FFFFFFFu;
    state.visible = (style & WS_VISIBLE) != 0;

    AndroidCompatWindowState* handleState = new AndroidCompatWindowState(state);
    HWND handle = reinterpret_cast<HWND>(handleState);

    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    AndroidCompatWindowMap()[handle] = state;
    return handle;
}
inline LRESULT SendMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    AndroidCompatWindowState* state = AndroidCompatGetWindowState(hWnd);
    if (!state)
    {
        return 0;
    }

    switch (msg)
    {
    case WM_SETTEXT:
    {
        const wchar_t* newText = reinterpret_cast<const wchar_t*>(lParam);
        if (!newText)
        {
            state->text.clear();
            return TRUE;
        }

        size_t sourceLength = wcslen(newText);
        size_t maxLength = (state->textLimit > 0) ? (size_t)state->textLimit : sourceLength;
        state->text.assign(newText, AndroidCompatMinSize(sourceLength, maxLength));
        return TRUE;
    }
    case WM_GETTEXT:
    {
        wchar_t* output = reinterpret_cast<wchar_t*>(lParam);
        if (!output || wParam == 0)
        {
            return 0;
        }

        size_t maxChars = static_cast<size_t>(wParam);
        size_t copyLength = AndroidCompatMinSize(state->text.size(), maxChars - 1);
        if (copyLength > 0)
        {
            wmemcpy(output, state->text.c_str(), copyLength);
        }
        output[copyLength] = L'\0';
        return static_cast<LRESULT>(copyLength);
    }
    case WM_GETTEXTLENGTH:
        return static_cast<LRESULT>(state->text.size());
    case WM_SETFONT:
        return 0;
    case EM_SETLIMITTEXT:
        state->textLimit = static_cast<UINT>(wParam);
        if (state->text.size() > state->textLimit)
        {
            state->text.resize(state->textLimit);
        }
        return 0;
    case EM_GETLINECOUNT:
        return static_cast<LRESULT>(AndroidCompatCountLines(state->text));
    case EM_SETSEL:
    case EM_SCROLL:
    case EM_LINESCROLL:
        return 0;
    default:
        return 0;
    }
}
inline BOOL PostMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SendMessage(hWnd, msg, wParam, lParam);
    return TRUE;
}
inline LRESULT SendMessageW(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return SendMessage(hWnd, msg, wParam, lParam); }
inline BOOL PostMessageW(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return PostMessage(hWnd, msg, wParam, lParam); }
inline BOOL SetTimer(HWND, UINT, UINT, void*)        { return TRUE; }
inline BOOL KillTimer(HWND, UINT)                    { return TRUE; }
inline BOOL ShowWindow(HWND hWnd, int nCmdShow)
{
    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    AndroidCompatWindowState* state = AndroidCompatGetWindowState(hWnd);
    if (state)
    {
        state->visible = (nCmdShow != 0);
    }
    return TRUE;
}
inline BOOL SetWindowPos(HWND hWnd, HWND, int, int, int, int, UINT uFlags)
{
    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    AndroidCompatWindowState* state = AndroidCompatGetWindowState(hWnd);
    if (!state)
    {
        return FALSE;
    }

    if (uFlags & SWP_SHOWWINDOW)
    {
        state->visible = true;
    }
    if (uFlags & SWP_HIDEWINDOW)
    {
        state->visible = false;
    }
    return TRUE;
}
inline BOOL UpdateWindow(HWND)                       { return TRUE; }
inline BOOL DestroyWindow(HWND hWnd)
{
    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    auto& windowMap = AndroidCompatWindowMap();
    auto it = windowMap.find(hWnd);
    if (it == windowMap.end())
    {
        return TRUE;
    }

    AndroidCompatWindowState* state = reinterpret_cast<AndroidCompatWindowState*>(hWnd);
    const bool focusedEdit = (AndroidCompatFocusedWindow() == hWnd && state && state->className == L"edit");
    windowMap.erase(it);
    delete state;

    if (AndroidCompatFocusedWindow() == hWnd)
    {
        AndroidCompatFocusedWindow() = nullptr;
        if (focusedEdit)
        {
            AndroidHideSoftKeyboard();
        }
    }

    return TRUE;
}
inline BOOL InvalidateRect(HWND, const RECT*, BOOL)  { return TRUE; }
inline HWND GetFocus()                               { return AndroidCompatFocusedWindow(); }
inline bool AndroidCompatIsEditControl(HWND hWnd)
{
    if (!hWnd)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    AndroidCompatWindowState* state = AndroidCompatGetWindowState(hWnd);
    return (state && state->className == L"edit");
}
inline HWND SetFocus(HWND h)
{
    HWND previous = AndroidCompatFocusedWindow();
    AndroidCompatFocusedWindow() = h;

    const bool wasEdit = AndroidCompatIsEditControl(previous);
    const bool isEdit = AndroidCompatIsEditControl(h);
    if (!wasEdit && isEdit)
    {
        AndroidShowSoftKeyboard();
    }
    else if (wasEdit && !isEdit)
    {
        AndroidHideSoftKeyboard();
    }

    return previous;
}
inline BOOL IsWindow(HWND h)
{
    if (!h)
    {
        return FALSE;
    }

    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    return AndroidCompatWindowMap().find(h) != AndroidCompatWindowMap().end() ? TRUE : FALSE;
}
inline BOOL IsWindowVisible(HWND hWnd)
{
    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    AndroidCompatWindowState* state = AndroidCompatGetWindowState(hWnd);
    return (state && state->visible) ? TRUE : FALSE;
}
inline int GetScrollPos(HWND, int) { return 0; }
inline int SetScrollPos(HWND, int, int nPos, BOOL) { return nPos; }
inline BOOL IsBadReadPtr(const void* p, UINT_PTR)   { return p == nullptr; }
inline SHORT GetKeyState(int) { return 0; }

#define GWL_WNDPROC  (-4)
#define GWL_USERDATA (-21)

inline LONG_PTR SetWindowLongPtrW(HWND hWnd, int index, LONG_PTR value)
{
    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    AndroidCompatWindowState* state = AndroidCompatGetWindowState(hWnd);
    if (!state)
    {
        return 0;
    }

    LONG_PTR previous = 0;
    if (index == GWL_WNDPROC)
    {
        previous = state->wndProc;
        state->wndProc = value;
    }
    else if (index == GWL_USERDATA)
    {
        previous = state->userData;
        state->userData = value;
    }

    return previous;
}
inline LONG_PTR GetWindowLongPtrW(HWND hWnd, int index)
{
    std::lock_guard<std::mutex> lock(AndroidCompatWindowMutex());
    AndroidCompatWindowState* state = AndroidCompatGetWindowState(hWnd);
    if (!state)
    {
        return 0;
    }

    if (index == GWL_WNDPROC)
    {
        return state->wndProc;
    }
    if (index == GWL_USERDATA)
    {
        return state->userData;
    }

    return 0;
}
inline LONG SetWindowLongW(HWND hWnd, int index, LONG value)
{
    return (LONG)SetWindowLongPtrW(hWnd, index, (LONG_PTR)value);
}
inline LONG GetWindowLongW(HWND hWnd, int index)
{
    return (LONG)GetWindowLongPtrW(hWnd, index);
}
inline LRESULT CallWindowProcW(WNDPROC wndProc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (wndProc)
    {
        return wndProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

#define CF_TEXT 1
inline BOOL OpenClipboard(HWND) { return TRUE; }
inline BOOL CloseClipboard() { return TRUE; }
inline HANDLE GetClipboardData(UINT) { return nullptr; }
inline LPVOID GlobalLock(HGLOBAL hMem) { return hMem; }
inline BOOL GlobalUnlock(HGLOBAL) { return TRUE; }
inline BOOL GetCaretPos(POINT* pt)
{
    if (!pt)
    {
        return FALSE;
    }

    pt->x = 0;
    pt->y = 0;
    return TRUE;
}
inline int GetWindowTextW(HWND hWnd, wchar_t* buffer, int maxCount)
{
    if (!buffer || maxCount <= 0)
    {
        return 0;
    }

    return static_cast<int>(SendMessage(hWnd, WM_GETTEXT, static_cast<WPARAM>(maxCount), reinterpret_cast<LPARAM>(buffer)));
}
inline BOOL SetWindowTextW(HWND hWnd, const wchar_t* text)
{
    return SendMessage(hWnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text)) ? TRUE : FALSE;
}

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
#define MB_ICONWARNING 0x30
#define MB_ICONINFO  0x40
#define IDOK         1
#define IDYES        6
#define IDNO         7
#define SW_SHOW      5
#define SW_NORMAL    1
#define SW_HIDE      0
#define ERROR_ALREADY_EXISTS 183
#ifndef MCI_SEQ_MAPPER
#  define MCI_SEQ_MAPPER 0xFFFFFFFFu
#endif
#define WM_IME_CONTROL            0x0283
#define WM_IME_STARTCOMPOSITION   0x010D
#define WM_IME_ENDCOMPOSITION     0x010E
#define WM_IME_COMPOSITION        0x010F
#define WM_IME_NOTIFY             0x0282
#define IMC_SETCOMPOSITIONWINDOW  0x000C
#define CFS_DEFAULT               0x0000
#define CFS_RECT                  0x0001
#define CFS_POINT                 0x0002
#define CFS_FORCE_POSITION        0x0020
#define IMN_SETOPENSTATUS         0x0008
#define IME_CMODE_NATIVE 0x0001
#define IME_CMODE_ALPHANUMERIC 0x0000
#define IME_SMODE_NONE   0x0000
#define IME_SMODE_AUTOMATIC 0x0008
#define GCS_COMPSTR       0x0008

typedef struct tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *LPCOMPOSITIONFORM;

inline HIMC ImmGetContext(HWND) { return nullptr; }
inline HWND ImmGetDefaultIMEWnd(HWND hWnd) { return hWnd; }
inline BOOL ImmReleaseContext(HWND, HIMC) { return TRUE; }
inline BOOL ImmGetCompositionWindow(HIMC, COMPOSITIONFORM* compositionForm)
{
    if (!compositionForm)
    {
        return FALSE;
    }

    compositionForm->dwStyle = CFS_DEFAULT;
    compositionForm->ptCurrentPos.x = 0;
    compositionForm->ptCurrentPos.y = 0;
    SetRect(&compositionForm->rcArea, 0, 0, 0, 0);
    return TRUE;
}
inline BOOL ImmSetCompositionWindow(HIMC, const COMPOSITIONFORM*) { return TRUE; }
inline BOOL ImmGetConversionStatus(HIMC, DWORD* lpfdwConversion, DWORD* lpfdwSentence)
{
    if (lpfdwConversion) *lpfdwConversion = 0;
    if (lpfdwSentence) *lpfdwSentence = 0;
    return TRUE;
}
inline BOOL ImmSetConversionStatus(HIMC, DWORD, DWORD) { return TRUE; }
inline LONG ImmGetCompositionString(HIMC, DWORD, LPVOID lpBuf, DWORD dwBufLen)
{
    if (lpBuf && dwBufLen > 0)
    {
        ((char*)lpBuf)[0] = '\0';
    }
    return 0;
}

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

inline int _muVsntprintf(char* buffer, size_t sizeOfBuffer, size_t count, const char* format, va_list args)
{
    if (!buffer || sizeOfBuffer == 0 || !format)
    {
        return -1;
    }

    size_t limit = sizeOfBuffer;
    if (count != _TRUNCATE && count < limit)
    {
        limit = count + 1;
    }

    int written = vsnprintf(buffer, limit, format, args);
    if (written < 0)
    {
        buffer[0] = '\0';
        return -1;
    }

    buffer[sizeOfBuffer - 1] = '\0';
    return written;
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
#ifdef _itoa_s
#  undef _itoa_s
#endif
#define _itoa_s(val, buf, sz, base) (snprintf((buf),(sz),((base)==16?"%x":"%d"),(val)),(void)0)
#ifdef _ultoa_s
#  undef _ultoa_s
#endif
#define _ultoa_s(...) _muUltoaS(__VA_ARGS__)
#ifdef _vsntprintf
#  undef _vsntprintf
#endif
#define _vsntprintf(buf, cnt, fmt, args) _muVsntprintf((buf), _muObjectSize(buf), (cnt), (fmt), (args))
// wsprintf (Win32 ANSI sprintf, no size arg) → snprintf with large buffer size
#ifndef wsprintf
#  define wsprintf(buf, fmt, ...) snprintf((buf), 4096, (fmt), ##__VA_ARGS__)
#endif
#ifndef wvsprintf
#  define wvsprintf(buf, fmt, args) vsprintf((buf), (fmt), (args))
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
#define INVALID_FILE_ATTRIBUTES   0xFFFFFFFF

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
    HANDLE h = (HANDLE)f;
    {
        std::lock_guard<std::mutex> lock(AndroidCompatFileHandleMutex());
        AndroidCompatFileHandleSet().insert(h);
    }
    return h;
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
    if (!h || h == INVALID_HANDLE_VALUE)
    {
        return TRUE;
    }

    {
        std::lock_guard<std::mutex> lock(AndroidCompatFileHandleMutex());
        auto& fileHandles = AndroidCompatFileHandleSet();
        auto it = fileHandles.find(h);
        if (it != fileHandles.end())
        {
            fclose((FILE*)h);
            fileHandles.erase(it);
        }
    }
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

// WinINet compatibility layer used by legacy download modules.
// Android implementation supports the HTTP path used by HTTPConnecter.
enum AndroidInternetHandleType
{
    ANDROID_INET_SESSION = 1,
    ANDROID_INET_CONNECTION = 2,
    ANDROID_INET_REQUEST = 3,
};

struct AndroidInternetBaseHandle
{
    AndroidInternetHandleType type;
    virtual ~AndroidInternetBaseHandle() = default;
};

struct AndroidInternetSessionHandle : AndroidInternetBaseHandle
{
    std::string agent;
    AndroidInternetSessionHandle() { type = ANDROID_INET_SESSION; }
};

struct AndroidInternetConnectionHandle : AndroidInternetBaseHandle
{
    std::string host;
    INTERNET_PORT port;
    AndroidInternetConnectionHandle() : port(INTERNET_DEFAULT_HTTP_PORT)
    {
        type = ANDROID_INET_CONNECTION;
    }
};

struct AndroidInternetRequestHandle : AndroidInternetBaseHandle
{
    std::string method;
    std::string host;
    INTERNET_PORT port;
    std::string path;
    int statusCode;
    size_t contentLength;
    std::vector<BYTE> body;
    size_t readOffset;
    bool sent;

    AndroidInternetRequestHandle()
        : port(INTERNET_DEFAULT_HTTP_PORT),
          statusCode(0),
          contentLength(0),
          readOffset(0),
          sent(false)
    {
        type = ANDROID_INET_REQUEST;
    }
};

static inline std::string AndroidHttpTrim(const std::string& value)
{
    size_t begin = 0;
    while (begin < value.size() && (value[begin] == ' ' || value[begin] == '\t' || value[begin] == '\r' || value[begin] == '\n'))
    {
        ++begin;
    }

    size_t end = value.size();
    while (end > begin && (value[end - 1] == ' ' || value[end - 1] == '\t' || value[end - 1] == '\r' || value[end - 1] == '\n'))
    {
        --end;
    }

    return value.substr(begin, end - begin);
}

static inline std::string AndroidHttpLower(const std::string& value)
{
    std::string out = value;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return out;
}

static inline std::vector<BYTE> AndroidHttpDecodeChunked(const BYTE* data, size_t size)
{
    std::vector<BYTE> decoded;
    size_t cursor = 0;

    while (cursor < size)
    {
        size_t lineEnd = cursor;
        while (lineEnd + 1 < size && !(data[lineEnd] == '\r' && data[lineEnd + 1] == '\n'))
        {
            ++lineEnd;
        }
        if (lineEnd + 1 >= size)
        {
            return std::vector<BYTE>();
        }

        std::string sizeLine((const char*)data + cursor, lineEnd - cursor);
        size_t semicolon = sizeLine.find(';');
        if (semicolon != std::string::npos)
        {
            sizeLine = sizeLine.substr(0, semicolon);
        }
        sizeLine = AndroidHttpTrim(sizeLine);
        if (sizeLine.empty())
        {
            return std::vector<BYTE>();
        }

        char* endPtr = nullptr;
        unsigned long chunkSize = std::strtoul(sizeLine.c_str(), &endPtr, 16);
        if (!endPtr || *endPtr != '\0')
        {
            return std::vector<BYTE>();
        }

        cursor = lineEnd + 2;
        if (chunkSize == 0)
        {
            return decoded;
        }

        if (cursor + chunkSize + 2 > size)
        {
            return std::vector<BYTE>();
        }

        decoded.insert(decoded.end(), data + cursor, data + cursor + chunkSize);
        cursor += chunkSize;

        if (!(data[cursor] == '\r' && data[cursor + 1] == '\n'))
        {
            return std::vector<BYTE>();
        }
        cursor += 2;
    }

    return std::vector<BYTE>();
}

static inline bool AndroidHttpFetch(const std::string& host,
                                    INTERNET_PORT port,
                                    const std::string& method,
                                    const std::string& path,
                                    int& outStatusCode,
                                    size_t& outContentLength,
                                    std::vector<BYTE>& outBody)
{
    outStatusCode = 0;
    outContentLength = 0;
    outBody.clear();

    if (host.empty())
    {
        errno = EINVAL;
        return false;
    }

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char portStr[16];
    std::snprintf(portStr, sizeof(portStr), "%u", (unsigned)port);

    struct addrinfo* result = nullptr;
    if (getaddrinfo(host.c_str(), portStr, &hints, &result) != 0 || !result)
    {
        return false;
    }

    int sock = -1;
    for (struct addrinfo* it = result; it; it = it->ai_next)
    {
        sock = (int)socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (sock < 0)
        {
            continue;
        }

        if (connect(sock, it->ai_addr, it->ai_addrlen) == 0)
        {
            break;
        }

        close(sock);
        sock = -1;
    }
    freeaddrinfo(result);

    if (sock < 0)
    {
        return false;
    }

    const std::string reqPath = path.empty() ? "/" : path;
    std::string request =
        method + " " + reqPath + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: close\r\n" +
        "User-Agent: MuAndroid/1.0\r\n" +
        "Accept: */*\r\n\r\n";

    size_t sent = 0;
    while (sent < request.size())
    {
        ssize_t n = send(sock, request.data() + sent, request.size() - sent, 0);
        if (n <= 0)
        {
            close(sock);
            return false;
        }
        sent += (size_t)n;
    }

    std::vector<BYTE> response;
    BYTE chunk[8192];
    for (;;)
    {
        ssize_t n = recv(sock, chunk, sizeof(chunk), 0);
        if (n < 0)
        {
            close(sock);
            return false;
        }
        if (n == 0)
        {
            break;
        }
        response.insert(response.end(), chunk, chunk + n);
    }
    close(sock);

    if (response.empty())
    {
        errno = EIO;
        return false;
    }

    size_t headerEnd = std::string::npos;
    for (size_t i = 0; i + 3 < response.size(); ++i)
    {
        if (response[i] == '\r' && response[i + 1] == '\n' &&
            response[i + 2] == '\r' && response[i + 3] == '\n')
        {
            headerEnd = i;
            break;
        }
    }
    if (headerEnd == std::string::npos)
    {
        errno = EPROTO;
        return false;
    }

    std::string headers((const char*)response.data(), headerEnd);
    size_t firstLineEnd = headers.find("\r\n");
    std::string statusLine = (firstLineEnd == std::string::npos) ? headers : headers.substr(0, firstLineEnd);

    size_t firstSpace = statusLine.find(' ');
    size_t secondSpace = (firstSpace == std::string::npos) ? std::string::npos : statusLine.find(' ', firstSpace + 1);
    if (firstSpace != std::string::npos)
    {
        std::string code = (secondSpace == std::string::npos)
            ? statusLine.substr(firstSpace + 1)
            : statusLine.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        outStatusCode = std::atoi(code.c_str());
    }
    if (outStatusCode <= 0)
    {
        outStatusCode = 500;
    }

    bool chunked = false;
    size_t cursor = (firstLineEnd == std::string::npos) ? headers.size() : firstLineEnd + 2;
    while (cursor < headers.size())
    {
        size_t lineEnd = headers.find("\r\n", cursor);
        std::string line = (lineEnd == std::string::npos) ? headers.substr(cursor) : headers.substr(cursor, lineEnd - cursor);
        cursor = (lineEnd == std::string::npos) ? headers.size() : lineEnd + 2;

        if (line.empty())
        {
            continue;
        }

        size_t sep = line.find(':');
        if (sep == std::string::npos)
        {
            continue;
        }

        std::string name = AndroidHttpLower(AndroidHttpTrim(line.substr(0, sep)));
        std::string value = AndroidHttpTrim(line.substr(sep + 1));

        if (name == "content-length")
        {
            outContentLength = (size_t)std::strtoull(value.c_str(), nullptr, 10);
        }
        else if (name == "transfer-encoding")
        {
            std::string lowerValue = AndroidHttpLower(value);
            if (lowerValue.find("chunked") != std::string::npos)
            {
                chunked = true;
            }
        }
    }

    const BYTE* bodyStart = response.data() + headerEnd + 4;
    size_t bodySize = response.size() - (headerEnd + 4);
    if (chunked)
    {
        std::vector<BYTE> decoded = AndroidHttpDecodeChunked(bodyStart, bodySize);
        if (!decoded.empty())
        {
            outBody.swap(decoded);
            outContentLength = outBody.size();
            return true;
        }
    }

    outBody.assign(bodyStart, bodyStart + bodySize);
    if (outContentLength == 0)
    {
        outContentLength = outBody.size();
    }
    return true;
}

static inline HINTERNET InternetOpen(const char* agent, DWORD, const char*, const char*, DWORD)
{
    AndroidInternetSessionHandle* h = new AndroidInternetSessionHandle();
    h->agent = (agent ? agent : "");
    return reinterpret_cast<HINTERNET>(h);
}

static inline HINTERNET InternetConnectA(HINTERNET hSession, const char* serverName, INTERNET_PORT port, const char*,
                                         const char*, DWORD, DWORD, DWORD_PTR)
{
    AndroidInternetBaseHandle* base = reinterpret_cast<AndroidInternetBaseHandle*>(hSession);
    if (!base || base->type != ANDROID_INET_SESSION || !serverName)
    {
        errno = EINVAL;
        return nullptr;
    }

    AndroidInternetConnectionHandle* h = new AndroidInternetConnectionHandle();
    h->host = serverName;
    h->port = (port != 0 ? port : INTERNET_DEFAULT_HTTP_PORT);
    return reinterpret_cast<HINTERNET>(h);
}

static inline HINTERNET FtpFindFirstFileA(HINTERNET, const char*, WIN32_FIND_DATAA* findData, DWORD, DWORD_PTR)
{
    if (findData)
    {
        std::memset(findData, 0, sizeof(*findData));
    }
    errno = ENOTSUP;
    return nullptr;
}

static inline HINTERNET FtpOpenFileA(HINTERNET, const char*, DWORD, DWORD, DWORD_PTR)
{
    errno = ENOTSUP;
    return nullptr;
}

static inline HINTERNET HttpOpenRequest(HINTERNET hConnection, const char* method, const char* objectName, const char*, const char*,
                                        const char* const*, DWORD, DWORD_PTR)
{
    AndroidInternetBaseHandle* base = reinterpret_cast<AndroidInternetBaseHandle*>(hConnection);
    if (!base || base->type != ANDROID_INET_CONNECTION)
    {
        errno = EINVAL;
        return nullptr;
    }

    AndroidInternetConnectionHandle* conn = reinterpret_cast<AndroidInternetConnectionHandle*>(base);
    AndroidInternetRequestHandle* req = new AndroidInternetRequestHandle();
    req->method = (method && method[0]) ? method : "GET";
    req->host = conn->host;
    req->port = conn->port;

    std::string path = (objectName ? objectName : "/");
    size_t schemePos = path.find("://");
    if (schemePos != std::string::npos)
    {
        size_t slashPos = path.find('/', schemePos + 3);
        path = (slashPos == std::string::npos) ? "/" : path.substr(slashPos);
    }
    if (path.empty() || path[0] != '/')
    {
        path = "/" + path;
    }
    req->path = path;
    return reinterpret_cast<HINTERNET>(req);
}

static inline BOOL HttpSendRequest(HINTERNET hRequest, const char*, DWORD, LPVOID, DWORD)
{
    AndroidInternetBaseHandle* base = reinterpret_cast<AndroidInternetBaseHandle*>(hRequest);
    if (!base || base->type != ANDROID_INET_REQUEST)
    {
        errno = EINVAL;
        return FALSE;
    }

    AndroidInternetRequestHandle* req = reinterpret_cast<AndroidInternetRequestHandle*>(base);
    req->body.clear();
    req->readOffset = 0;
    req->contentLength = 0;
    req->statusCode = 0;

    if (!AndroidHttpFetch(req->host, req->port, req->method, req->path, req->statusCode, req->contentLength, req->body))
    {
        return FALSE;
    }

    req->sent = true;
    return TRUE;
}

static inline BOOL HttpQueryInfo(HINTERNET hRequest, DWORD infoLevel, LPVOID buffer, LPDWORD bufferLength, LPDWORD)
{
    AndroidInternetBaseHandle* base = reinterpret_cast<AndroidInternetBaseHandle*>(hRequest);
    if (!base || base->type != ANDROID_INET_REQUEST || !buffer || !bufferLength || *bufferLength == 0)
    {
        errno = EINVAL;
        return FALSE;
    }

    AndroidInternetRequestHandle* req = reinterpret_cast<AndroidInternetRequestHandle*>(base);
    const DWORD query = (infoLevel & 0xFFFFu);
    char out[64];
    out[0] = '\0';

    if (query == HTTP_QUERY_STATUS_CODE)
    {
        std::snprintf(out, sizeof(out), "%d", req->statusCode > 0 ? req->statusCode : 500);
    }
    else if (query == HTTP_QUERY_CONTENT_LENGTH)
    {
        std::snprintf(out, sizeof(out), "%llu", (unsigned long long)req->contentLength);
    }
    else
    {
        errno = ENOTSUP;
        return FALSE;
    }

    std::snprintf((char*)buffer, *bufferLength, "%s", out);
    return TRUE;
}

static inline BOOL InternetQueryDataAvailable(HINTERNET hRequest, LPDWORD available, DWORD, DWORD_PTR)
{
    AndroidInternetBaseHandle* base = reinterpret_cast<AndroidInternetBaseHandle*>(hRequest);
    if (!base || base->type != ANDROID_INET_REQUEST)
    {
        errno = EINVAL;
        return FALSE;
    }

    AndroidInternetRequestHandle* req = reinterpret_cast<AndroidInternetRequestHandle*>(base);
    size_t remaining = (req->readOffset < req->body.size()) ? (req->body.size() - req->readOffset) : 0;
    if (available)
    {
        *available = (DWORD)remaining;
    }
    return TRUE;
}

static inline BOOL InternetReadFile(HINTERNET hRequest, LPVOID outBuffer, DWORD bytesToRead, LPDWORD bytesRead)
{
    AndroidInternetBaseHandle* base = reinterpret_cast<AndroidInternetBaseHandle*>(hRequest);
    if (!base || base->type != ANDROID_INET_REQUEST || !outBuffer)
    {
        errno = EINVAL;
        return FALSE;
    }

    AndroidInternetRequestHandle* req = reinterpret_cast<AndroidInternetRequestHandle*>(base);
    size_t remaining = (req->readOffset < req->body.size()) ? (req->body.size() - req->readOffset) : 0;
    size_t toCopy = std::min<size_t>((size_t)bytesToRead, remaining);
    if (toCopy > 0)
    {
        std::memcpy(outBuffer, req->body.data() + req->readOffset, toCopy);
        req->readOffset += toCopy;
    }

    if (bytesRead)
    {
        *bytesRead = (DWORD)toCopy;
    }
    return TRUE;
}

static inline BOOL InternetCloseHandle(HINTERNET hInternet)
{
    if (!hInternet)
    {
        return TRUE;
    }

    AndroidInternetBaseHandle* base = reinterpret_cast<AndroidInternetBaseHandle*>(hInternet);
    delete base;
    return TRUE;
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

#define GetPrivateProfileIntA      GetPrivateProfileInt
#define GetPrivateProfileStringA   GetPrivateProfileString
#define WritePrivateProfileStringA WritePrivateProfileString
