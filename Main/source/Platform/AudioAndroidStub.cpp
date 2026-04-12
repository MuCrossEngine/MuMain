#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// AudioAndroidStub.cpp
// Silent no-op stubs for wzAudio and DirectSound APIs.
// These keep the game compilable while AudioOpenSLES.cpp is being implemented.
// Remove this file once AudioOpenSLES.cpp provides real implementations.
// ─────────────────────────────────────────────────────────────────────────────

// ── wzAudio API stubs ────────────────────────────────────────────────────────
extern "C" {
    void* wzAudioCreate(void*)             { return (void*)1; }
    void  wzAudioDestroy(void*)            {}
    int   wzAudioPlay(const char*, int)    { return 0; }
    void  wzAudioStop()                    {}
    void  wzAudioPause()                   {}
    void  wzAudioResume()                  {}
    void  wzAudioSetVolume(int)            {}
    int   wzAudioGetStreamOffsetRange()    { return 0; }
    void  wzAudioSeek(int)                 {}
    void  wzAudioSetMixerMode(int, int)    {}
    void  wzAudioSetEqualizer(int, float*) {}
}

// ── DirectSound API stubs ────────────────────────────────────────────────────
// The real implementations are in DSplaysound.cpp wrapped with #ifdef _WIN32.
// These stubs satisfy the linker when building for Android.
extern "C" {
    int  InitDirectSound(void*)                       { return 1; }
    void UninitDirectSound()                          {}
    int  CreateStaticBuffer(int, const char*)         { return -1; }
    void PlayBuffer(int, int, int, float, float, float) {}
    void StopBuffer(int)                              {}
    void SetVolume(int, int)                          {}
    int  LoadWaveFile(const char*, int)               { return -1; }
    void Set3DSoundPosition(int, float, float, float, float, float, float) {}
    void SetListenerPosition(float, float, float)     {}
}

#endif // __ANDROID__
