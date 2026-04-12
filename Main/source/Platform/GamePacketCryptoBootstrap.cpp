#include "GamePacketCryptoBootstrap.h"

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "MUAndroid"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

// Declared in Protocol.cpp / SocketSystem.cpp
extern bool LoadEncryptionKeys();  // loads Enc1.dat / Dec2.dat

namespace GamePacketCryptoBootstrap
{
    bool Load()
    {
#ifdef __ANDROID__
        LOGI("Loading packet crypto keys...");
#endif
        bool ok = LoadEncryptionKeys();
#ifdef __ANDROID__
        if (ok) LOGI("Crypto keys loaded OK");
        else    LOGE("Crypto keys load FAILED");
#endif
        return ok;
    }
}
