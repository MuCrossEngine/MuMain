#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// LegacyAndroidClientStubs.cpp
// No-op stubs for Windows-only systems that have no Android equivalent:
//   - Nprotect / GameGuard anti-cheat
//   - ShareMemory (shared memory IPC)
//   - CheckHack / GetCheckSum (integrity checks)
//   - DirectInput stubs
// ─────────────────────────────────────────────────────────────────────────────
#include <android/log.h>
#include <atomic>
#include <string.h>
#include "AndroidWin32Compat.h"
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
// Integrity check stubs (used in CGMProtect.cpp)
// ─────────────────────────────────────────────────────────────────────────────
void CheckHack(void) {}
DWORD GetCheckSum(WORD) { return 0; }

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
