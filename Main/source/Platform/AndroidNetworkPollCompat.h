#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// AndroidNetworkPollCompat.h
// Replaces the WSAAsyncSelect model (event-driven, WM_SOCKET messages) with
// a synchronous non-blocking poll() call that is called every frame.
// Called from android_main.cpp game loop before ProtocolCompiler().
// ─────────────────────────────────────────────────────────────────────────────
#ifdef __ANDROID__

// Called every frame — polls the socket and drives the protocol state machine.
// Replaces the async WSAAsyncSelect + WM_SOCKET handler in WINHANDLE.cpp.
void PollSocketIO();

#else
// On Windows, PollSocketIO is a no-op (WSAAsyncSelect handles it).
inline void PollSocketIO() {}
#endif
