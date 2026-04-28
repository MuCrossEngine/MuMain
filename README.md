# Mu Cross Engine

## Visao geral

Repositorio principal do cliente MU Online baseado em C++ com foco no `Main` legado de Windows e continuidade de portabilidade para Android via APK nativo.

## Estrutura principal

- `Main/`: codigo-fonte principal do cliente, solucao Visual Studio e projeto Android.
- `Main/source/`: codigo legado do cliente, incluindo renderizacao, rede, UI e sistemas do jogo.
- `Main/android/`: projeto Gradle/CMake do port Android.
- `Util/`: bibliotecas utilitarias e dependencias auxiliares do cliente.
- `docs/`: documentacao tecnica do repositorio em pt-BR.

## Estado atual do port Android

O projeto ja possui base Android com:

- Projeto Gradle.
- App Android com `minSdk 26`.
- Build nativo via CMake.
- Backend OpenGL ES 3.0.
- Camada de compatibilidade Win32 para grande parte do cliente legado.
- Build `debug` Android concluido localmente com geracao de APK.

Os principais bloqueios conhecidos no momento sao:

- Distribuicao de assets já suporta manifesto remoto/local e pacote `.zip` com extração no Android; falta publicar/validar em ambiente de servidor definitivo.
- Validacao CRC foi integrada com sidecar opcional (`.crc32`), mas falta politica final de integridade para todos os pacotes.
- Integracao completa de teclado virtual/IME Android ainda pendente.
- Validacao final em runtime da ligacao C++/JNI do HWID Android ainda pendente.
- Validacao funcional completa de `server select` -> `character scene` ainda pendente.
- Validacao funcional em runtime no dispositivo Android está parcial: fluxo `LoginMain -> ServerSel click -> ReceiveServerConnect -> LoginWin` foi comprovado; falta progressao completa ate `CHARACTER_SCENE`.
- Ajustes adicionais de UX mobile, entrada de texto e distribuicao de assets ainda sao necessarios para considerar o port pronto para uso final.

## Roadmap de execucao

Este roadmap e a visao de alto nivel do projeto. O detalhamento tecnico por arquivo/fase continua em `ANDROID_PORT.md`.

### Politica de manutencao (obrigatoria)

- Toda implementacao nova deve atualizar **os dois arquivos**: `README.md` (roadmap) e `ANDROID_PORT.md` (checklist tecnico).
- Ao concluir uma tarefa, marcar checkbox no `ANDROID_PORT.md` e refletir o impacto no roadmap abaixo.
- Ao iniciar uma tarefa grande, registrar como **Em andamento** no roadmap e detalhar subtarefas no `ANDROID_PORT.md`.
- Nao fechar fase no roadmap sem evidencia de build e validacao runtime (quando aplicavel).

### Fases do roadmap

| Fase | Objetivo | Status atual | Proximo criterio de saida |
|------|----------|--------------|----------------------------|
| R0 | Build e bootstrap Android | ✅ Concluido | APK debug instala e inicia sem crash imediato |
| R1 | Compatibilidade de plataforma (Win32/PAL) | 🔄 Em andamento | Cobrir APIs restantes usadas em runtime principal |
| R2 | Render OpenGL ES (fixed-function emulacao) | 🔄 Em andamento | Login + Character + Main com render funcional basico |
| R3 | Input/IME mobile | 🔄 Em andamento | Texto e foco funcionando em login/chat sem regressao |
| R4 | Audio Android | 🔄 Em andamento | BGM e SFX estaveis em cenas principais |
| R5 | Integracao por cena | 🔄 Em andamento | Fluxo completo: Boot -> Login -> Server Select -> Character -> Main |
| R6 | Downloader/manifesto remoto | ⬜ Adiado (final) | Ativar apenas apos runtime render/input/audio estavel |
| R7 | Hardening e release prep | ⬜ Pendente | FPS alvo, sem ANR/crash e validacao final |

### Curto prazo (proximas iteracoes)

