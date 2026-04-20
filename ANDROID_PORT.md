# Android Port — Status de Progresso

Port do cliente MU Online (Win32/OpenGL) para Android (NativeActivity/OpenGL ES 3.0).
- **Min API:** Android 8.0 (API 26)
- **ABI:** arm64-v8a + armeabi-v7a
- **Game data:** Embutido em `assets/Data` (cópia local no bootstrap); downloader HTTP adiado para a fase final
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
| 7 | Game Data Downloader (1ª execução) | ⬜ Adiado (último) |
| 8 | Integração & Testes por Cena | 🔄 Em andamento |

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
- [x] `Platform/GLFixedFunctionStubs.h` — emulação de client arrays (`glEnableClientState`, `gl*Pointer`, `glDrawArrays`) para fluxos legados de render
- [x] `Platform/GLFixedFunctionStubs.h` — compat de `glDrawElements` com client arrays e tratamento de `GL_QUADS` para caminhos legados
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
- [x] `ZzzAI.cpp` — `WriteDebugInfoStr` com fallback Android (`fopen/fwrite`) em vez de `CreateFile/WriteFile`
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
- [x] `create_hwid_system()` — implementação Android ligada ao helper JNI de HWID determinístico (`getAndroidHardwareId`); validação final de runtime ainda pendente nesta rodada
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

> **Status atual:** adiado para a última etapa. No fluxo atual, o app usa `Data/` embutido em `android/app/src/main/assets/Data` e copia localmente no bootstrap, sem download HTTP.

- [x] Bootstrap local otimizado: cópia de `assets/Data` apenas na 1ª execução (ou se faltarem arquivos mínimos), com marcador `.data_assets_copied`

- [ ] `Platform/GameDownloader.h/cpp` (refazer por último, após integração completa)
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
- [x] Gerar manifesto completo de arquivos em `Data/assets-manifest.txt`
- [x] Estabilizar transporte HTTP Android para arquivos grandes no runtime (streaming no WinINet compat + fallback sem `Content-Length`)

---

## Fase 8 — Integração & Testes

### Por Cena (validar em ordem)
- [x] Boot: `OpenBasicData()` carrega dados sem crash (WEBZEN_SCENE) e avanca para `LOG_IN_SCENE`
- [ ] Login: render e conexao inicial validados; faltam validacoes finais de input/texto e fluxo completo de UI (LOG_IN_SCENE)
- [ ] Server select: lista de servidores aparece, seleção funciona
- [ ] Character select: lista renderiza com modelos 3D
- [ ] Character creation: modelos de criação renderizam
- [ ] Main scene: terreno, personagem, UI in-game (MAIN_SCENE)
- [x] Crash hardening inicial em `MainScene`: removidos aborts imediatos por formatação insegura de strings (`FORTIFY`) no Android
- [x] Instrumentação Android de cena em `ZzzScene.cpp` (`MUScene`): transição, primeira tentativa de render, primeiro sucesso e falhas com contagem

### Verificação técnica
- [ ] Framerate estável ≥ 30 FPS em device mid-range
- [ ] Sem memory leaks (usar Android Studio Profiler)
- [ ] Sem ANR (Application Not Responding)
- [x] Rotação de tela desabilitada (orientação fixa em `landscape` no AndroidManifest)
- [x] Back button fecha o jogo corretamente *(validado via ADB em 2026-04-20: `pidof` ativo antes e vazio após `input keyevent 4`)*

### Última validação (2026-04-16)

- Auditoria de documentação x código executada. Resultado:
  - build Android continua validado localmente (`assembleDebug`) ✅
  - endpoint Android fixo `74.63.218.132:44404` continua implementado no bootstrap ✅
  - envio de `SendRequestServerHWID()` e `SendRequestServerList()` continua presente no protocolo ✅
  - recepção `ReceiveServerList total=%d` continua instrumentada em runtime ✅
- Divergências encontradas que ainda **não** podem ser marcadas como concluídas:
  - validação final de runtime do HWID JNI ainda não foi refeita após a correção de código desta rodada ❌
  - validação final de runtime do fallback para grupos ausentes no `ServerList.bmd` ainda não foi refeita após a correção de código desta rodada ❌
  - shim de polling/reconnect do login Android está presente no `ZzzScene.cpp`; falta apenas revalidação final de runtime nesta rodada ❌
