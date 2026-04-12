#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// GameAssetPath.h
// Cross-platform asset path resolution.
// On Windows: paths are relative to the executable directory (current dir).
// On Android: paths are under getExternalFilesDir() set at startup.
// ─────────────────────────────────────────────────────────────────────────────
#include <string>
#include <cstdio>

namespace GameAssetPath
{
    // Call once at startup with the external files directory path.
    void Init(const std::string& basePath);

    // Returns the full path to a game file, e.g. "Data/Player/Player.bmd"
    std::string Resolve(const char* relativePath);

    // Returns base data directory (e.g. "/sdcard/Android/data/com.mu/files/")
    const std::string& GetBase();

    // fopen wrapper: resolves path and opens the file.
    // On Android also handles case-insensitive lookup (ext4 is case-sensitive).
    FILE* OpenFile(const char* relativePath, const char* mode);
}

// Convenience macro used throughout the game source.
// On Windows the game just calls fopen("Data/...", ...) directly, which works
// because the CWD is the game folder.
// On Android we intercept via this macro.
#ifdef __ANDROID__
#  define MU_FOPEN(path, mode) GameAssetPath::OpenFile(path, mode)
#else
#  define MU_FOPEN(path, mode) fopen(path, mode)
#endif
