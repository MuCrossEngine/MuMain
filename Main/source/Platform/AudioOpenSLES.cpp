#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// AudioOpenSLES.cpp
// OpenSL ES implementation for BGM (MP3/OGG) and SFX (WAV).
// BGM uses Android FileDescriptor player for MP3 decoding via the OS codec.
// SFX uses PCM buffer players for low-latency WAV playback.
// ─────────────────────────────────────────────────────────────────────────────
#include "AudioOpenSLES.h"
#include "AndroidWin32Compat.h"
#include "GameAssetPath.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#define LOG_TAG "MUAudio"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ─────────────────────────────────────────────────────────────────────────────
// Engine globals
// ─────────────────────────────────────────────────────────────────────────────
static SLObjectItf  g_engineObj   = nullptr;
static SLEngineItf  g_engine      = nullptr;
static SLObjectItf  g_outputMix   = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// BGM player (URI-based for MP3 playback via Android MediaCodec)
// ─────────────────────────────────────────────────────────────────────────────
static SLObjectItf  g_bgmPlayerObj  = nullptr;
static SLPlayItf    g_bgmPlay       = nullptr;
static SLVolumeItf  g_bgmVolume     = nullptr;
static SLSeekItf    g_bgmSeek       = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// SFX buffers (legacy client loads many WAV ids up front)
// ─────────────────────────────────────────────────────────────────────────────
static const int MAX_SFX_CHANNELS = 512;

struct SfxChannel
{
    SLObjectItf playerObj  = nullptr;
    SLPlayItf   play       = nullptr;
    SLVolumeItf volume     = nullptr;
    std::vector<uint8_t> pcmData;
    bool        active     = false;
};

static SfxChannel g_sfx[MAX_SFX_CHANNELS];
static bool g_audioInitialized = false;
static int  g_bgmVolumePercent = 100;
static long g_masterVolumeDS   = 0;
static bool g_enableSound      = true;

static constexpr int MAX_LEGACY_SOUND_BUFFER = 4096;
static std::array<int, MAX_LEGACY_SOUND_BUFFER> g_legacyBufferToSfxSlot;
static bool g_legacyBufferMapInitialized = false;

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────
static void EnsureLegacyBufferMap()
{
    if (g_legacyBufferMapInitialized) return;
    g_legacyBufferToSfxSlot.fill(-1);
    g_legacyBufferMapInitialized = true;
}

static int ClampPercent(int volume)
{
    return std::max(0, std::min(100, volume));
}

static int DSoundVolumeToPercent(long volume)
{
    if (volume <= -10000L) return 0;
    if (volume >= 0L)      return 100;
    return static_cast<int>((volume + 10000L) / 100L);
}

static void DestroyBGMPlayer()
{
    if (g_bgmPlayerObj)
    {
        (*g_bgmPlayerObj)->Destroy(g_bgmPlayerObj);
        g_bgmPlayerObj = nullptr;
        g_bgmPlay = nullptr;
        g_bgmVolume = nullptr;
        g_bgmSeek = nullptr;
    }
}

static void ReleaseSFXSlot(int idx)
{
    if (idx < 0 || idx >= MAX_SFX_CHANNELS) return;

    if (g_sfx[idx].playerObj)
    {
        (*g_sfx[idx].playerObj)->Destroy(g_sfx[idx].playerObj);
        g_sfx[idx].playerObj = nullptr;
    }

    g_sfx[idx].play = nullptr;
    g_sfx[idx].volume = nullptr;
    g_sfx[idx].pcmData.clear();
    g_sfx[idx].active = false;
}

static int LookupSfxSlotByBuffer(int buffer)
{
    EnsureLegacyBufferMap();
    if (buffer < 0 || buffer >= MAX_LEGACY_SOUND_BUFFER) return -1;
    return g_legacyBufferToSfxSlot[buffer];
}

static bool EnsureAudioInitialized()
{
    if (g_audioInitialized) return true;
    g_audioInitialized = AudioOpenSLES::Init();
    if (g_audioInitialized)
    {
        AudioOpenSLES::SetBGMVolume(g_bgmVolumePercent);
    }
    return g_audioInitialized;
}