- Estado atual do bloqueio funcional:
  - fluxo `WEBZEN_SCENE` -> `LOG_IN_SCENE` e base de rede seguem estruturados, mas a validação completa de `server select`/`character scene` permanece pendente no estado atual do código.

### Atualizações da rodada 2026-04-16 (código)

- `Util.cpp`
  - `create_hwid_system()` agora consulta `MainActivity.getAndroidHardwareId()` via JNI em vez de manter placeholder fixo no C++.
- `ServerListManager.cpp`
  - `MakeServerGroup()` agora cria grupo fallback quando o índice não existe em `ServerList.bmd`, preservando compatibilidade com servidores privados/custom.
- Estado desta rodada:
  - implementação de código concluída;
  - `get_errors` sem erros nas unidades alteradas;
  - validação final em runtime ainda pendente.

### Atualizações da rodada 2026-04-20 (código)

- `Main/source/Platform/MuCryptoStub.cpp`
  - substituído stub pass-through por implementação real de `MuCrypto` (mesmo fluxo de algoritmos/modulus do cliente legado).
- `Main/android/app/src/main/cpp/CMakeLists.txt`
  - integrada árvore `Main/dependencies/cryptopp-src` no build Android.
  - corrigido filtro de fontes do Crypto++ para **manter** `dll.cpp`/`default.cpp` (instanciações de template exigidas no link).
- validação técnica:
  - `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon` concluído com sucesso após os ajustes.
- tentativa adicional nesta rodada:
  - `CGMFontLayer.cpp` recebeu caminho Android com fallback para fontes por arquivo (`GameAssetPath` + `/system/fonts`), mas o build Android ainda não pode migrar do `CGMFontLayerStub.cpp` sem integrar FreeType no CMake Android (símbolos `FT_*` indefinidos no link).
  - conclusão: item de fonte/render de texto continua pendente na Fase 5 até integração formal do FreeType no Android.
- `Platform/AndroidWin32Compat.h` + `Platform/Win32SecondaryStubs.h`
  - reforçada a ponte GDI Android (`CreateFont`, `CreateCompatibleDC`, `CreateDIBSection`, `SelectObject`, `GetCurrentObject`) para registrar estado real de `HDC/HBITMAP/HFONT` e encaminhar para `AndroidTextRenderer`.
  - `DeleteObject` agora libera buffers de `DIBSection` registrados no Android e remove vínculo de bitmaps nos DCs.
- `Platform/CGMFontLayerStub.cpp`
  - fallback Android agora renderiza via `g_pRenderText` (em vez de retorno apenas métrico), reaproveitando o pipeline de texto já conectado ao `AndroidTextRenderer`.
- `Main/android/app/src/main/cpp/android_main.cpp`
  - `AndroidTextRenderer::Init()` adicionado no bootstrap e `AndroidTextRenderer::Shutdown()` no cleanup do `android_main`.
  - tratamento explícito de `AKEYCODE_BACK` no callback `OnInputEvent`: consome o evento e chama `ANativeActivity_finish()` para fechamento consistente no Android.
- `Platform/AndroidWin32Compat.h`
  - transporte HTTP do WinINet compat mudou para modo streaming (`HttpSendRequest` + `InternetReadFile`), reduzindo uso de memória em downloads grandes.
  - fallback buffered mantido quando a resposta chega em `Transfer-Encoding: chunked`.
- `GameShop/FileDownloader/HTTPConnecter.cpp`
  - `OpenRemoteFile()` agora aceita ausência de `Content-Length` e permite download até EOF (compatível com respostas streamed/chunked).
- validação técnica complementar:
  - `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon` permanece **verde** após os ajustes da ponte GDI e do transporte HTTP.
  - validação runtime de back button via ADB concluída (`PID_BEFORE` presente e `PID_AFTER` vazio após `adb shell input keyevent 4`).

### Validação anterior (2026-04-15)

