// ─────────────────────────────────────────────────────────────────────────────
// android_main.cpp
// Entry point for Android NativeActivity.
// Replaces WinMain() + WINHANDLE message loop from the Windows build.
// The entire game logic (ZzzScene, Protocol, UI, etc.) is identical to PC.
// ─────────────────────────────────────────────────────────────────────────────
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <jni.h>
#include <string>
#include <atomic>
#include <chrono>

#define LOG_TAG "MUAndroid"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ── Platform layer ───────────────────────────────────────────────────────────
#include "Platform/AndroidWin32Compat.h"
#include "Platform/AndroidEglWindow.h"
#include "Platform/GameAssetPath.h"
#include "Platform/GameConfigBootstrap.h"
#include "Platform/GamePacketCryptoBootstrap.h"
#include "Platform/GameConnectServerBootstrap.h"
#include "Platform/LegacyClientRuntime.h"
#include "Platform/GameMouseInput.h"
#include "Platform/RenderBackend.h"
#include "Platform/GameDownloader.h"
#include "Platform/AudioOpenSLES.h"

// ── Game headers (same as Windows) ──────────────────────────────────────────
#include "stdafx.h"
#include "ZzzScene.h"
#include "ZzzOpenglUtil.h"
#include "ZzzTexture.h"
#include "ZzzOpenData.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "UIManager.h"
#include "UIMng.h"
#include "Input.h"
#include "Time/Timer.h"
#include "CGMProtect.h"
#include "CGMFontLayer.h"

// Networking poll (replaces WSAAsyncSelect model)
#include "Platform/AndroidNetworkPollCompat.h"

// External SceneFlag and game loop functions declared in ZzzScene.cpp
extern int SceneFlag;
extern void MainScene();

// ─────────────────────────────────────────────────────────────────────────────
// App state
// ─────────────────────────────────────────────────────────────────────────────
static android_app*        g_app         = nullptr;
static AndroidEglWindow*   g_eglWindow   = nullptr;
static bool                g_initialized = false;
static bool                g_focused     = false;
static std::atomic<bool>   g_running{true};

// JNI helper: get external files path from Java side
static std::string GetExternalFilesDir(android_app* app)
{
    JNIEnv* env = nullptr;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    jclass    cls    = env->GetObjectClass(app->activity->clazz);
    jmethodID method = env->GetMethodID(cls, "getExternalFilesPath", "()Ljava/lang/String;");
    jstring   jpath  = (jstring)env->CallObjectMethod(app->activity->clazz, method);

    const char* cpath = env->GetStringUTFChars(jpath, nullptr);
    std::string path(cpath);
    env->ReleaseStringUTFChars(jpath, cpath);
    env->DeleteLocalRef(jpath);
    app->activity->vm->DetachCurrentThread();
    return path;
}