- Consolidar compatibilidade de render legada (`ZzzBMD`, `ZzzLodTerrain`, `ZzzEffect*`).
- Fluxo `WEBZEN_SCENE` -> `LOG_IN_SCENE` e clique/conexao de server list validados; foco atual em consolidar progressao completa para `CHARACTER_SCENE`.
- Manter estabilidade do caminho de login Android sem regressão ao remover instrumentações temporárias.
- Avancar substituicoes pendentes de fonte/GDI para Android.
- Downloader HTTP remoto já foi reativado com estabilização de transporte para arquivos grandes; falta validação E2E no servidor definitivo.

## Build

### Windows

- Solucao: `Main/Main.sln`
- Plataforma principal: `Win32`

### Android

- Projeto Gradle: `Main/android`
- Entrada nativa: `Main/android/app/src/main/cpp/android_main.cpp`
- Manifesto: `Main/android/app/src/main/AndroidManifest.xml`
- APK debug gerado: `Main/android/app/build/outputs/apk/debug/app-debug.apk`
- Exemplo de manifesto de assets para servidor: `docs/assets-server/Data/assets-manifest.txt`
- Gerador de manifesto de produção: `tools/generate_assets_manifest.sh`

Exemplo de geração automática do manifesto a partir do diretório de assets do servidor:

- `tools/generate_assets_manifest.sh --asset-root /caminho/asset-server-root --output /caminho/asset-server-root/Data/assets-manifest.txt --zip-extract Data`

Para reproduzir o build local usado nesta etapa:

- `JAVA_HOME`: `D:\Projetos\MuCrossEngine\MuMain\.tools\jdk17\jdk-17.0.18+8`
- `GRADLE_USER_HOME`: `D:\Projetos\MuCrossEngine\MuMain\Main\android\.gradle-user`
- `ANDROID_USER_HOME`: `D:\Projetos\MuCrossEngine\MuMain\Main\android\.android-user`
- `ANDROID_SDK_ROOT`: instalacao local do Android SDK
- Comando: `gradlew.bat assembleDebug --stacktrace`

## Changelog

### 2026-04-28

- Corrigido crash de tela preta ao fazer login e tentar entrar na tela de seleção de personagem (`CHARACTER_SCENE`):
  - Causa raiz: `CreateCharacterScene()` não tinha fallback Android equivalente ao de `CreateLogInScene()`; sem `Configs.xtm` no Android `SceneCharacter` era 0 (default), acionando `LoadWorld(74) + RenderTerrain()` em `ZzzLodTerrain.cpp` (não adaptado para GLES3).
  - Fix: adicionado `#ifdef __ANDROID__` em `CreateCharacterScene()` que força `gmProtect->SceneCharacter = 1`, usando o path de cartão (sem terrain/world), idêntico ao padrão já usado no login (`CreateLogInScene`).
  - Arquivo alterado: `Main/source/ZzzScene.cpp`.
  - Build `assembleDebug` + `adb install` validados; validação de runtime da tela de seleção de personagem pendente.

### 2026-04-22

- Fluxo Android validado em runtime no emulador, com evidência de marcos de login/rede:
	- `ServerSelWin click ... connectIndex=1`
	- `ReceiveServerConnect: target=74.63.218.132:55901`
	- `ReceiveServerConnect: Android compat open LoginWin after connect`
- Executado sanity pass em estado limpo (sem instrumentação temporária):
	- build `assembleDebug` concluído;
	- `adb install -r` + `pm clear` + cold start concluídos;
	- processo e activity em foreground confirmados;
	- sem assinatura de `FATAL EXCEPTION`, `SIGSEGV` ou `ANR` na rodada.
- Limpeza concluída de toda instrumentação temporária usada apenas para calibração de taps e prova end-to-end.
- Ajustado pipeline de fonte no Android para remover ilegibilidade do fallback tipo bitmap:
	- `Platform/AndroidTextRenderer` passou a respeitar o formato real do `DIBSection` (24bpp/32bpp), evitando escrita fora de layout e degradação visual do texto.
	- `Platform/AndroidWin32Compat` passou a propagar `bytesPerPixel` do bitmap selecionado para o renderer.
- Build nativo Android validado após o ajuste de fonte/render de texto: `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon --stacktrace`.
- Ajustado cálculo de escala em runtime Android para telas com resolução/aspect ratio altos:
	- `android_main.cpp` (`SyncLegacyScreenMetrics`) deixou de aplicar escala uniforme 4:3 no caminho `ScreenType == 1`.
	- efeito esperado: remoção de barras laterais pretas (pillarbox) em dispositivos wide/ultra-wide.

