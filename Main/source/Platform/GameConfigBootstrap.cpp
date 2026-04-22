// ─────────────────────────────────────────────────────────────────────────────
// GameConfigBootstrap.cpp
// Calls the existing CGMProtect::runtime_load_protect() that already handles
// config loading on Windows. On Android we just need to ensure the asset path
// is set before calling it.
// ─────────────────────────────────────────────────────────────────────────────
#include "GameConfigBootstrap.h"

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "MUAndroid"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

// Declared in CGMProtect.h
extern bool runtime_load_protect();

namespace GameConfigBootstrap
{
    bool Load()
    {
        bool ok = runtime_load_protect();
#ifdef __ANDROID__
        if (!ok) LOGE("Config load FAILED");
#endif
        return ok;
    }
}
