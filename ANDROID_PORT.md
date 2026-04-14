# Android Port — Status de Progresso

Port do cliente MU Online (Win32/OpenGL) para Android (NativeActivity/OpenGL ES 3.0).
- **Min API:** Android 8.0 (API 26)
- **ABI:** arm64-v8a + armeabi-v7a
- **Game data:** Baixado na 1ª execução via downloader integrado
- **Áudio:** OpenSL ES

Atualizar os checkboxes conforme cada item for concluído.

---

## Progresso Geral

| Fase | Componente | Status |
|------|-----------|--------|
| 0 | Build System (Gradle + CMake) | ✅ Completo |
| 1 | Platform Abstraction Layer (PAL) | ✅ Completo |
| 2 | OpenGL ES 3.0 Render Backend | 🔄 Em andamento |
| 3 | Input System (Touch → Mouse) | 🔄 Em andamento |
| 4 | Áudio (OpenSL ES) | 🔄 Em andamento |
| 5 | Font/Text Rendering (stb_truetype) | 🔄 Em andamento |
| 6 | Win32 System Replacements | 🔄 Em andamento |
| 7 | Game Data Downloader (1ª execução) | 🔄 Em andamento |
| 8 | Integração & Testes por Cena | ⬜ Pendente |

---

## Fase 0 — Build System Android

- [x] `Main/android/settings.gradle`
- [x] `Main/android/build.gradle`
- [x] `Main/android/gradle.properties`
- [x] `Main/android/app/build.gradle` (minSdk 26, externalNativeBuild CMake)
- [x] `Main/android/app/src/main/AndroidManifest.xml`
- [x] `Main/android/app/src/main/cpp/CMakeLists.txt` (~1200 source files)
- [x] `Main/android/app/src/main/cpp/android_main.cpp` (entry point)
- [x] `Main/android/run_adb_debug.sh` (com fallback para `gradle` quando `gradlew` não existir)
- [x] Build limpo sem erros: `./gradlew assembleDebug` (requer NDK + 3rd-party libs)

---

## Fase 1 — Platform Abstraction Layer (PAL)

### Win32 / Sistema
- [x] `Platform/AndroidWin32Compat.h` — HWND, DWORD, BOOL, VK_*, WM_*, structs Win32, INI functions
- [x] `Platform/WinsockAndroidCompat.h` — WSAStartup, closesocket, SOCKET shims
- [x] `Platform/AndroidNetworkPollCompat.h/cpp` — PollSocketIO() via poll()
- [x] `Platform/LegacyClientRuntime.h/cpp` — g_hWnd, g_hDC, RandomTable globals
- [x] `Platform/LegacyAndroidClientStubs.cpp` — CheckHack, Nprotect, ShareMemory stubs
- [x] `Platform/Win32SecondaryStubs.h` — CreateDIBSection, BitBlt stubs

### Janela / Contexto
- [x] `Platform/AndroidEglWindow.h/cpp` — EGL context RGBA8888 + depth24, ES 3.0
- [x] `Platform/GameAssetPath.h/cpp` — getExternalFilesDir(), fopen_ci()

### Config / Bootstrap
- [x] `Platform/GameConfigBootstrap.h/cpp` — CProtect + Configs.xtm
- [x] `Platform/GamePacketCryptoBootstrap.h/cpp` — Enc1.dat/Dec2.dat
- [x] `Platform/GameConnectServerBootstrap.h/cpp` — lista de servidores

### Modificações em arquivos existentes
- [x] `StdAfx.h` — `#ifndef __ANDROID__` em volta de windows.h, WinSock2, mmsystem, imm
- [x] `CGMProtect.cpp` — desabilitar Nprotect, Themida, IsRunningInVM no Android
- [x] `Nprotect.h` — stubs Android para todas as funções (LaunchNprotect/FindNprotectWindow/etc.)
- [x] `ProtocolAsio.h` — guard `_WIN32_WINNT` para Android (já existia)
- [x] `Winmain.cpp` — `#ifndef __ANDROID__` em volta de WinMain() inteiro
- [x] `WINHANDLE.h/cpp` — `#ifndef __ANDROID__` em toda a classe
- [x] `SpinLock.h` — CriticalSectionLock → std::mutex no Android
- [x] `Time/Timer.h/cpp` — QPC/timeGetTime → clock_gettime(CLOCK_MONOTONIC)

---

## Fase 2 — OpenGL ES 3.0 Render Backend

> **Maior tarefa do port.** Emular o pipeline fixed-function usando shaders GLES3.

