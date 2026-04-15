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
#include <sys/stat.h>
#include <cstdio>

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
#include "CGMModelManager.h"
#include "w_MapProcess.h"
#include "w_PetProcess.h"

// Networking poll (replaces WSAAsyncSelect model)
#include "Platform/AndroidNetworkPollCompat.h"

// External SceneFlag and game loop functions declared in ZzzScene.cpp
extern int SceneFlag;
extern void Scene(HDC hDC);
extern float g_fScreenRate_x;
extern float g_fScreenRate_y;

// ─────────────────────────────────────────────────────────────────────────────
// App state
// ─────────────────────────────────────────────────────────────────────────────
static android_app*        g_app         = nullptr;
static AndroidEglWindow*   g_eglWindow   = nullptr;
static bool                g_initialized = false;
static bool                g_focused     = false;
static std::atomic<bool>   g_running{true};
static bool                g_renderBackendInitialized = false;

// Windows initializes this singleton in WinMain(); Android must do it here.
static CGMModelManager     g_androidModelManager;

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

static std::string GetAssetServerUrl(android_app* app)
{
    JNIEnv* env = nullptr;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    jclass cls = env->GetObjectClass(app->activity->clazz);
    jmethodID method = env->GetMethodID(cls, "getAssetServerUrl", "()Ljava/lang/String;");
    if (!method)
    {
        env->DeleteLocalRef(cls);
        app->activity->vm->DetachCurrentThread();
        return std::string();
    }

    jstring jurl = (jstring)env->CallObjectMethod(app->activity->clazz, method);
    std::string url;
    if (jurl)
    {
        const char* cUrl = env->GetStringUTFChars(jurl, nullptr);
        if (cUrl)
        {
            url = cUrl;
            env->ReleaseStringUTFChars(jurl, cUrl);
        }
        env->DeleteLocalRef(jurl);
    }

    env->DeleteLocalRef(cls);
    app->activity->vm->DetachCurrentThread();
    return url;
}

static void SyncLegacyScreenMetrics(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    WindowWidth = static_cast<unsigned int>(width);
    WindowHeight = static_cast<unsigned int>(height);

    int screenType = 0;
    if (gmProtect != nullptr)
    {
        screenType = gmProtect->ScreenType;
    }

    if (screenType == 0)
    {
        g_fScreenRate_x = static_cast<float>(WindowWidth) / 640.0f;
        g_fScreenRate_y = static_cast<float>(WindowHeight) / 480.0f;
    }
    else if (screenType == 1)
    {
        const float byWidth = static_cast<float>(WindowWidth) / 640.0f;
        const float byHeight = static_cast<float>(WindowHeight) / 480.0f;
        const float uniScale = (byWidth >= byHeight) ? byHeight : byWidth;
        g_fScreenRate_x = uniScale;
        g_fScreenRate_y = uniScale;
    }
    else
    {
        if (WindowWidth >= 1920)
        {
            g_fScreenRate_x = 1.6f;
            g_fScreenRate_y = 1.6f;
        }
        else
        {
            g_fScreenRate_x = 1.25f;
            g_fScreenRate_y = 1.25f;
        }
    }

    if (g_fScreenRate_x <= 0.0f || g_fScreenRate_y <= 0.0f)
    {
        g_fScreenRate_x = static_cast<float>(WindowWidth) / 640.0f;
        g_fScreenRate_y = static_cast<float>(WindowHeight) / 480.0f;
    }

    CInput::Instance().Create(gwinhandle->GethWnd(), WindowWidth, WindowHeight);

    LOGI("Screen sync: window=%ux%u scale=%.3f/%.3f screenType=%d",
         WindowWidth,
         WindowHeight,
         g_fScreenRate_x,
         g_fScreenRate_y,
         screenType);
}

static void CallMainActivityVoidMethod(const char* methodName)
{
    if (!g_app || !g_app->activity || !methodName)
    {
        return;
    }

    JNIEnv* env = nullptr;
    if (g_app->activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK || !env)
    {
        LOGE("JNI AttachCurrentThread failed for %s", methodName);
        return;
    }

    jclass cls = env->GetObjectClass(g_app->activity->clazz);
    if (!cls)
    {
        LOGE("GetObjectClass failed for %s", methodName);
        g_app->activity->vm->DetachCurrentThread();
        return;
    }

    jmethodID method = env->GetMethodID(cls, methodName, "()V");
    if (!method)
    {
        LOGE("GetMethodID failed: %s", methodName);
        env->DeleteLocalRef(cls);
        g_app->activity->vm->DetachCurrentThread();
        return;
    }

    env->CallVoidMethod(g_app->activity->clazz, method);
    env->DeleteLocalRef(cls);
    g_app->activity->vm->DetachCurrentThread();
}

