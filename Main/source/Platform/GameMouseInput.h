#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// GameMouseInput.h
// Translates Android touch / motion events into the global mouse state that
// the game reads every frame (MouseX, MouseY, MouseLButton, etc.).
// These globals are declared in ZzzOpenglUtil.cpp and read throughout the game.
// ─────────────────────────────────────────────────────────────────────────────
#include <android/input.h>

namespace GameMouseInput
{
    // Called from android_main.cpp onInputEvent callback.
    // Returns 1 if the event was consumed.
    int ProcessEvent(AInputEvent* event);

    // Called every frame to decay one-frame flags (Push/Pop states).
    void Update();
}

#endif // __ANDROID__
