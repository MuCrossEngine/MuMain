# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Project Is

A Windows Win32/OpenGL MMORPG game client for MU Online ("Mu Cross Engine"), written in C++. Target platform is Windows x86 only. The codebase is ~1,200 source files in `Main/source/`.

## Build

Open `Main/Main.sln` in Visual Studio and build via the IDE or MSBuild:

```
# Debug (toolset v143, outputs to ../../../../Client_2)
msbuild Main\Main.sln /p:Configuration=Debug /p:Platform=Win32

# Release (toolset v142, outputs to Main\Release)
msbuild Main\Main.sln /p:Configuration=Release /p:Platform=Win32
```

All dependencies are vendored in `Main/dependencies/` — no external package manager. No test suite; validation is manual.

## Architecture

### Entry Point and Game Loop

`WinMain()` at `Main/source/Winmain.cpp:1061` initializes OpenGL context, fonts, audio (`wzAudio`), and protection systems, then drives the main loop.

Scene dispatch lives in `Main/source/ZzzScene.cpp`. The global `SceneFlag` determines the active scene:

- `WEBZEN_SCENE` — startup/data loading via `OpenBasicData()`
- `LOG_IN_SCENE` — login screen
- `CHARACTER_SCENE` — character selection/creation
- `MAIN_SCENE` — in-game play

Each frame runs: input → network polling (`WSclient`) → `ProtocolCompiler()` → object/character updates → `RenderScene()` → UI overlay.

### Rendering

- `ZzzOpenglUtil.cpp` — OpenGL state management and utility functions
- `CShaderGL.cpp` — GLSL shader management
- `ZzzBMD.cpp` — loads BMD skeletal 3D model format (characters, objects)
- `ZzzLodTerrain.cpp` — LOD terrain mesh rendering
- `ZzzTexture.cpp` / `GlobalBitmap.cpp` — texture loading (OZJ/OZT formats, JPEG-based)
- `ZzzEffect.cpp` + `ZzzEffect*.cpp` — particles, joints, magic effects
- `ZzzCharacter.cpp` / `CGMCharacter.cpp` — character skeletal animation and rendering

### UI System

Two parallel UI layers exist:

1. **Legacy UI** (`UIMng.cpp`, `Win.cpp`, `WinEx.cpp`, `UIControls.cpp`) — older window-based widgets used for login/character scenes
2. **New UI** (`NewUIManager.cpp` + ~80 `NewUI*.cpp` files) — modern UI used for in-game HUD, inventory, shop, skills, quests, chat, etc.

`NewUIManager` owns all `NewUIBase`-derived windows and routes messages to them. `UIMng` manages the legacy scene windows.

### Networking

- `WSclient.cpp` / `wsctlc.cpp` — socket management, packet send/receive loop
- `Protocol.cpp` / `ProtocolSend.cpp` — game protocol packet parsing and dispatch
- `SocketSystem.cpp` — low-level socket wrapper
- `MuCrypto/MuCrypto.cpp` — client-side packet encryption
- Connection flow: Connect Server → Login Server → Game Server

### Key Subdirectories Under `Main/source/`

| Path | Purpose |
|------|---------|
| `ExternalObject/Chilkat/` | Chilkat library headers (crypto, HTTP, FTP, networking) |
| `ExternalObject/Leaf/` | Exception handler, crash dump, PE integrity checks |
| `ExternalObject/ResourceGuard/` | Anti-cheat/protection hooks |
| `GameShop/` | In-game shop UI + HTTP/FTP item list downloader |
| `Math/` | `ZzzMathLib` — vector/matrix math |
| `MuCrypto/` | Packet crypto |
| `Time/` | `Timer`, `CTimCheck` — frame timing utilities |
| `Utilities/Dump/` | Crash reporter and minidump uploader |
| `Utilities/Log/` | `ErrorReport`, `muConsoleDebug`, debug logging |
| `Utilities/Memory/` | `HashTable`, `MemoryLock` |

### Map/World Modules

Each game zone has a dedicated `GM*.cpp` file (e.g. `GMAida.cpp`, `GMHellas.cpp`, `GMBattleCastle.cpp`). They handle zone-specific rendering, NPC scripting, and event logic. `w_MapProcess.cpp` / `w_MapHeaders.h` coordinate map transitions.

### Precompiled Header

`StdAfx.h` / `StdAfx.cpp` is the precompiled header. All source files must `#include "stdafx.h"` as the first include.
