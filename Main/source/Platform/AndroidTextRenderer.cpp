#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// AndroidTextRenderer.cpp
// Uses stb_truetype (from ImGui dependencies) to rasterize text into
// off-screen pixel buffers, then uploads to GL textures.
// ─────────────────────────────────────────────────────────────────────────────
#include "AndroidTextRenderer.h"
#include <GLES3/gl3.h>
#include <android/log.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <wchar.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "imstb_truetype.h"  // from Main/dependencies/ImGui/imstb_truetype.h

#define LOG_TAG "MUAndroid"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ─────────────────────────────────────────────────────────────────────────────
// Font cache
// ─────────────────────────────────────────────────────────────────────────────
struct FontEntry
{
    stbtt_fontinfo info;
    std::vector<uint8_t> data;
    float scale;
    int ascent, descent, lineGap;
};

struct DCState
{
    void*  bits    = nullptr;  // pixel buffer (BGRA)
    int    width   = 0;
    int    height  = 0;
    int    fontSz  = 16;
    bool   bold    = false;
};

static std::vector<uint8_t> s_regularFontData;
static std::vector<uint8_t> s_boldFontData;
static stbtt_fontinfo       s_regularFont;
static stbtt_fontinfo       s_boldFont;
static bool                 s_initialized = false;

// hdc → DCState (hdc is just a small integer index cast to void*)
static std::unordered_map<uintptr_t, DCState> s_dcMap;

static const char* k_regularFontPaths[] = {
    "/system/fonts/NotoSans-Regular.ttf",
    "/system/fonts/Roboto-Regular.ttf",
    "/system/fonts/DroidSans.ttf",
    nullptr
};
static const char* k_boldFontPaths[] = {
    "/system/fonts/NotoSans-Bold.ttf",
    "/system/fonts/Roboto-Bold.ttf",
    "/system/fonts/DroidSans-Bold.ttf",
    nullptr
};

static bool LoadFont(const char* paths[], std::vector<uint8_t>& outData, stbtt_fontinfo& outInfo)
{
    for (int i = 0; paths[i]; i++)
    {
        FILE* f = fopen(paths[i], "rb");
        if (!f) continue;
        fseek(f, 0, SEEK_END);
        size_t sz = ftell(f); rewind(f);
        outData.resize(sz);
        fread(outData.data(), 1, sz, f);
        fclose(f);
        if (stbtt_InitFont(&outInfo, outData.data(), 0))
        {
            LOGI("Font loaded: %s", paths[i]);
            return true;
        }
        outData.clear();
    }
    return false;
}