// ─────────────────────────────────────────────────────────────────────────────
// Initialization — mirrors WinMain() init sequence
// ─────────────────────────────────────────────────────────────────────────────
static bool InitGame(android_app* app)
{
    LOGI("InitGame: starting bootstrap");

    // 1. Asset path — everything else depends on this
    std::string dataPath = GetExternalFilesDir(app);
    GameAssetPath::Init(dataPath);
    LOGI("InitGame: asset path = %s", dataPath.c_str());

    // 2. First-run downloader: if game data is missing, download it
    if (!GameDownloader::IsDataReady())
    {
        LOGI("InitGame: game data not found, starting download");
        if (!GameDownloader::DownloadAll(g_eglWindow))
        {
            LOGE("InitGame: download failed");
            return false;
        }
    }

    // 3. Config bootstrap (CProtect, Configs.xtm)
    if (!GameConfigBootstrap::Load())
    {
        LOGE("InitGame: config bootstrap failed");
        return false;
    }

    // 4. Packet crypto keys (Enc1.dat / Dec2.dat)
    if (!GamePacketCryptoBootstrap::Load())
    {
        LOGE("InitGame: crypto bootstrap failed");
        return false;
    }

    // 5. Connect server list
    if (!GameConnectServerBootstrap::Load())
    {
        LOGE("InitGame: connect server bootstrap failed");
        return false;
    }

    // 6. Audio engine
    AudioOpenSLES::Init();

    // 7. Global random table (used throughout the game)
    LegacyClientRuntime::InitRandomTable();

    // 8. Game data loading (mirrors OpenBasicData call sequence in WebzenScene)
    // SceneFlag starts at WEBZEN_SCENE; MainScene() handles the rest
    LOGI("InitGame: bootstrap complete, entering game loop");
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Per-frame render — identical to the Windows Scene() call
// ─────────────────────────────────────────────────────────────────────────────
static void RenderFrame()
{
    if (!g_eglWindow || !g_focused) return;

    // Process network packets (replaces WSAAsyncSelect / WM_SOCKET)
    PollSocketIO();
    ProtocolCompiler();

    // Game logic + rendering — same function as on Windows
    MainScene();

    g_eglWindow->SwapBuffers();
}

// ─────────────────────────────────────────────────────────────────────────────
// NativeActivity command callback
// ─────────────────────────────────────────────────────────────────────────────
static void OnAppCmd(android_app* app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        if (app->window)
        {
            LOGI("APP_CMD_INIT_WINDOW");
            g_eglWindow = new AndroidEglWindow(app->window);
            if (!g_eglWindow->Create())
            {
                LOGE("EGL context creation failed");
                ANativeActivity_finish(app->activity);
                return;
            }
            LegacyClientRuntime::SetWindow(app->window);

            if (!g_initialized)
            {
                if (!InitGame(app))
                {
                    ANativeActivity_finish(app->activity);
                    return;
                }
                g_initialized = true;
            }
            g_focused = true;
        }
        break;

    case APP_CMD_TERM_WINDOW:
        LOGI("APP_CMD_TERM_WINDOW");
        g_focused = false;
        if (g_eglWindow)
        {
            g_eglWindow->Destroy();
            delete g_eglWindow;
            g_eglWindow = nullptr;
        }
        break;

    case APP_CMD_GAINED_FOCUS:
        g_focused = true;
        break;

    case APP_CMD_LOST_FOCUS:
        g_focused = false;
        break;

    case APP_CMD_DESTROY:
        g_running = false;
        break;

    case APP_CMD_SAVE_STATE:
        break;

    default:
        break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Input callback
// ─────────────────────────────────────────────────────────────────────────────
static int32_t OnInputEvent(android_app* app, AInputEvent* event)
{
    return GameMouseInput::ProcessEvent(event);
}

// ─────────────────────────────────────────────────────────────────────────────
// android_main — equivalent to WinMain
// ─────────────────────────────────────────────────────────────────────────────
void android_main(android_app* app)
{
    LOGI("android_main: start");

    g_app = app;
    app->onAppCmd    = OnAppCmd;
    app->onInputEvent = OnInputEvent;

    // Main loop — equivalent to the PeekMessage loop in WINHANDLE.cpp
    while (g_running)
    {
        int events;
        android_poll_source* source;

        // Poll for events (timeout=0 when rendering, -1 when paused)
        int timeout = (g_focused && g_initialized) ? 0 : -1;
        if (ALooper_pollAll(timeout, nullptr, &events, (void**)&source) >= 0)
        {
            if (source) source->process(app, source);
        }

        if (app->destroyRequested)
        {
            g_running = false;
            break;
        }

        if (g_focused && g_initialized)
        {
            RenderFrame();
        }
    }

    // Cleanup
    AudioOpenSLES::Shutdown();
    if (g_eglWindow)
    {
        g_eglWindow->Destroy();
        delete g_eglWindow;
    }

    LOGI("android_main: exit");
}
