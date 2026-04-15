#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// AndroidNetworkPollCompat.cpp
// Non-blocking socket poll for Android — replaces WSAAsyncSelect model.
// ─────────────────────────────────────────────────────────────────────────────
#include "stdafx.h"
#include "AndroidNetworkPollCompat.h"
#include "wsctlc.h"
#include <android/log.h>
#define LOG_TAG "MUNetwork"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern CWsctlc SocketClient;

void PollSocketIO()
{
    if (SocketClient.GetSocket() == INVALID_SOCKET)
    {
        return;
    }

    SocketClient.FDWriteSend();
    SocketClient.nRecv();
}

#endif // __ANDROID__
