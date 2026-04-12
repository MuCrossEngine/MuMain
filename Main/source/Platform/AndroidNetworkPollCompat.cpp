#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// AndroidNetworkPollCompat.cpp
// Non-blocking socket poll for Android — replaces WSAAsyncSelect model.
// ─────────────────────────────────────────────────────────────────────────────
#include "AndroidNetworkPollCompat.h"
#include <android/log.h>
#define LOG_TAG "MUNetwork"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void PollSocketIO()
{
    // ProtocolCompiler() already runs every frame on Android,
    // so this shim is intentionally a no-op.
}

#endif // __ANDROID__
