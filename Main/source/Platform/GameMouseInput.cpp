#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// GameMouseInput.cpp
// Touch → mouse translation.
//
// Gesture mapping:
//   Single tap           → LButton click
//   Long press (>500ms)  → RButton click
//   Double tap (<300ms)  → LButton double-click
//   Drag (1 finger)      → mouse move + LButton held
//   Two-finger pinch     → mouse wheel (zoom camera)
// ─────────────────────────────────────────────────────────────────────────────
#include "../stdafx.h"
#include "GameMouseInput.h"
#include "AndroidWin32Compat.h"
#include "../NewUICommon.h"   // SEASON3B::CNewKeyInput + KEY_STATE
#include <android/input.h>
#include <android/log.h>
#include <cmath>

#define LOG_TAG "MUAndroid"

// ── Mouse state globals declared in ZzzOpenglUtil.cpp ────────────────────────
extern float MouseX, MouseY;
extern float MouseRenderX, MouseRenderY;
extern bool  MouseLButton, MouseRButton, MouseMButton;
extern bool  MouseLButtonPush, MouseRButtonPush;   // pressed this frame
extern bool  MouseLButtonPop,  MouseRButtonPop;    // released this frame
extern bool  MouseLButtonDBClick;
extern int   MouseWheel;

// Screen dimensions (set by LegacyClientRuntime)
extern int WindowSizeX, WindowSizeY;
// Logical render dimensions used by legacy UI/input hit-tests.
extern unsigned int WindowWidth, WindowHeight;
extern float g_fScreenRate_x;
extern float g_fScreenRate_y;

namespace GameMouseInput
{
    static bool  s_touching        = false;
    static float s_touchX          = 0.f;
    static float s_touchY          = 0.f;
    static float s_lastTouchX      = 0.f;
    static float s_lastTouchY      = 0.f;
    static DWORD s_touchDownTime   = 0;
    static DWORD s_lastTapTime     = 0;
    static bool  s_longPressFired  = false;

    // Pinch tracking
    static float s_lastPinchDist   = 0.f;

    // Deferred release: for quick taps, DOWN and UP both fire before Scene()
    // runs. We defer KEY_RELEASE until Update() (which runs AFTER Scene()) so
    // CInput::Update() inside Scene() always sees KEY_PRESS for at least one frame.
    static bool  s_pendingLButtonRelease = false;
    static bool  s_pendingRButtonRelease = false;

    static float PinchDistance(AInputEvent* event)
    {
        if (AMotionEvent_getPointerCount(event) < 2) return 0.f;
        float x0 = AMotionEvent_getX(event, 0), y0 = AMotionEvent_getY(event, 0);
        float x1 = AMotionEvent_getX(event, 1), y1 = AMotionEvent_getY(event, 1);
        float dx = x1 - x0, dy = y1 - y0;
        return std::sqrt(dx*dx + dy*dy);
    }

    static void SetMousePos(float x, float y)
    {
        float mappedRenderX = x;
        float mappedRenderY = y;

        // Android motion events can arrive in physical surface coordinates
        // while the game runs in a fixed logical backbuffer (1280x720).
        if (WindowSizeX > 0 && WindowSizeY > 0 && WindowWidth > 0 && WindowHeight > 0)
        {
            mappedRenderX = x * (float)WindowWidth / (float)WindowSizeX;
            mappedRenderY = y * (float)WindowHeight / (float)WindowSizeY;
        }

        // Clamp to render target bounds first (WindowWidth/WindowHeight).
        if (mappedRenderX < 0.f) mappedRenderX = 0.f;
        if (mappedRenderY < 0.f) mappedRenderY = 0.f;
        if (WindowWidth > 0 && mappedRenderX > (float)WindowWidth - 1.f) mappedRenderX = (float)WindowWidth - 1.f;
        if (WindowHeight > 0 && mappedRenderY > (float)WindowHeight - 1.f) mappedRenderY = (float)WindowHeight - 1.f;

        // Match WINHANDLE.cpp behavior:
        //   MouseRender* = render-space coordinates
        //   Mouse*       = logical UI coordinates (640x480 space)
        float logicalX = mappedRenderX;
        float logicalY = mappedRenderY;
        if (g_fScreenRate_x > 0.0f) logicalX = mappedRenderX / g_fScreenRate_x;
        if (g_fScreenRate_y > 0.0f) logicalY = mappedRenderY / g_fScreenRate_y;

        const float logicalWidth = (g_fScreenRate_x > 0.0f) ? ((float)WindowWidth / g_fScreenRate_x) : (float)WindowWidth;
        const float logicalHeight = (g_fScreenRate_y > 0.0f) ? ((float)WindowHeight / g_fScreenRate_y) : (float)WindowHeight;
        if (logicalX < 0.f) logicalX = 0.f;
        if (logicalY < 0.f) logicalY = 0.f;
        if (logicalWidth > 0.f && logicalX > logicalWidth - 1.f) logicalX = logicalWidth - 1.f;
        if (logicalHeight > 0.f && logicalY > logicalHeight - 1.f) logicalY = logicalHeight - 1.f;

        MouseRenderX = mappedRenderX;
        MouseRenderY = mappedRenderY;
        MouseX = logicalX;
        MouseY = logicalY;
    }

