#include "GamePacketCryptoBootstrap.h"

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "MUAndroid"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

// Declared in Protocol.cpp / SocketSystem.cpp
extern bool LoadEncryptionKeys();  // loads Enc1.dat / Dec2.dat

namespace GamePacketCryptoBootstrap
{
    bool Load()
    {
        bool ok = LoadEncryptionKeys();
#ifdef __ANDROID__
        if (!ok) LOGE("Crypto keys load FAILED");
#endif
        return ok;
    }
}
