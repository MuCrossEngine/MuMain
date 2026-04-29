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
    // Walks every path component from the base directory case-insensitively,
    // so paths like "Data/World1/EncTerrain1.map" find "Data/world1/encterrain1.map".
    static FILE* fopen_ci(const std::string& fullPath, const char* mode)
    {
        // Try exact path first (fast path)
        FILE* f = fopen(fullPath.c_str(), mode);
        if (f) return f;

        // Must be under the base path to do the component walk.
        if (s_basePath.empty() || fullPath.size() <= s_basePath.size() ||
            fullPath.substr(0, s_basePath.size()) != s_basePath)
        {
            LOGW("fopen_ci: not found: %s", fullPath.c_str());
            return nullptr;
        }

        // rel = path relative to base, e.g. "Data/World1/EncTerrain1.map"
        std::string rel = fullPath.substr(s_basePath.size());

        // current accumulates the resolved absolute path component by component.
        // s_basePath already ends with '/', so strip the trailing slash for joining.
        std::string current = s_basePath;
        if (!current.empty() && current.back() == '/')
            current.pop_back();

        while (!rel.empty())
        {
            size_t slash = rel.find('/');
            std::string component;
            if (slash == std::string::npos)
            {
                component = rel;
                rel.clear();
            }
            else
            {
                component = rel.substr(0, slash);
                rel = rel.substr(slash + 1);
            }
            if (component.empty()) continue;

            // Try exact match first (no opendir needed)
            std::string exact = current + '/' + component;
            struct stat st;
            if (stat(exact.c_str(), &st) == 0)
            {
                current = exact;
                continue;
            }

            // Case-insensitive scan of the current directory
            DIR* d = opendir(current.c_str());
            if (!d)
            {
                LOGW("fopen_ci: can't open dir: %s", current.c_str());
                return nullptr;
            }
            std::string match;
            struct dirent* entry;
            while ((entry = readdir(d)) != nullptr)
            {
                if (strcasecmp(entry->d_name, component.c_str()) == 0)
                {
                    match = current + '/' + entry->d_name;
                    break;
                }
            }
            closedir(d);

            if (match.empty())
            {
                LOGW("fopen_ci: component not found: '%s' in '%s'", component.c_str(), current.c_str());
                return nullptr;
            }
            current = match;
        }

        return fopen(current.c_str(), mode);
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
