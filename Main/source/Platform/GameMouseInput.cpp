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
        // Clamp to screen
        MouseRenderX = (int)x;
        MouseRenderY = (int)y;
        // Game coordinates (may differ if game uses a virtual resolution)
        MouseX = MouseRenderX;
        MouseY = MouseRenderY;
    }

    // Bridge: also update SEASON3B::CNewKeyInput so IsPress(VK_LBUTTON) works
    static inline void SetVKey(int vk, SEASON3B::CNewKeyInput::KEY_STATE st)
    {
        if (SEASON3B::g_pNewKeyInput)
            SEASON3B::g_pNewKeyInput->SetKeyState(vk, st);
    }

    static void FireLButtonDown()
    {
        MouseLButton     = true;
        MouseLButtonPush = true;
        MouseLButtonPop  = false;
        SetVKey(VK_LBUTTON, SEASON3B::CNewKeyInput::KEY_PRESS);
    }

    static void FireLButtonUp()
    {
        MouseLButton     = false;
        MouseLButtonPop  = true;
        MouseLButtonPush = false;
        SetVKey(VK_LBUTTON, SEASON3B::CNewKeyInput::KEY_RELEASE);
    }

    static void FireRButtonDown()
    {
        MouseRButton     = true;
        MouseRButtonPush = true;
        MouseRButtonPop  = false;
        SetVKey(VK_RBUTTON, SEASON3B::CNewKeyInput::KEY_PRESS);
    }

    static void FireRButtonUp()
    {
        MouseRButton     = false;
        MouseRButtonPop  = true;
        MouseRButtonPush = false;
        SetVKey(VK_RBUTTON, SEASON3B::CNewKeyInput::KEY_RELEASE);
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
        // After FireLButtonUp/FireRButtonUp we set KEY_RELEASE; clear to KEY_NONE here
        // (KEY_PRESS → KEY_REPEAT is handled by CNewKeyInput::UpdateInput on Windows;
        //  on Android we just clear release state so IsNone() works next frame)
        if (SEASON3B::g_pNewKeyInput)
        {
            if (!MouseLButton)
                SEASON3B::g_pNewKeyInput->SetKeyState(VK_LBUTTON, SEASON3B::CNewKeyInput::KEY_NONE);
            if (!MouseRButton)
                SEASON3B::g_pNewKeyInput->SetKeyState(VK_RBUTTON, SEASON3B::CNewKeyInput::KEY_NONE);
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