    static void FireLButtonDown()
    {
        MouseLButton     = true;
        MouseLButtonPush = true;
        MouseLButtonPop  = false;
        // VK_LBUTTON state is driven by ScanAsyncKeyState() from MouseLButton.
    }

    static void FireLButtonUp()
    {
        // Defer MouseLButton=false to Update() so that ScanAsyncKeyState()
        // sees the press for at least one frame before transitioning to release.
        MouseLButtonPop  = true;
        s_pendingLButtonRelease = true;
    }

    static void FireRButtonDown()
    {
        MouseRButton     = true;
        MouseRButtonPush = true;
        MouseRButtonPop  = false;
        // VK_RBUTTON state is driven by ScanAsyncKeyState() from MouseRButton.
    }

    static void FireRButtonUp()
    {
        // Same deferral for RButton.
        MouseRButtonPop  = true;
        s_pendingRButtonRelease = true;
    }

    int ProcessEvent(AInputEvent* event)
    {
        if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION)
            return 0;

        int action   = AMotionEvent_getAction(event);
        int actionCode = action & AMOTION_EVENT_ACTION_MASK;
        int ptrCount = (int)AMotionEvent_getPointerCount(event);

        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        DWORD now = GetTickCount();

        switch (actionCode)
        {
        case AMOTION_EVENT_ACTION_DOWN:
            s_touching       = true;
            s_touchX         = x;
            s_touchY         = y;
            s_lastTouchX     = x;
            s_lastTouchY     = y;
            s_touchDownTime  = now;
            s_longPressFired = false;
            SetMousePos(x, y);
            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "TouchDown x=%.0f y=%.0f", x, y);
            FireLButtonDown();
            break;

        case AMOTION_EVENT_ACTION_MOVE:
            if (ptrCount >= 2)
            {
                // Pinch → mouse wheel
                float dist = PinchDistance(event);
                if (s_lastPinchDist > 0.f)
                {
                    float delta = dist - s_lastPinchDist;
                    MouseWheel += (int)(delta / 10.f); // scale factor
                }
                s_lastPinchDist = dist;
            }
            else
            {
                s_lastPinchDist = 0.f;
                SetMousePos(x, y);

                // Long press detection (>500ms without moving)
                float dx = x - s_touchX, dy = y - s_touchY;
                float moved = std::sqrt(dx*dx + dy*dy);
                if (!s_longPressFired && moved < 10.f && (now - s_touchDownTime) > 500)
                {
                    s_longPressFired = true;
                    FireLButtonUp();   // cancel left
                    FireRButtonDown(); // simulate right click
                    FireRButtonUp();
                }
            }
            break;

        case AMOTION_EVENT_ACTION_UP:
            s_touching = false;
            s_lastPinchDist = 0.f;
            SetMousePos(x, y);

            if (!s_longPressFired)
            {
                FireLButtonUp();

                // Double-tap detection
                if ((now - s_lastTapTime) < 300)
                    MouseLButtonDBClick = true;
                s_lastTapTime = now;
            }
            else
            {
                FireLButtonUp();
            }
            break;

        case AMOTION_EVENT_ACTION_CANCEL:
            s_touching = false;
            FireLButtonUp();
            break;

        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            s_lastPinchDist = PinchDistance(event);
            break;

        case AMOTION_EVENT_ACTION_POINTER_UP:
            s_lastPinchDist = 0.f;
            break;

        default:
            return 0;
        }

        return 1; // consumed
    }

    void Update()
    {
        // Apply deferred releases: clear MouseLButton/MouseRButton AFTER Scene()
        // so ScanAsyncKeyState() (at start of next frame) can see the transition.
        if (s_pendingLButtonRelease)
        {
            s_pendingLButtonRelease = false;
            MouseLButton = false;
        }

        if (s_pendingRButtonRelease)
        {
            s_pendingRButtonRelease = false;
            MouseRButton = false;
        }

        // Clear one-frame flags at the END of each frame
        MouseLButtonPush  = false;
        MouseRButtonPush  = false;
        MouseLButtonPop   = false;
        MouseRButtonPop   = false;
        MouseLButtonDBClick = false;
        MouseWheel        = 0;
    }
}

#endif // __ANDROID__