namespace AndroidTextRenderer
{

bool Init()
{
    bool ok = true;
    if (!LoadFont(k_regularFontPaths, s_regularFontData, s_regularFont))
    {
        LOGE("Failed to load regular font");
        ok = false;
    }
    LoadFont(k_boldFontPaths, s_boldFontData, s_boldFont);
    s_initialized = ok;
    return ok;
}

void Shutdown()
{
    s_dcMap.clear();
    s_regularFontData.clear();
    s_boldFontData.clear();
    s_initialized = false;
}

HFONT CreateFont(int heightPx, bool bold)
{
    // Encode size + bold into the handle value (no allocation needed)
    uintptr_t handle = ((uintptr_t)(unsigned)heightPx << 1) | (bold ? 1 : 0);
    return (HFONT)(void*)handle;
}

void SetDCFont(HDC hdc, HFONT font)
{
    uintptr_t key = (uintptr_t)hdc;
    uintptr_t h   = (uintptr_t)font;
    s_dcMap[key].fontSz = (int)(h >> 1);
    s_dcMap[key].bold   = (h & 1) != 0;
}

void SetDCBitmap(HDC hdc, HBITMAP /*hbm*/, int width, int height, void* bits)
{
    uintptr_t key = (uintptr_t)hdc;
    s_dcMap[key].bits   = bits;
    s_dcMap[key].width  = width;
    s_dcMap[key].height = height;
}

void* GetDCBits(HDC hdc)
{
    auto it = s_dcMap.find((uintptr_t)hdc);
    return (it != s_dcMap.end()) ? it->second.bits : nullptr;
}

// Render a single Unicode codepoint into the DC bitmap at (x,y)
// Returns the advance width in pixels.
static int RenderGlyph(DCState& dc, stbtt_fontinfo& font, int codepoint, int px, int x, int y)
{
    if (!dc.bits || dc.width <= 0) return 0;

    float scale = stbtt_ScaleForPixelHeight(&font, (float)px);
    int advW, lsb, x0, y0, x1, y1;
    stbtt_GetCodepointHMetrics(&font, codepoint, &advW, &lsb);
    stbtt_GetCodepointBitmapBox(&font, codepoint, scale, scale, &x0, &y0, &x1, &y1);

    int gw = x1 - x0, gh = y1 - y0;
    if (gw <= 0 || gh <= 0) return (int)(advW * scale);

    std::vector<uint8_t> bitmap(gw * gh, 0);
    stbtt_MakeCodepointBitmap(&font, bitmap.data(), gw, gh, gw, scale, scale, codepoint);

    // Blit into DC bitmap (BGRA format, 4 bytes per pixel)
    uint8_t* dst = (uint8_t*)dc.bits;
    int ascent; int descent; int lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    int baseline = y + (int)(ascent * scale);

    for (int gy = 0; gy < gh; gy++)
    {
        int py = baseline + y0 + gy;
        if (py < 0 || py >= dc.height) continue;
        for (int gx = 0; gx < gw; gx++)
        {
            int px2 = x + x0 + gx;
            if (px2 < 0 || px2 >= dc.width) continue;
            uint8_t alpha = bitmap[gy * gw + gx];
            if (alpha < 80) continue;

            int idx = (py * dc.width + px2) * 4;
            // White text with alpha
            dst[idx + 0] = 255; // B
            dst[idx + 1] = 255; // G
            dst[idx + 2] = 255; // R
            dst[idx + 3] = alpha;
        }
    }
    return (int)(advW * scale);
}

void TextOut(HDC hdc, int x, int y, const char* str, int len)
{
    if (!s_initialized || !str || len <= 0) return;
    auto it = s_dcMap.find((uintptr_t)hdc);
    if (it == s_dcMap.end()) return;
    DCState& dc = it->second;

    stbtt_fontinfo& font = (dc.bold && !s_boldFontData.empty()) ? s_boldFont : s_regularFont;
    int cx = x;
    for (int i = 0; i < len; i++)
        cx += RenderGlyph(dc, font, (unsigned char)str[i], dc.fontSz, cx, y);
}

void TextOutW(HDC hdc, int x, int y, const wchar_t* str, int len)
{
    if (!s_initialized || !str || len <= 0) return;
    auto it = s_dcMap.find((uintptr_t)hdc);
    if (it == s_dcMap.end()) return;
    DCState& dc = it->second;

    stbtt_fontinfo& font = (dc.bold && !s_boldFontData.empty()) ? s_boldFont : s_regularFont;
    int cx = x;
    for (int i = 0; i < len; i++)
        cx += RenderGlyph(dc, font, (int)str[i], dc.fontSz, cx, y);
}

bool GetTextExtentPoint32(HDC hdc, const char* str, int len, SIZE* sz)
{
    if (!s_initialized || !str || !sz) return false;
    auto it = s_dcMap.find((uintptr_t)hdc);
    int fontSz = (it != s_dcMap.end()) ? it->second.fontSz : 16;
    stbtt_fontinfo& font = s_regularFont;
    float scale = stbtt_ScaleForPixelHeight(&font, (float)fontSz);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

    int w = 0;
    for (int i = 0; i < len; i++)
    {
        int advW, lsb;
        stbtt_GetCodepointHMetrics(&font, (unsigned char)str[i], &advW, &lsb);
        w += (int)(advW * scale);
    }
    sz->cx = w;
    sz->cy = (int)((ascent - descent) * scale);
    return true;
}

bool GetTextExtentPoint32W(HDC hdc, const wchar_t* str, int len, SIZE* sz)
{
    if (!s_initialized || !str || !sz) return false;
    auto it = s_dcMap.find((uintptr_t)hdc);
    int fontSz = (it != s_dcMap.end()) ? it->second.fontSz : 16;
    stbtt_fontinfo& font = s_regularFont;
    float scale = stbtt_ScaleForPixelHeight(&font, (float)fontSz);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

    int w = 0;
    for (int i = 0; i < len; i++)
    {
        int advW, lsb;
        stbtt_GetCodepointHMetrics(&font, (int)str[i], &advW, &lsb);
        w += (int)(advW * scale);
    }
    sz->cx = w;
    sz->cy = (int)((ascent - descent) * scale);
    return true;
}

unsigned int UploadTextBitmap(HDC hdc, int width, int height)
{
    auto it = s_dcMap.find((uintptr_t)hdc);
    if (it == s_dcMap.end() || !it->second.bits) return 0;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, it->second.bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

} // namespace AndroidTextRenderer

#endif // __ANDROID__