static bool CallMainActivityBoolMethod2Strings(const char* methodName,
                                                const char* firstArg,
                                                const char* secondArg)
{
    if (!g_app || !g_app->activity || !methodName || !firstArg || !secondArg)
    {
        return false;
    }

    JNIEnv* env = nullptr;
    if (g_app->activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK || !env)
    {
        LOGE("JNI AttachCurrentThread failed for %s", methodName);
        return false;
    }

    bool ok = false;
    jclass cls = env->GetObjectClass(g_app->activity->clazz);
    if (!cls)
    {
        LOGE("GetObjectClass failed for %s", methodName);
        g_app->activity->vm->DetachCurrentThread();
        return false;
    }

    jmethodID method = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;)Z");
    if (!method)
    {
        LOGE("GetMethodID failed: %s", methodName);
        env->DeleteLocalRef(cls);
        g_app->activity->vm->DetachCurrentThread();
        return false;
    }

    jstring arg0 = env->NewStringUTF(firstArg);
    jstring arg1 = env->NewStringUTF(secondArg);
    if (arg0 && arg1)
    {
        ok = (env->CallBooleanMethod(g_app->activity->clazz, method, arg0, arg1) == JNI_TRUE);
    }

    if (arg0) env->DeleteLocalRef(arg0);
    if (arg1) env->DeleteLocalRef(arg1);
    env->DeleteLocalRef(cls);
    g_app->activity->vm->DetachCurrentThread();
    return ok;
}

