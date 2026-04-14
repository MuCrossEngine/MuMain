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

- Distribuicao de assets já suporta manifesto remoto/local e pacote `.zip` com extração no Android, mas ainda falta manifesto completo de produção.
- Validacao CRC foi integrada com sidecar opcional (`.crc32`), mas falta politica final de integridade para todos os pacotes.
- Integracao completa de teclado virtual/IME Android ainda pendente.
- Validacao funcional em runtime no dispositivo Android ainda pendente.
- Ajustes adicionais de UX mobile, entrada de texto e distribuicao de assets ainda sao necessarios para considerar o port pronto para uso final.

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

### 2026-04-14

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
- Atualizado `ANDROID_PORT.md` com o status atual da Fase 7 e pendencias remanescentes.
- Atualizado o relatorio diario `docs/android-port-compat-ui-2026-04-13.md` para refletir a rodada atual de implementacao.
- Validado build Android com `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon` e `./gradlew assembleDebug --no-daemon`.

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
