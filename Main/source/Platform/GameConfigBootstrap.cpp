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
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

// Declared in CGMProtect.h
extern bool runtime_load_protect();

namespace GameConfigBootstrap
{
    bool Load()
    {
#ifdef __ANDROID__
        LOGI("Loading config (CProtect/Configs.xtm)...");
#endif
        bool ok = runtime_load_protect();
#ifdef __ANDROID__
        if (ok) LOGI("Config loaded OK");
        else    LOGE("Config load FAILED");
#endif
        return ok;
    }
}