### 2026-04-20

- Integrada a implementação real de `MuCrypto` no Android (`Main/source/Platform/MuCryptoStub.cpp`), substituindo o comportamento pass-through anterior.
- Integrada a árvore `Main/dependencies/cryptopp-src` no CMake Android com ajustes de flags e fontes.
- Corrigida falha de link do Crypto++ mantendo `dll.cpp` e `default.cpp` no build (instanciações de template exigidas).
- Build nativo Android validado localmente após os ajustes: `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon`.
- Tentativa de migrar `CGMFontLayer` real no Android foi documentada, mas a troca definitiva segue pendente até integrar FreeType no CMake Android.
- Reforçada a ponte GDI Android (`AndroidWin32Compat` + `Win32SecondaryStubs`) para conectar `HDC/HBITMAP/HFONT` ao `AndroidTextRenderer`, incluindo ciclo de vida de `DIBSection`.
- `AndroidTextRenderer` agora inicializa no bootstrap Android e encerra no cleanup do `android_main`.
- `CGMFontLayerStub` no Android passou a delegar `RenderText` para `g_pRenderText`, removendo o fallback puramente “métrico” enquanto a versão FreeType continua pendente.
- Implementado tratamento de botão `Back` (`AKEYCODE_BACK`) no `OnInputEvent` Android, finalizando a `NativeActivity` via `ANativeActivity_finish()`; validação final em device permanece pendente.
- Validação em runtime do `Back` concluída via ADB (`pidof` ativo antes e ausente após `adb shell input keyevent 4`).
- Transporte HTTP do WinINet compat no Android foi migrado para modo streaming em `AndroidWin32Compat` (`HttpSendRequest` + `InternetReadFile`), evitando carregar arquivo inteiro na memória para downloads grandes.
- `HTTPConnecter::OpenRemoteFile` passou a aceitar ausência de `Content-Length` e seguir download até EOF, reduzindo falhas com respostas streamed/chunked.
- Build nativo Android segue verde após os ajustes de transporte: `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon`.

### 2026-04-16

- Auditoria de consistência entre documentação e código atual executada.
- Confirmado no código atual: endpoint Android permanece fixo em `74.63.218.132:44404`, envio de `SendRequestServerHWID()`/`SendRequestServerList()` segue presente e o log `ReceiveServerList total=%d` continua instrumentado.
- Corrigida divergência de documentação/código: `create_hwid_system()` agora consulta `MainActivity.getAndroidHardwareId()` via JNI no Android.
- Corrigida divergência de documentação/código: `ServerListManager.cpp` agora cria grupo fallback quando o índice não existe no `ServerList.bmd`.
- Estado real do bloqueio funcional em 16/04: fluxo base até `LOG_IN_SCENE` segue como progresso válido, mas a validação final em runtime desses ajustes e a progressão completa de `server select`/`character scene` ainda permanecem pendentes.

### 2026-04-15

- Atualizado roadmap do `README.md`: Fase R5 (integração por cena) passou para **Em andamento**.
- Endpoint Android de login fixado para `74.63.218.132:44404` no bootstrap (`LegacyBootstrapStubs.cpp`) e validado em runtime via log.
- Rodada de paridade com `Main/sourceOld` aplicada no fluxo de login/server list, mantendo shim mínimo Android para confiabilidade de rede.
- Fluxo de cena segue estável sem crash fatal imediato (`WEBZEN_SCENE` -> `LOG_IN_SCENE`) com build debug validado localmente.
- Estado atual do bloqueio funcional: confirmar estabilidade do receive de server list em todas as execuções após limpeza de logs/instrumentação temporária.

### 2026-04-14

- Adicionado roadmap de execucao neste `README.md` com politica obrigatoria de sincronizacao entre `README.md` e `ANDROID_PORT.md`.
- Evoluida a compatibilidade de render para `GL_QUADS` com emulacao robusta em `glDrawArrays` e continuidade para `glDrawElements` com client arrays no Android.
- Corrigido desligamento incorreto de estado de array em sombras de `ZzzBMD` (`glDisableClientState(GL_VERTEX_ARRAY)`).
- Validado novo ciclo `assembleDebug` + install/launch em adb sem crash fatal imediato.

