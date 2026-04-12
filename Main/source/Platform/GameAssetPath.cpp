// ─────────────────────────────────────────────────────────────────────────────
// GameAssetPath.cpp
// ─────────────────────────────────────────────────────────────────────────────
#include "GameAssetPath.h"
#include <string>
#include <algorithm>
#include <cctype>

#ifdef __ANDROID__
#include <dirent.h>
#include <sys/stat.h>
#include <android/log.h>
#define LOG_TAG "MUAssets"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#endif

namespace GameAssetPath
{
    static std::string s_basePath;

    void Init(const std::string& basePath)
    {
        s_basePath = basePath;
        if (!s_basePath.empty() && s_basePath.back() != '/')
            s_basePath += '/';
#ifdef __ANDROID__
        LOGI("Asset base: %s", s_basePath.c_str());
#endif
    }

    const std::string& GetBase() { return s_basePath; }

    std::string Resolve(const char* relativePath)
    {
        if (!relativePath) return s_basePath;

        // Normalize: replace backslashes with forward slashes
        std::string rel(relativePath);
        std::replace(rel.begin(), rel.end(), '\\', '/');

        // If already absolute, return as-is
        if (rel[0] == '/') return rel;

        return s_basePath + rel;
    }

#ifdef __ANDROID__
    // Case-insensitive fopen for ext4 (Android filesystem is case-sensitive).
    // Walks each path component and does a case-insensitive match.
    static FILE* fopen_ci(const std::string& fullPath, const char* mode)
    {
        // Try exact path first (fast path)
        FILE* f = fopen(fullPath.c_str(), mode);
        if (f) return f;

        // Split into directory + filename
        size_t slash = fullPath.rfind('/');
        if (slash == std::string::npos) return nullptr;

        std::string dir  = fullPath.substr(0, slash);
        std::string file = fullPath.substr(slash + 1);

        // Case-insensitive search in the directory
        DIR* d = opendir(dir.c_str());
        if (!d)
        {
            // Try case-insensitive on the directory itself (recursive would be heavy)
            return nullptr;
        }

        std::string match;
        struct dirent* entry;
        while ((entry = readdir(d)) != nullptr)
        {
            if (strcasecmp(entry->d_name, file.c_str()) == 0)
            {
                match = dir + '/' + entry->d_name;
                break;
            }
        }
        closedir(d);

        if (!match.empty())
            return fopen(match.c_str(), mode);

        LOGW("fopen_ci: not found: %s", fullPath.c_str());
        return nullptr;
    }

    FILE* OpenFile(const char* relativePath, const char* mode)
    {
        std::string full = Resolve(relativePath);
        return fopen_ci(full, mode);
    }

#else
    FILE* OpenFile(const char* relativePath, const char* mode)
    {
        return fopen(relativePath, mode);
    }
#endif

} // namespace GameAssetPath
