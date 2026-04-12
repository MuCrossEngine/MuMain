#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// AudioOpenSLES.h
// OpenSL ES audio engine.
// Implements both the wzAudio (BGM/MP3 streaming) and DirectSound (WAV SFX)
// APIs for Android.
// ─────────────────────────────────────────────────────────────────────────────

namespace AudioOpenSLES
{
    // ── Engine lifecycle ─────────────────────────────────────────────────────
    bool Init();
    void Shutdown();

    // ── BGM (wzAudio replacement) ────────────────────────────────────────────
    // Play an MP3 or OGG file. Pass repeat=-1 for infinite loop.
    bool PlayBGM(const char* filename, int repeat = -1);
    void StopBGM();
    void PauseBGM();
    void ResumeBGM();
    void SetBGMVolume(int volume); // 0–100

    // ── SFX (DirectSound replacement) ────────────────────────────────────────
    // Load a WAV file into a buffer slot (0-based index).
    int  LoadWAV(const char* filename);

    // Play a loaded buffer. loop=true for looping sounds.
    void PlaySFX(int bufferIdx, bool loop = false);
    void StopSFX(int bufferIdx);
    void SetSFXVolume(int bufferIdx, int volume); // 0–100

    // 3D positional audio
    void SetListenerPosition(float x, float y, float z);
    void SetSFXPosition(int bufferIdx, float x, float y, float z);
}

#endif // __ANDROID__