- Build Android: `./gradlew :app:assembleDebug` ✅
- Endpoint de login Android validado em runtime: `74.63.218.132:44404` ✅
- Teste de conectividade de porta realizado em host e device (`74.63.218.132:44404`) ✅
- Fluxo de cena continua estável sem crash fatal imediato no bootstrap/login (processo vivo após launch) ✅
- Rodada de paridade com `Main/sourceOld` no fluxo login/server-list aplicada com shim mínimo Android (polling/reconnect controlado) ✅
- Estado atual do bloqueio funcional:
  - recepção da server list (`ReceiveServerList total=...`) mostrou comportamento intermitente em janelas curtas de validação pós-limpeza;
  - próxima ação é consolidar uma janela de validação end-to-end (server select -> character scene) com os ajustes finais já aplicados.

### Validação anterior (2026-04-14)

- Build Android: `./gradlew assembleDebug` ✅
- Install/launch via ADB: ✅
- Instalação limpa validada com `adb shell pm clear com.mucrossengine.client` ✅
- Evidência de runtime coletada:
  - `MUScene: Scene transition: UNKNOWN_SCENE (-9999) -> WEBZEN_SCENE (1)`
  - `MUScene: Scene transition: WEBZEN_SCENE (1) -> LOG_IN_SCENE (2)`
  - `MUScene: LOG_IN_SCENE: SceneLogin=0 detected, forcing Android compatibility mode=1`
  - `MUScene: LOG_IN_SCENE: forced initial login UI visibility (LoginMainWin + ServerSelWin)`
  - `MUScene: LOG_IN_SCENE: ReconnectCreateConnection success (...), requested server list`
  - `MUScene: First successful render in LOG_IN_SCENE`
  - `pidof com.mucrossengine.client` retornando PID ativo apos janela adicional de execucao
- Correções validadas nesta rodada:
  - fallback de compatibilidade no login Android forçando `SceneLogin=1` para evitar fechamento por erro de `LoadWorld()` em mapas de login custom;
  - restauração do render da tela de loading/título no Android (`RenderTitleSceneUI` sem swap direto no caminho `hDC=null`);
  - fallback de visibilidade inicial da UI de login (`LoginMainWin` + `ServerSelWin`);
  - robustez de send em socket não bloqueante (`WSAENOTCONN/WSAEINPROGRESS/WSAEALREADY` tratados como transitórios).
- Estado atual: bootstrap + login render estaveis sem crash fatal imediato; bloqueio funcional restante esta em fluxo completo de login/server-select/character.

### Atualizações da rodada 2026-04-15

- `Platform/LegacyBootstrapStubs.cpp`
  - endpoint Android fixado para `74.63.218.132:44404` e log explícito de endpoint aplicado em runtime.
- `ZzzScene.cpp`
  - refinamentos no caminho de `CreateLogInScene`/`NewMoveLogInScene` para alinhar com `sourceOld` mantendo compatibilidade Android mínima de request/reconnect da server list.
  - limpeza de logs de diagnóstico temporários e ajuste fino das condições de polling Android.
- `WSclient.cpp`
  - mantido log Android de confirmação de retorno da server list (`ReceiveServerList total=%d`) para validação de campo.

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

- **MuCrypto/CSimpleModulus**: implementação real já integrada no Android (Crypto++); manter apenas validação final de compatibilidade com servidor alvo.
- **IME (texto unicode)**: campos de texto precisam de integração completa com Android InputMethodManager via JNI para suporte a caracteres especiais.
- **Inline `__asm`**: ~12 arquivos têm blocos `__asm` que não compilam no ARM. Identificar e substituir por equivalentes C++.
- **glprocs.lib**: funções proprietárias de extensão OpenGL. Avaliar quais são usadas e se têm equivalente ES3.
- **ShareMemory.lib**: lib de memória compartilhada proprietária. Stub no Android.
- **wzAudio.lib**: lib Windows proprietária. Reimplementar API via OpenSL ES (Fase 4).

---

*Última atualização: 2026-04-20 — integração de `MuCrypto` real no Android concluída com inclusão do Crypto++ no build nativo e validação de compilação `arm64-v8a`; transporte HTTP Android também foi estabilizado em modo streaming para arquivos grandes, com fallback para respostas chunked e compatibilidade sem `Content-Length`. O endpoint Android `74.63.218.132:44404`, o envio de HWID e o fluxo base até `LOG_IN_SCENE` permanecem como direção ativa. Pendências práticas: revalidação de runtime (HWID/fallback server group) e progressão funcional completa de `server select` até `CHARACTER_SCENE`, além da migração final do texto Android após integração de FreeType.*
