#ifdef __ANDROID__
#include "stdafx.h"
// ─────────────────────────────────────────────────────────────────────────────
// LegacyAndroidClientStubs.cpp
// No-op stubs for Windows-only systems that have no Android equivalent:
//   - Nprotect / GameGuard anti-cheat
//   - ShareMemory (shared memory IPC)
//   - GetCheckSum (integrity checks)
//   - DirectInput stubs
// ─────────────────────────────────────────────────────────────────────────────
#include <android/log.h>
#include <atomic>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "AndroidWin32Compat.h"
#include "GameAssetPath.h"
#include "wsclientinline.h"
#define LOG_TAG "MUAndroid"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ─────────────────────────────────────────────────────────────────────────────
// Nprotect stubs (declared in Nprotect.h)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" {
    void  LaunchNprotect()           {}
    void  CloseNprotect()            {}
    int   CheckTotalNprotect()       { return 0; }  // 0 = no hack detected
    bool  FindNprotectWindow()       { return false; }
    void  NprotectCheckCRC(void*)    {}
}

// ─────────────────────────────────────────────────────────────────────────────
// ShareMemory stubs (ShareMemory.lib)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" {
    void* CreateShareMemory(const char*, int)  { return nullptr; }
    void* OpenShareMemory(const char*)         { return nullptr; }
    void  CloseShareMemory(void*)              {}
    void* GetShareMemoryPtr(void*)             { return nullptr; }
}

// ─────────────────────────────────────────────────────────────────────────────
// Integrity check — real implementation matching Winmain.cpp
// ─────────────────────────────────────────────────────────────────────────────
void CheckHack(void)
{
    // Keep original client behavior: periodically send 0x0E check packet.
    // GameServer drops the client with CheckSumTime Error if this is skipped.
    SendCheck();
}

static WORD DecryptCheckSumKey(WORD wSource)
{
    WORD wAcc = wSource ^ 0xB479;
    return ((wAcc >> 10) << 4) | (wAcc & 0xF);
}

static DWORD GenerateCheckSum(BYTE* pbyBuffer, DWORD dwSize, WORD wKey)
{
    DWORD dwKey = (DWORD)wKey;
    DWORD dwResult = dwKey << 9;
    for (DWORD dwChecked = 0; dwChecked <= dwSize - 4; dwChecked += 4)
    {
        DWORD dwTemp;
        memcpy(&dwTemp, pbyBuffer + dwChecked, sizeof(DWORD));

        switch ((dwChecked / 4 + wKey) % 3)
        {
        case 0:
            dwResult ^= dwTemp;
            break;
        case 1:
            dwResult += dwTemp;
            break;
        case 2:
            dwResult <<= (dwTemp % 11);
            dwResult ^= dwTemp;
            break;
        }

        if (0 == (dwChecked % 4))
        {
            dwResult ^= ((dwKey + dwResult) >> ((dwChecked / 4) % 16 + 3));
        }
    }
    return dwResult;
}

DWORD GetCheckSum(WORD wKey)
{
    wKey = DecryptCheckSumKey(wKey);

    std::string path = GameAssetPath::Resolve("Data/Local/GameGuard.csr");
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp)
    {
        LOGI("GetCheckSum: could not open %s", path.c_str());
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 0)
    {
        fclose(fp);
        return 0;
    }

    std::vector<BYTE> buf(sz);
    fread(buf.data(), 1, sz, fp);
    fclose(fp);

    return GenerateCheckSum(buf.data(), (DWORD)sz, wKey);
}

void StopMp3(char* Name, BOOL)
{
    static char s_lastMp3[256] = {0};
    if (Name && strcmp(Name, s_lastMp3) == 0)
        s_lastMp3[0] = '\0';
}

void PlayMp3(char* Name, BOOL)
{
    static char s_lastMp3[256] = {0};
    if (!Name) return;
    strncpy(s_lastMp3, Name, sizeof(s_lastMp3) - 1);
    s_lastMp3[sizeof(s_lastMp3) - 1] = '\0';
}

bool IsEndMp3() { return true; }
int GetMp3PlayPosition() { return 100; }

unsigned int GenID()
{
    static std::atomic_uint s_id{1};
    return s_id.fetch_add(1);
}

void CloseMainExe(void) {}
void KillGLWindow(void) {}
void DestroyWindow() {}
void DestroySound() {}

// ─────────────────────────────────────────────────────────────────────────────
// glprocs stubs (Windows GL extension loader — not needed on Android/GLES3)
// ─────────────────────────────────────────────────────────────────────────────
// All glProcs functions are native in GLES3; no stubs needed.

#endif // __ANDROID__
