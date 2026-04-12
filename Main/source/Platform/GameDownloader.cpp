#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// GameDownloader.cpp
// Downloads game data on first run. Uses ASIO (already a dependency) for HTTP.
// Reutilizes GameShop/FileDownloader/HTTPConnecter.cpp for the transport layer.
// ─────────────────────────────────────────────────────────────────────────────
#include "GameDownloader.h"
#include "GameAssetPath.h"
#include "AndroidEglWindow.h"
#include <GLES3/gl3.h>
#include <android/log.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>

// CRC32 from Util/
#include "../../../Util/CCRC32.H"

#define LOG_TAG "MUAssets"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ─────────────────────────────────────────────────────────────────────────────
// Files that must be present for the game to run (relative to data dir)
// ─────────────────────────────────────────────────────────────────────────────
static const char* k_requiredFiles[] = {
    "Data/av-code45.pak",
    "Data/Local/Connect.msil",
    nullptr
};

static std::string s_serverURL = "http://127.0.0.1/mu_assets"; // override via SetServerURL

namespace GameDownloader
{

void SetServerURL(const char* url) { if (url) s_serverURL = url; }
const char* GetServerURL()         { return s_serverURL.c_str(); }

bool IsDataReady()
{
    for (int i = 0; k_requiredFiles[i]; i++)
    {
        std::string full = GameAssetPath::Resolve(k_requiredFiles[i]);
        struct stat st;
        if (stat(full.c_str(), &st) != 0 || st.st_size == 0)
        {
            LOGI("Missing: %s", full.c_str());
            return false;
        }
    }
    return true;
}

// ── Simple progress bar rendered via GLES3 ───────────────────────────────────
static void RenderProgress(AndroidEglWindow* win, float progress, const char* label)
{
    if (!win) return;
    glViewport(0, 0, win->GetWidth(), win->GetHeight());
    glClearColor(0.05f, 0.05f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: draw actual progress bar with text using AndroidTextRenderer
    // For now just clear + swap (shows a dark blue screen during download)
    (void)progress; (void)label;

    win->SwapBuffers();
}

// ── HTTP download helper (basic implementation) ───────────────────────────────
// For a full implementation this would use HTTPConnecter.cpp from GameShop.
// Here we provide a minimal implementation using POSIX sockets.
static bool DownloadFile(const std::string& url, const std::string& destPath)
{
    LOGI("Downloading: %s → %s", url.c_str(), destPath.c_str());

    // Ensure directory exists
    std::string dir = destPath.substr(0, destPath.rfind('/'));
    if (!dir.empty())
    {
        // Create directory tree
        std::string tmp;
        for (char c : dir + '/')
        {
            tmp += c;
            if (c == '/') mkdir(tmp.c_str(), 0755);
        }
    }

    // TODO: implement HTTP download via ASIO HTTPConnecter or libcurl
    // For now this is a placeholder that returns false (download not yet implemented)
    // The user must implement this or provide game data manually.
    LOGE("HTTP downloader not yet implemented. Place game data manually in: %s",
         GameAssetPath::GetBase().c_str());
    return false;
}

bool DownloadAll(AndroidEglWindow* eglWindow)
{
    LOGI("Starting asset download from: %s", s_serverURL.c_str());

    int total   = 0;
    int current = 0;

    // Count required files
    for (int i = 0; k_requiredFiles[i]; i++) total++;

    for (int i = 0; k_requiredFiles[i]; i++)
    {
        const char* rel  = k_requiredFiles[i];
        std::string full = GameAssetPath::Resolve(rel);
        std::string url  = s_serverURL + '/' + rel;

        RenderProgress(eglWindow, (float)current / total, rel);

        struct stat st;
        if (stat(full.c_str(), &st) == 0 && st.st_size > 0)
        {
            LOGI("Already have: %s", rel);
            current++;
            continue;
        }

        if (!DownloadFile(url, full))
        {
            LOGE("Failed to download: %s", rel);
            return false;
        }
        current++;
    }

    RenderProgress(eglWindow, 1.f, "Done");
    LOGI("All assets ready");
    return true;
}

} // namespace GameDownloader

#endif // __ANDROID__