- [x] `Platform/GLFixedFunctionStubs.h` — redefine glBegin/glEnd, glVertex*, glColor*, glNormal*, glTexCoord*, glMatrixMode, glPushMatrix, glPopMatrix, glRotatef, glTranslatef, glScalef, glLoadIdentity, gluPerspective, gluLookAt, glLightfv, glFogf/fv, glAlphaFunc
- [x] `Platform/RenderStateCompat.h` — estado da state machine OpenGL (enable/disable flags, blend, depth, fog, light)
- [x] `Platform/OpenGLESRenderBackend.h/cpp` — VBO temporário + flush no glEnd(), matrix stack com GLM
- [x] `Platform/RenderBackend.h/cpp` — interface pública (BeginFrame, EndFrame, SwapBuffers)
- [x] `android/app/src/main/assets/shaders/fixed_vert.glsl` — a_position, a_texcoord, a_color, a_normal + iluminação por vértice
- [x] `android/app/src/main/assets/shaders/fixed_frag.glsl` — textura + fog + alpha test
- [x] `ZzzOpenglUtil.cpp` — includes GLEW/GL cobertos via StdAfx.h → GLFixedFunctionStubs.h
- [ ] Verificar `ZzzBMD.cpp` — adaptar rendering de modelos 3D ao backend
- [ ] Verificar `ZzzLodTerrain.cpp` — adaptar terrain rendering
- [ ] Verificar `ZzzEffect*.cpp` — adaptar particle/effect rendering

---

## Fase 3 — Input System

- [x] `Platform/GameMouseInput.h/cpp` — AInputEvent → MouseX/Y, LButton, RButton, DBClick, wheel
  - Tap → LButtonDown+Up
  - Long press → RButton
  - Double-tap → DBClick
  - Drag → mouse move com botão pressed
  - Pinch (2 dedos) → zoom câmera
- [x] `Input.cpp` — `#ifdef __ANDROID__` com cursor explícito via globals `MouseX/MouseY` (bridge GameMouseInput)
- [x] `WINHANDLE.cpp` — `#ifndef __ANDROID__` em WndProc() e message loop
- [x] Soft keyboard — bridge JNI `showSoftKeyboard()/hideSoftKeyboard()` acionado automaticamente via `SetFocus()` quando foco entra/sai de controles `edit`
- [x] VK_* mapeados em AndroidWin32Compat.h para AKEYCODE_*

---

## Fase 4 — Áudio (OpenSL ES)

### wzAudio (BGM — MP3/OGG streaming)
- [x] `Platform/AudioAndroidStub.cpp` — stubs silenciosos temporários (wzAudioCreate/Play/Stop)
- [x] `Platform/AudioOpenSLES.h/cpp` — implementação real via OpenSL ES URI player (MP3/OGG)
  - `PlayBGM(filename, repeat)` → URI-based + SLSeekItf para loop
  - `StopBGM/PauseBGM/ResumeBGM/SetBGMVolume`
- [x] Ativar bridge AudioAndroidStub → AudioOpenSLES removendo AudioAndroidStub do CMakeLists
- [ ] Adicionar minimp3 (header-only) em `Main/dependencies/include/minimp3/` (decode offline)

### DirectSound (SFX — WAV buffers)
- [x] `Platform/AudioOpenSLES.h/cpp` — implementação real via OpenSL ES PCM buffer players
  - `LoadWAV(filename)` → parse WAV header + PCM buffer
  - `PlaySFX/StopSFX/SetSFXVolume` — até 16 canais simultâneos
  - `StopBuffer(index)` → SL_PLAYSTATE_STOPPED
  - `Set3DSoundPosition()` → SL3DLocationItf
  - `SetVolume()` → SLVolumeItf
- [x] `DSplaysound.cpp` — `#ifndef __ANDROID__` em volta de toda implementação DirectSound

---

## Fase 5 — Font/Text Rendering

- [x] `Platform/AndroidTextRenderer.h/cpp` — stb_truetype (já disponível em ImGui/imstb_truetype.h)
  - Carrega `/system/fonts/NotoSans-Regular.ttf` (fallback chain)
  - Atlas de glyphs lazy (gerado sob demanda)
  - `TextOut/TextOutW`, `GetTextExtentPoint32/W`, `CreateFont`, `UploadTextBitmap`
- [ ] `CGMFontLayer.cpp` — `#ifdef __ANDROID__`: substituir bloco GDI (GetFontData, GetTextMetricsW, HDC) por AndroidTextRenderer
- [ ] Bundlar fonte TTF em `android/app/src/main/assets/fonts/NotoSans-Regular.ttf`

---

