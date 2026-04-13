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

- Downloader de assets ainda sem transporte HTTP final.
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

Para reproduzir o build local usado nesta etapa:

- `JAVA_HOME`: `D:\Projetos\MuCrossEngine\MuMain\.tools\jdk17\jdk-17.0.18+8`
- `GRADLE_USER_HOME`: `D:\Projetos\MuCrossEngine\MuMain\Main\android\.gradle-user`
- `ANDROID_USER_HOME`: `D:\Projetos\MuCrossEngine\MuMain\Main\android\.android-user`
- `ANDROID_SDK_ROOT`: instalacao local do Android SDK
- Comando: `gradlew.bat assembleDebug --stacktrace`

## Changelog

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