// ─────────────────────────────────────────────────────────────────────────────
namespace AudioOpenSLES
{

bool Init()
{
    if (g_engineObj && g_engine && g_outputMix)
        return true;

    SLresult result;

    // Create engine
    result = slCreateEngine(&g_engineObj, 0, nullptr, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) { LOGE("slCreateEngine failed: %d", result); return false; }

    result = (*g_engineObj)->Realize(g_engineObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) { LOGE("Engine Realize failed"); return false; }

    result = (*g_engineObj)->GetInterface(g_engineObj, SL_IID_ENGINE, &g_engine);
    if (result != SL_RESULT_SUCCESS) { LOGE("GetInterface ENGINE failed"); return false; }

    // Create output mix
    const SLInterfaceID outIds[]  = { SL_IID_VOLUME };
    const SLboolean     outReq[]  = { SL_BOOLEAN_FALSE };
    result = (*g_engine)->CreateOutputMix(g_engine, &g_outputMix, 1, outIds, outReq);
    if (result != SL_RESULT_SUCCESS) { LOGE("CreateOutputMix failed"); return false; }

    result = (*g_outputMix)->Realize(g_outputMix, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) { LOGE("OutputMix Realize failed"); return false; }

    g_audioInitialized = true;
    LOGI("OpenSL ES initialized");
    return true;
}

void Shutdown()
{
    DestroyBGMPlayer();

    for (int i = 0; i < MAX_SFX_CHANNELS; i++)
    {
        ReleaseSFXSlot(i);
    }

    if (g_outputMix) { (*g_outputMix)->Destroy(g_outputMix); g_outputMix = nullptr; }
    if (g_engineObj) { (*g_engineObj)->Destroy(g_engineObj); g_engineObj = nullptr; }
    g_engine = nullptr;
    g_audioInitialized = false;

    LOGI("OpenSL ES shut down");
}

// ── BGM ──────────────────────────────────────────────────────────────────────

bool PlayBGM(const char* filename, int /*repeat*/)
{
    if (!g_engine || !filename) return false;
    DestroyBGMPlayer();

    std::string path = GameAssetPath::Resolve(filename);
    std::string uri  = (path.rfind("file://", 0) == 0) ? path : ("file://" + path);

    // Use Android URI data source (supports MP3/OGG via MediaCodec)
    SLDataLocator_URI   loc = { SL_DATALOCATOR_URI, (SLchar*)uri.c_str() };
    SLDataFormat_MIME   fmt = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource        src = { &loc, &fmt };

    SLDataLocator_OutputMix outLoc = { SL_DATALOCATOR_OUTPUTMIX, g_outputMix };
    SLDataSink              sink   = { &outLoc, nullptr };

    const SLInterfaceID ids[]  = { SL_IID_PLAY, SL_IID_VOLUME, SL_IID_SEEK };
    const SLboolean     req[]  = { SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE };

    SLresult r = (*g_engine)->CreateAudioPlayer(g_engine, &g_bgmPlayerObj, &src, &sink, 3, ids, req);
    if (r != SL_RESULT_SUCCESS) { LOGE("CreateAudioPlayer BGM failed: %d", r); return false; }

    (*g_bgmPlayerObj)->Realize(g_bgmPlayerObj, SL_BOOLEAN_FALSE);
    (*g_bgmPlayerObj)->GetInterface(g_bgmPlayerObj, SL_IID_PLAY,   &g_bgmPlay);
    (*g_bgmPlayerObj)->GetInterface(g_bgmPlayerObj, SL_IID_VOLUME, &g_bgmVolume);
    (*g_bgmPlayerObj)->GetInterface(g_bgmPlayerObj, SL_IID_SEEK,   &g_bgmSeek);

    if (g_bgmSeek)
        (*g_bgmSeek)->SetLoop(g_bgmSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);

    (*g_bgmPlay)->SetPlayState(g_bgmPlay, SL_PLAYSTATE_PLAYING);
    LOGI("BGM playing: %s", filename);
    return true;
}

void StopBGM()
{
    if (g_bgmPlay) (*g_bgmPlay)->SetPlayState(g_bgmPlay, SL_PLAYSTATE_STOPPED);
    DestroyBGMPlayer();
}

void PauseBGM()
{
    if (g_bgmPlay) (*g_bgmPlay)->SetPlayState(g_bgmPlay, SL_PLAYSTATE_PAUSED);
}

void ResumeBGM()
{
    if (g_bgmPlay) (*g_bgmPlay)->SetPlayState(g_bgmPlay, SL_PLAYSTATE_PLAYING);
}

void SetBGMVolume(int volume)
{
    if (!g_bgmVolume) return;
    g_bgmVolumePercent = ClampPercent(volume);
    // Convert 0-100 → millibels (-6000 to 0)
    SLmillibel mb = (g_bgmVolumePercent <= 0) ? SL_MILLIBEL_MIN : (SLmillibel)((g_bgmVolumePercent - 100) * 60);
    (*g_bgmVolume)->SetVolumeLevel(g_bgmVolume, mb);
}

// ── SFX ──────────────────────────────────────────────────────────────────────

// Simple WAV header parser (PCM only)
static bool ParseWAV(const std::vector<uint8_t>& data,
                     int* outRate, int* outChannels, int* outBits,
                     size_t* outDataOffset, size_t* outDataSize)
{
    if (data.size() < 44) return false;
    if (memcmp(data.data(), "RIFF", 4) != 0) return false;
    if (memcmp(data.data() + 8, "WAVE", 4) != 0) return false;

    // Find 'fmt ' chunk
    size_t pos = 12;
    while (pos + 8 <= data.size())
    {
        uint32_t chunkId   = *(uint32_t*)(data.data() + pos);
        uint32_t chunkSize = *(uint32_t*)(data.data() + pos + 4);
        if (memcmp(data.data() + pos, "fmt ", 4) == 0)
        {
            *outChannels = *(uint16_t*)(data.data() + pos + 10);
            *outRate     = *(uint32_t*)(data.data() + pos + 12);
            *outBits     = *(uint16_t*)(data.data() + pos + 22);
        }
        else if (memcmp(data.data() + pos, "data", 4) == 0)
        {
            *outDataOffset = pos + 8;
            *outDataSize   = chunkSize;
            return true;
        }
        pos += 8 + chunkSize;
        (void)chunkId;
    }
    return false;
}

int LoadWAV(const char* filename)
{
    // Find a free channel
    int slot = -1;
    for (int i = 0; i < MAX_SFX_CHANNELS; i++)
        if (!g_sfx[i].active) { slot = i; break; }
    if (slot < 0) { LOGE("LoadWAV: no free channel"); return -1; }

    FILE* f = GameAssetPath::OpenFile(filename, "rb");
    if (!f) { LOGE("LoadWAV: cannot open %s", filename); return -1; }

    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    rewind(f);

    g_sfx[slot].pcmData.resize(sz);
    fread(g_sfx[slot].pcmData.data(), 1, sz, f);
    fclose(f);

    int rate = 22050, channels = 2, bits = 16;
    size_t dataOff = 0, dataSz = 0;
    if (!ParseWAV(g_sfx[slot].pcmData, &rate, &channels, &bits, &dataOff, &dataSz))
    {
        LOGE("LoadWAV: invalid WAV %s", filename);
        g_sfx[slot].pcmData.clear();
        return -1;
    }

    SLDataLocator_BufferQueue bufLoc = { SL_DATALOCATOR_BUFFERQUEUE, 1 };
    SLDataFormat_PCM pcmFmt = {
        SL_DATAFORMAT_PCM,
        (SLuint32)channels,
        (SLuint32)(rate * 1000), // millihertz
        (SLuint32)bits,
        (SLuint32)bits,
        (channels == 2) ? (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT)
                        : SL_SPEAKER_FRONT_CENTER,
        SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource src  = { &bufLoc, &pcmFmt };

    SLDataLocator_OutputMix outLoc = { SL_DATALOCATOR_OUTPUTMIX, g_outputMix };
    SLDataSink              sink   = { &outLoc, nullptr };

    const SLInterfaceID ids[] = { SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
    const SLboolean     req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE };

    if (g_sfx[slot].playerObj)
    {
        (*g_sfx[slot].playerObj)->Destroy(g_sfx[slot].playerObj);
        g_sfx[slot].playerObj = nullptr;
    }

    SLresult r = (*g_engine)->CreateAudioPlayer(g_engine, &g_sfx[slot].playerObj, &src, &sink, 2, ids, req);
    if (r != SL_RESULT_SUCCESS) { LOGE("CreateAudioPlayer SFX failed: %d", r); return -1; }

    (*g_sfx[slot].playerObj)->Realize(g_sfx[slot].playerObj, SL_BOOLEAN_FALSE);
    (*g_sfx[slot].playerObj)->GetInterface(g_sfx[slot].playerObj, SL_IID_PLAY,        &g_sfx[slot].play);
    (*g_sfx[slot].playerObj)->GetInterface(g_sfx[slot].playerObj, SL_IID_VOLUME,      &g_sfx[slot].volume);

    g_sfx[slot].active = true;
    return slot;
}

void PlaySFX(int idx, bool loop)
{
    if (idx < 0 || idx >= MAX_SFX_CHANNELS || !g_sfx[idx].active) return;

    // Re-enqueue PCM data (find data chunk offset)
    int rate, channels, bits;
    size_t dataOff = 0, dataSz = 0;
    ParseWAV(g_sfx[idx].pcmData, &rate, &channels, &bits, &dataOff, &dataSz);

    SLBufferQueueItf bq;
    (*g_sfx[idx].playerObj)->GetInterface(g_sfx[idx].playerObj, SL_IID_BUFFERQUEUE, &bq);
    if (bq)
    {
        (*bq)->Clear(bq);
        (*bq)->Enqueue(bq, g_sfx[idx].pcmData.data() + dataOff, (SLuint32)dataSz);
    }
    (*g_sfx[idx].play)->SetPlayState(g_sfx[idx].play, SL_PLAYSTATE_PLAYING);
    (void)loop;
}

void StopSFX(int idx)
{
    if (idx < 0 || idx >= MAX_SFX_CHANNELS || !g_sfx[idx].active) return;
    (*g_sfx[idx].play)->SetPlayState(g_sfx[idx].play, SL_PLAYSTATE_STOPPED);
}

void SetSFXVolume(int idx, int volume)
{
    if (idx < 0 || idx >= MAX_SFX_CHANNELS || !g_sfx[idx].volume) return;
    SLmillibel mb = (volume <= 0) ? SL_MILLIBEL_MIN : (SLmillibel)((volume - 100) * 60);
    (*g_sfx[idx].volume)->SetVolumeLevel(g_sfx[idx].volume, mb);
}

void SetListenerPosition(float, float, float) {} // TODO: SL3DLocationItf
void SetSFXPosition(int, float, float, float)  {} // TODO: SL3DLocationItf

} // namespace AudioOpenSLES

// ─────────────────────────────────────────────────────────────────────────────
// Bridge: legacy audio APIs (wzAudio + DSPlaySound) → AudioOpenSLES
// ─────────────────────────────────────────────────────────────────────────────
extern "C" {
int wzAudioCreate(HWND)
{
    return EnsureAudioInitialized() ? 0 : -1;
}

void wzAudioDestroy()
{
    AudioOpenSLES::Shutdown();
}

void wzAudioPlay(char* filename, int repeat)
{
    if (!g_enableSound || !filename) return;
    if (!EnsureAudioInitialized()) return;
    AudioOpenSLES::PlayBGM(filename, repeat);
}

void wzAudioPause()
{
    AudioOpenSLES::PauseBGM();
}

void wzAudioStop()
{
    AudioOpenSLES::StopBGM();
}

void wzAudioSetVolume(int volume)
{
    g_bgmVolumePercent = ClampPercent(volume);
    if (!EnsureAudioInitialized()) return;
    AudioOpenSLES::SetBGMVolume(g_bgmVolumePercent);
}

int wzAudioGetVolume()
{
    return g_bgmVolumePercent;
}

void wzAudioVolumeUp()
{
    wzAudioSetVolume(g_bgmVolumePercent + 5);
}

void wzAudioVolumeDown()
{
    wzAudioSetVolume(g_bgmVolumePercent - 5);
}

int wzAudioOpenFile(char*)
{
    return 0;
}

void wzAudioSeek(int)
{
}

int wzAudioGetStreamOffsetRange()
{
    return 0;
}

int wzAudioGetStreamOffsetSec()
{
    return 0;
}

void wzAudioSetMixerMode(int)
{
}

void wzAudioGetStreamInfo(char* bitrate, char* frequency)
{
    if (bitrate)   strcpy(bitrate, "N/A");
    if (frequency) strcpy(frequency, "N/A");
}

void wzAudioOption(int, int)
{
}

void wzAudioSetEqualizer(const int*)
{
}
} // extern "C"

class OBJECT;

HRESULT InitDirectSound(HWND)
{
    return EnsureAudioInitialized() ? S_OK : E_FAIL;
}

void SetEnableSound(bool enabled)
{
    g_enableSound = enabled;
}

void LoadWaveFile(int buffer, char* filename, int, bool)
{
    EnsureLegacyBufferMap();
    if (!EnsureAudioInitialized() || !filename) return;
    if (buffer < 0 || buffer >= MAX_LEGACY_SOUND_BUFFER) return;

    const int oldSlot = g_legacyBufferToSfxSlot[buffer];
    if (oldSlot >= 0)
    {
        ReleaseSFXSlot(oldSlot);
        g_legacyBufferToSfxSlot[buffer] = -1;
    }

    int slot = AudioOpenSLES::LoadWAV(filename);
    g_legacyBufferToSfxSlot[buffer] = slot;

    if (slot >= 0)
    {
        AudioOpenSLES::SetSFXVolume(slot, DSoundVolumeToPercent(g_masterVolumeDS));
    }
}

HRESULT PlayBuffer(int buffer, OBJECT*, BOOL looped)
{
    if (!g_enableSound) return S_FALSE;

    const int slot = LookupSfxSlotByBuffer(buffer);
    if (slot < 0) return E_FAIL;

    AudioOpenSLES::PlaySFX(slot, looped != FALSE);
    return S_OK;
}

void StopBuffer(int buffer, BOOL)
{
    const int slot = LookupSfxSlotByBuffer(buffer);
    if (slot >= 0)
    {
        AudioOpenSLES::StopSFX(slot);
    }
}

void AllStopSound(void)
{
    EnsureLegacyBufferMap();
    for (int buffer = 0; buffer < MAX_LEGACY_SOUND_BUFFER; ++buffer)
    {
        const int slot = g_legacyBufferToSfxSlot[buffer];
        if (slot >= 0)
        {
            AudioOpenSLES::StopSFX(slot);
        }
    }
}

void Set3DSoundPosition()
{
}

HRESULT ReleaseBuffer(int buffer)
{
    EnsureLegacyBufferMap();
    if (buffer < 0 || buffer >= MAX_LEGACY_SOUND_BUFFER) return E_INVALIDARG;

    const int slot = g_legacyBufferToSfxSlot[buffer];
    if (slot >= 0)
    {
        ReleaseSFXSlot(slot);
        g_legacyBufferToSfxSlot[buffer] = -1;
    }

    return S_OK;
}

HRESULT RestoreBuffers(int, int)
{
    return S_OK;
}

void SetVolume(int buffer, long volume)
{
    const int slot = LookupSfxSlotByBuffer(buffer);
    if (slot < 0) return;
    AudioOpenSLES::SetSFXVolume(slot, DSoundVolumeToPercent(volume));
}

void SetMasterVolume(long volume)
{
    g_masterVolumeDS = volume;
    const int percent = DSoundVolumeToPercent(volume);

    AudioOpenSLES::SetBGMVolume(percent);

    EnsureLegacyBufferMap();
    for (int buffer = 0; buffer < MAX_LEGACY_SOUND_BUFFER; ++buffer)
    {
        const int slot = g_legacyBufferToSfxSlot[buffer];
        if (slot >= 0)
        {
            AudioOpenSLES::SetSFXVolume(slot, percent);
        }
    }
}

void FreeDirectSound()
{
    AudioOpenSLES::Shutdown();
    EnsureLegacyBufferMap();
    g_legacyBufferToSfxSlot.fill(-1);
}

#endif // __ANDROID__