- Integrado o fluxo de downloader Android ao codigo legado `GameShop/FileDownloader` + `HTTPConnecter`.
- Corrigido o loop de transferencia em `FileDownloader::TransferRemoteFile()` para leitura incremental real do stream remoto.
- Adicionado suporte de integridade CRC32 no downloader Android via sidecar opcional `<arquivo>.crc32` (download, parse e comparacao).
- Conectado override de servidor de assets por runtime: `MU_ASSET_SERVER` em `run_adb_debug.sh` -> `intent extra` -> `MainActivity` -> `android_main.cpp` -> `GameDownloader::SetServerURL`.
- Adicionado suporte a manifesto remoto/local de assets (`Data/assets-manifest.txt`) com fallback para lista minima hardcoded.
- Parser de manifesto fortalecido para formatos legados/comuns (`path|crc`, `crc path`, `path=...`, `crc=...`) e tolerancia a colunas extras.
- Download do manifesto remoto passou a usar arquivo temporario + troca atomica (`.tmp` -> final) para evitar corromper cache local em caso de falha de parse.
- `IsDataReady()` agora aplica refresh periodico do manifesto local (intervalo de 6h), forçando nova verificacao remota de assets sem limpeza manual.
- Implementado fluxo de pacote compactado `.zip` com extração no Android via JNI (`GameDownloader` -> `android_main.cpp` -> `MainActivity.extractZipArchive`).
- Entradas de pacote no manifesto agora podem declarar extração por token (`extract=...`) e usam marcador local `.extracted` para confirmar extração concluída.
- Adicionado template de manifesto de produção em `docs/assets-server/Data/assets-manifest.txt` (formatos aceitos, exemplos de `archive=1`, `extract=...` e CRC).
- Adicionada ferramenta CLI para gerar manifesto completo com CRC32 automaticamente: `tools/generate_assets_manifest.cpp` + wrapper `tools/generate_assets_manifest.sh`.
- Gerado manifesto completo de produção em `Data/assets-manifest.txt` usando a árvore real de assets local (`Data/`) com 2117 entradas.
- Atualizado `ANDROID_PORT.md` com o status atual da Fase 7 e pendencias remanescentes.
- Atualizado o relatorio diario `docs/android-port-compat-ui-2026-04-13.md` para refletir a rodada atual de implementacao.
- Validado build Android com `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon` e `./gradlew assembleDebug --no-daemon`.
- Fortalecido o transporte HTTP no compat WinINet Android com retries internos, timeout de I/O maior, logs detalhados de falha e tratamento de `2xx`.
- Corrigido crash de recursao no stub GL (`glViewport`/`gl*`) e estabilizado fechamento de handles no `FileDownloader`.
- Corrigida validacao de CRC32 no Android para aritmetica estritamente 32-bit (evitando divergencia `unsigned long` 64-bit).
- Adicionada instrumentacao de diagnostico no downloader Android (`MUAssets`) para rastrear erro de transporte em runtime.
- Investigado fallback Java HTTP (`HttpURLConnection`) e bridge JNI para o `MainActivity`; mantido desativado no fluxo final por nao resolver o bloqueio de transporte do arquivo `Data/Custom/NPC/Monster1000.bmd`.
- Bloqueio atual de runtime permanece: abort/timeout de conexao no download de `Monster1000.bmd` no emulador, mesmo com retries.
- Adicionada instrumentacao Android de cena em `ZzzScene.cpp` (`MUScene`) para registrar transicao de cena, primeira tentativa de render, primeiro render bem-sucedido e falhas de render com contagem.
- Validada execucao Android com evidencia de transicao para `WEBZEN_SCENE` e tentativa de render sem crash fatal imediato.
- Novo bloqueio rastreado por log: `Render*` da `WEBZEN_SCENE` retorna `false` no ciclo inicial (pendente correcao para avancar a Fase 8).
- Corrigido fluxo Android para usar o dispatcher `Scene()` (em vez de chamar `MainScene()` direto), destravando a sequencia real de cenas no runtime.
- Adicionado hardening em `CSprite::Create` para fallback sem textura quando `FindTexture` retorna ponteiro nulo no Android.
- Ajustado carregamento de `.OZJ/.OZT` em `GlobalBitmap.cpp` para abrir via resolvedor de path Android (`GameAssetPath`).
- Corrigida recursao de bind de textura no backend GLES (`glBindTexture` <-> `GLESFF::BindTexture`) com bind direto ao driver, removendo assinatura de stack overflow observada anteriormente.
- Nova validacao adb (2026-04-14): bootstrap completo, entrada em `WEBZEN_SCENE` confirmada e processo permanece ativo (`pidof`), com proximo foco em avancar ate `LOG_IN_SCENE`.
- Nova validacao adb (2026-04-14): transicao `WEBZEN_SCENE` -> `LOG_IN_SCENE` confirmada sem `SIGSEGV` imediato e processo estavel por janela de 45s.
- Corrigidos null dereferences de bootstrap/login em `CGMCharacter`, `CDirection::HeroFallingDownInit` e criacao de login (`Hero` lazy-init).
- Adicionados guards de null no `OptionWin` para evitar crash quando backend de opcoes do NewUI nao esta disponivel no caminho Android atual.
- Bloqueio atual de UX: tela permanece preta no login apesar de processo vivo e sem crash fatal; proximo passo e instrumentar/renderizar heartbeat visual no `LOG_IN_SCENE`.
- Corrigida causa principal de fechamento em tela preta no login Android: `CreateLogInScene()` agora forca `SceneLogin=1` em Android quando vier modo custom (`2/3/4/0`), evitando `LoadWorld()` de login custom que encerrava o app em falha de arquivo.
- Validacao ADB em instalacao limpa (`pm clear`) confirmada apos patch: `WEBZEN_SCENE` -> `LOG_IN_SCENE`, primeiro render em login e processo vivo (`pidof`) sem `SIGSEGV`.
- Corrigido caminho da tela de loading no Android: `RenderTitleSceneUI()` passou a renderizar sem `SwapBuffers(hDC)` quando `hDC` e nulo, e o `WebzenScene` voltou a chamar o render da tela de titulo no caminho Android.
- Adicionado fallback visual no login Android para evitar tela preta sem janelas: `CreateLogInScene()` agora exibe inicialmente `LoginMainWin` e `ServerSelWin` logo apos `CreateLoginScene()`.
- Melhorada robustez do socket não bloqueante Android em `wsctlc.cpp`: `sSend/FDWriteSend` agora tratam `WSAENOTCONN/WSAEINPROGRESS/WSAEALREADY` como estado transitório (sem fechar socket).
- `CreateLogInScene()` agora também dispara `SendRequestServerList()` no sucesso de `ReconnectCreateConnection`, reduzindo janela de login sem lista quando a conexão demora.
- Nova validacao adb (2026-04-14): logs confirmam `forced initial login UI visibility`, `ReconnectCreateConnection ... requested server list`, `First successful render in LOG_IN_SCENE` e processo vivo.