## Fase 6 — Win32 System Replacements

### Threading (~20 arquivos)
- [x] `SpinLock.h` — `#ifdef __ANDROID__` usar std::mutex em vez de CRITICAL_SECTION
- [x] `Winmain.cpp` — já dentro de `#ifndef __ANDROID__`
- [x] `GameShop/FileDownloader/FileDownloader.cpp` — `_beginthreadex` → `std::thread`
- [x] `GameShop/ShopListManager/ListManager.cpp` — `_beginthreadex` → `std::thread`
- [x] `Utilities/Dump/CrashHandler.cpp` — inteiro dentro de `#ifndef __ANDROID__`
- [x] `Utilities/Dump/Uploader.cpp` — inteiro dentro de `#ifndef __ANDROID__`

### Timers
- [x] `Time/Timer.cpp` — `#ifdef __ANDROID__`: clock_gettime(CLOCK_MONOTONIC)
- [x] `Time/Timer.h` — `__int64` → `int64_t`, guardas `#ifndef __ANDROID__`
- [x] `GetTickCount()` / `timeGetTime()` — shimados em AndroidWin32Compat.h
- [x] `timeBeginPeriod(1)` / `timeEndPeriod(1)` → no-op em AndroidWin32Compat.h

### Registry
- [x] `ExternalObject/Leaf/regkey.h` — Android stub: lê/escreve arquivo flat key=value em registry/<subkey>.reg

### INI files
- [x] `GetPrivateProfileInt/String` — implementados em AndroidWin32Compat.h via POSIX fopen parser (25 usages cobertas)
- [x] `WritePrivateProfileString` — stub retorna TRUE

### File I/O
- [x] `CGMProtect.cpp` — CreateFile/ReadFile → fopen/fread
- [ ] `ZzzAI.cpp` — idem (verificar se usa CreateFile)
- [x] `_access()` → `access()` (POSIX, disponível no NDK)

### Font
- [x] `CGMFontLayer.cpp` — `runtime_font_property(HDC,HFONT,...)` guarda com `#ifndef __ANDROID__`; no Android usa fallback file-based FreeType path

### Outros
- [x] `Utilities/Dump/CrashHandler.cpp` / `Uploader.cpp` — `#ifndef __ANDROID__`
- [x] `ExternalObject/Leaf/ExceptionHandler.cpp` — `#ifndef __ANDROID__`
- [x] `ExternalObject/Leaf/checkintegrity.cpp` / `peimage.cpp` — `#ifndef __ANDROID__`
- [x] `DSwaveIO.h/cpp` — `#ifndef __ANDROID__`
- [x] `MovieScene` / `dshow.h` — já dentro de `#ifdef MOVIE_DIRECTSHOW`
- [x] `GCCertification.cpp` / `LauncherHelper.cpp` / `UsefulDef.cpp` — guards adicionados
- [x] `Util.cpp` — UUID/RPC → Android stub
- [x] `create_hwid_system()` — Android stub (placeholder ID)
- [x] `AndroidWin32Compat.h` — CreateFile/ReadFile/WriteFile/CloseHandle shims via fopen
- [x] `AndroidWin32Compat.h` — GetCursorPos/ScreenToClient → MouseX/MouseY globals
- [x] `AndroidWin32Compat.h` — sprintf_s/vsprintf_s/wsprintf → snprintf shims
- [x] `AndroidWin32Compat.h` — GetSystemInfo/GetVolumeInformation stubs
- [x] `AndroidWin32Compat.h` — GWL_WNDPROC/GWL_USERDATA, WM_IME_*, IMN_SETOPENSTATUS, GetWindowTextW, GetCaretPos
- [x] `AndroidWin32Compat.h` — OffsetRect, _vsntprintf, GetKeyState
- [x] `regkey.h` — Android flat-file backend
- [x] `GameMouseInput.cpp` — bridge Fire*() → CNewKeyInput::SetKeyState(VK_LBUTTON/RBUTTON)
- [x] `Main/android/app/src/main/cpp/CMakeLists.txt` — remover `CBTMessageBox.cpp` do build Android

---

## Fase 7 — Game Data Downloader (1ª Execução)