static bool PathExists(const std::string& path)
{
    if (path.empty())
    {
        return false;
    }

    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

static bool WriteBootstrapMarker(const std::string& markerPath)
{
    FILE* marker = fopen(markerPath.c_str(), "wb");
    if (!marker)
    {
        return false;
    }

    static const char kMarkerContent[] = "copied\n";
    const size_t wrote = fwrite(kMarkerContent, 1, sizeof(kMarkerContent) - 1, marker);
    fclose(marker);
    return wrote == sizeof(kMarkerContent) - 1;
}

static bool AreBootstrapFilesPresent()
{
    const std::string pakPath = GameAssetPath::Resolve("Data/av-code45.pak");
    const std::string connectPath = GameAssetPath::Resolve("Data/Local/Connect.msil");
    return PathExists(pakPath) && PathExists(connectPath);
}

static bool AreLoginWorldFilesPresent()
{
    const std::string worldMapPath = GameAssetPath::Resolve("Data/World95/EncTerrain95.map");
    const std::string worldAttPath = GameAssetPath::Resolve("Data/World95/EncTerrain95.att");
    return PathExists(worldMapPath) && PathExists(worldAttPath);
}

static bool AreShaderFilesPresent()
{
    const std::string vertexPath = GameAssetPath::Resolve("shaders/fixed_vert.glsl");
    const std::string fragmentPath = GameAssetPath::Resolve("shaders/fixed_frag.glsl");
    return PathExists(vertexPath) && PathExists(fragmentPath);
}

bool AndroidExtractZipArchive(const char* zipPath, const char* targetDir)
{
    return CallMainActivityBoolMethod2Strings("extractZipArchive", zipPath, targetDir);
}

static bool AndroidCopyAssetDirectory(const char* assetDir, const char* targetDir)
{
    return CallMainActivityBoolMethod2Strings("copyAssetDirectoryToPath", assetDir, targetDir);
}

void AndroidShowSoftKeyboard()
{
    CallMainActivityVoidMethod("showSoftKeyboard");
}

void AndroidHideSoftKeyboard()
{
    CallMainActivityVoidMethod("hideSoftKeyboard");
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

    // 1.1 Optional downloader base URL from launch intent extra.
    std::string assetServerUrl = GetAssetServerUrl(app);
    if (!assetServerUrl.empty())
    {
        GameDownloader::SetServerURL(assetServerUrl.c_str());
        LOGI("InitGame: asset server override = %s", assetServerUrl.c_str());
    }

    // 2. Local bundled assets: copy Data/ from APK assets into external files
    //    only on first run (or when required files are missing).
    std::string dataTarget = GameAssetPath::Resolve("Data");
    std::string copyMarker = GameAssetPath::Resolve(".data_assets_copied");

    const bool markerExists = PathExists(copyMarker);
    const bool bootstrapDataReady = AreBootstrapFilesPresent();
    const bool mustCopyData = !markerExists || !bootstrapDataReady;

    if (mustCopyData)
    {
        LOGI("InitGame: copying bundled Data assets (marker=%d, dataReady=%d)",
             markerExists ? 1 : 0,
               bootstrapDataReady ? 1 : 0);

        if (!AndroidCopyAssetDirectory("Data", dataTarget.c_str()))
        {
            LOGE("InitGame: failed to copy bundled assets Data -> %s", dataTarget.c_str());
            return false;
        }

        if (!WriteBootstrapMarker(copyMarker))
        {
            LOGE("InitGame: failed to write copy marker: %s", copyMarker.c_str());
            return false;
        }
    }
    else
    {
        LOGI("InitGame: bundled Data already materialized, skipping copy");
    }

    if (!AreBootstrapFilesPresent())
    {
        LOGE("InitGame: bundled assets copy completed but bootstrap files are still missing");
        return false;
    }

    // 2.1 Ensure login-world files exist. External downloader mirrors can be
    // partial, so keep this world materialized from APK assets when missing.
    if (!AreLoginWorldFilesPresent())
    {
        std::string loginWorldTarget = GameAssetPath::Resolve("Data/World95");
        LOGI("InitGame: login world files missing, copying Data/World95");
        if (!AndroidCopyAssetDirectory("Data/World95", loginWorldTarget.c_str()))
        {
            LOGE("InitGame: failed to copy login world assets -> %s", loginWorldTarget.c_str());
            return false;
        }
    }

    if (!AreLoginWorldFilesPresent())
    {
        LOGE("InitGame: login world assets are missing after copy");
        return false;
    }

    // 2.2 Materialize shader sources used by the GLES fixed-function backend.
    std::string shaderTarget = GameAssetPath::Resolve("shaders");
    if (!AreShaderFilesPresent())
    {
        if (!AndroidCopyAssetDirectory("shaders", shaderTarget.c_str()))
        {
            LOGE("InitGame: failed to copy shader assets -> %s", shaderTarget.c_str());
            return false;
        }
    }

    if (!AreShaderFilesPresent())
    {
        LOGE("InitGame: shader assets are missing after copy");
        return false;
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

#ifndef __ANDROID__
    // 7.1 Buff system runtime (initialized in WinMain on Windows)
    if (!g_BuffSystem)
    {
        g_BuffSystem = BuffStateSystem::Make();
    }

    // 7.2 Map process runtime (initialized in WinMain on Windows)
    if (!g_MapProcess)
    {
        g_MapProcess = MapProcess::Make();
    }

    // 7.3 Pet process runtime (initialized in WinMain on Windows)
    if (!g_petProcess)
    {
        g_petProcess = PetProcess::Make();
    }
#else
    // Android: defer heavy runtime subsystems until MAIN_SCENE entry.
    // They are not required for WEBZEN/LOGIN bootstrap and can trigger
    // early LMK kills on constrained devices.
    LOGI("InitGame: deferring Buff/Map/Pet runtime init until MAIN_SCENE");
#endif

    // 8. Game data loading starts in WEBZEN_SCENE and advances via Scene() dispatcher
    LOGI("InitGame: bootstrap complete, entering game loop");
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Per-frame render — mirrors Windows loop (Scene dispatcher)
// ─────────────────────────────────────────────────────────────────────────────
static void RenderFrame()
{
    if (!g_eglWindow || !g_focused) return;

    // Process network packets (replaces WSAAsyncSelect / WM_SOCKET)
    PollSocketIO();
    ProtocolCompiler();

    // Game logic + rendering — use full scene dispatcher
    Scene(nullptr);
    GameMouseInput::Update();

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

            const int width = ANativeWindow_getWidth(app->window);
            const int height = ANativeWindow_getHeight(app->window);

            if (!g_initialized)
            {
                if (!InitGame(app))
                {
                    ANativeActivity_finish(app->activity);
                    return;
                }
                g_initialized = true;
            }

            if (!g_renderBackendInitialized)
            {
                if (!RenderBackend::Init(width, height))
                {
                    LOGE("RenderBackend::Init failed");
                    ANativeActivity_finish(app->activity);
                    return;
                }
                g_renderBackendInitialized = true;
            }
            else
            {
                RenderBackend::SetScreenSize(width, height);
            }

            SyncLegacyScreenMetrics(width, height);

            g_focused = true;
        }
        break;

    case APP_CMD_TERM_WINDOW:
        LOGI("APP_CMD_TERM_WINDOW");
        g_focused = false;
        if (g_renderBackendInitialized)
        {
            RenderBackend::Shutdown();
            g_renderBackendInitialized = false;
        }
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
    AndroidCompatSetNativeActivity(app->activity);
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
    if (g_renderBackendInitialized)
    {
        RenderBackend::Shutdown();
        g_renderBackendInitialized = false;
    }
    if (g_eglWindow)
    {
        g_eglWindow->Destroy();
        delete g_eglWindow;
    }

    LOGI("android_main: exit");
}