### 2026-04-13

- Atualizado o `.gitignore` para bloquear caches de IDE, Gradle, CMake Android, ferramentas locais e artefatos de build.
- Registrada a limpeza de artefatos locais em `docs/gitignore-artefatos-locais-2026-04-13.md`.
- Incluidos no `.gitignore` os novos artefatos locais `Main/android/.android-user/` e `analytics.settings` para evitar poluicao na branch colaborativa.
- Validado o build Android completo com `assembleDebug`.
- Confirmada a geracao do APK `Main/android/app/build/outputs/apk/debug/app-debug.apk`.

### 2026-04-12

- Expandida a camada `AndroidWin32Compat.h` para suportar melhor caixas de texto, mensagens de janela, foco, IME e constantes Win32 usadas pela UI legada no Android.
- Adicionada documentacao tecnica da etapa em `docs/android-port-compat-ui-2026-04-12.md`.
- Registrado o estado atual e os bloqueios do port Android neste README.
- Preparado um JDK portatil local para o projeto, configurado o cache local do Gradle e levado o build Android ate a compilacao nativa do NDK.
- Corrigido conflito entre macro legado `min` e a camada de compatibilidade Android.
- Removido `CBTMessageBox.cpp` do build Android por ser um modulo puramente Win32 baseado em hook CBT.
- Adicionados novos shims Android para `OffsetRect`, `_vsntprintf` e `GetKeyState`.