- [x] `Platform/GameDownloader.h/cpp`
  - [x] Detecta se game data existe em `getExternalFilesDir()`
  - [x] Exibe tela de download com progresso (renderizada via GLES3)
  - [x] Baixa arquivos do servidor de assets reutilizando `GameShop/FileDownloader` + `HTTPConnecter`
  - [x] Suporta manifesto remoto/local de assets (`Data/assets-manifest.txt`) com fallback para lista mínima
  - [x] Parser de manifesto tolerante a formatos legados/comuns (`path|crc`, `crc path`, `path=...`, `crc=...`) e colunas extras
  - [x] Download de manifesto com escrita segura (arquivo temporário + troca para destino final após parse válido)
  - [x] Verifica integridade CRC32 (sidecar opcional `<arquivo>.crc32` via `Util/CCRC32.H`)
  - [x] Refresh periódico do manifesto local (6h) para forçar rechecagem remota de assets
  - [x] Fluxo de pacote compactado `.zip` + extração para `getExternalFilesDir()` via JNI (`MainActivity.extractZipArchive`) com marcador `.extracted`
- [x] Definir URL base do servidor de assets por parâmetro de execução (`MU_ASSET_SERVER` → intent extra `MU_ASSET_SERVER`)
- [x] Publicar template de manifesto de produção no repositório (`docs/assets-server/Data/assets-manifest.txt`)
- [x] Adicionar ferramenta para geração automática do manifesto de produção com CRC (`tools/generate_assets_manifest.sh`)
- [ ] Publicar manifesto completo de arquivos (hoje: lista mínima + suporte a manifesto)

---

## Fase 8 — Integração & Testes

### Por Cena (validar em ordem)
- [ ] Boot: `OpenBasicData()` carrega dados sem crash (WEBZEN_SCENE)
- [ ] Login: tela renderiza, campos de texto funcionam, socket conecta (LOG_IN_SCENE)
- [ ] Server select: lista de servidores aparece, seleção funciona
- [ ] Character select: lista renderiza com modelos 3D
- [ ] Character creation: modelos de criação renderizam
- [ ] Main scene: terreno, personagem, UI in-game (MAIN_SCENE)

### Verificação técnica
- [ ] Framerate estável ≥ 30 FPS em device mid-range
- [ ] Sem memory leaks (usar Android Studio Profiler)
- [ ] Sem ANR (Application Not Responding)
- [ ] Rotação de tela desabilitada (portrait fixo)
- [ ] Back button fecha o jogo corretamente

---

## Comandos de Build & Debug

```bash
# Build debug
cd Main/android && ./gradlew assembleDebug

# Build + install + launch
./Main/android/run_adb_debug.sh

# Build limpo com download de game data
MU_ASSET_SERVER=http://seu-servidor.com ./Main/android/run_adb_debug.sh --clean

# Gerar manifesto de produção (assets server)
tools/generate_assets_manifest.sh --asset-root /caminho/asset-server-root --output /caminho/asset-server-root/Data/assets-manifest.txt --zip-extract Data

# Logs
adb logcat -s MUAndroid:V          # log geral
adb logcat -s MURender:V           # render backend GLES3
adb logcat -s MUAudio:V            # OpenSL ES
adb logcat -s MUNetwork:V          # protocolo de rede
adb logcat -s MUAssets:V           # download e carregamento de assets
```

---

## Problemas Conhecidos / TODOs

- **CSimpleModulus**: atualmente stub pass-through. Implementar crypto real se o servidor exigir.
- **IME (texto unicode)**: campos de texto precisam de integração completa com Android InputMethodManager via JNI para suporte a caracteres especiais.
- **Inline `__asm`**: ~12 arquivos têm blocos `__asm` que não compilam no ARM. Identificar e substituir por equivalentes C++.
- **glprocs.lib**: funções proprietárias de extensão OpenGL. Avaliar quais são usadas e se têm equivalente ES3.
- **ShareMemory.lib**: lib de memória compartilhada proprietária. Stub no Android.
- **wzAudio.lib**: lib Windows proprietária. Reimplementar API via OpenSL ES (Fase 4).

---

*Última atualização: 2026-04-14 — downloader Android integrado ao código original (`GameShop/FileDownloader` + `HTTPConnecter`) com progresso em tela, compat WinINet HTTP ativa no Android, ajuste no loop de transferência (`TransferRemoteFile`), suporte a manifesto remoto/local de assets (`assets-manifest.txt`) com parser robusto + escrita segura em `.tmp` e refresh periódico (6h), validação CRC32 por sidecar opcional (`.crc32`), fluxo de pacote `.zip` + extração JNI (`MainActivity.extractZipArchive`) com marcador `.extracted`, template de manifesto de produção publicado em `docs/assets-server/Data/assets-manifest.txt`, ferramenta de geração automática (`tools/generate_assets_manifest.sh`), override de servidor via `MU_ASSET_SERVER` (intent extra) e builds `buildCMakeDebug[arm64-v8a]` + `assembleDebug` validados*
