# Continuidade do Port Android: Verificação pós-git pull (2026-04-14)

## Resumo

Após o `git pull` no branch `mobile`, foi realizada uma auditoria de consistência entre:

- código-fonte atual,
- checklist de progresso em `ANDROID_PORT.md`,
- e o relatório anterior de compatibilidade de UI/texto.

O objetivo foi confirmar o que realmente já está implementado no port Android e atualizar o status sem marcar itens de forma prematura.

## Atualização do dia (2026-04-14)

Nesta continuidade foi concluída a integração do downloader Android com o código original do game:

- `Platform/GameDownloader.cpp` agora usa o pipeline legado `FileDownloader` + `HTTPConnecter` para transporte HTTP.
- O loop de transferência em `GameShop/FileDownloader/FileDownloader.cpp` foi ajustado para leitura incremental real a cada bloco.
- Foi adicionada validação CRC32 no downloader Android com sidecar opcional `<arquivo>.crc32` (download, parse e comparação).
- O override de servidor de assets foi ligado de ponta a ponta:
  - `MU_ASSET_SERVER` no `run_adb_debug.sh`;
  - intent extra na `MainActivity`;
  - leitura em `android_main.cpp` com `GameDownloader::SetServerURL`.
- Foi adicionado suporte a manifesto de assets:
  - download opcional de `Data/assets-manifest.txt` do servidor;
  - fallback para manifesto local já baixado;
  - fallback final para lista mínima hardcoded quando não houver manifesto.
- O parser de manifesto foi ampliado para aceitar formatos legados/comuns:
  - `path|crc`, `crc path`, `path=...`, `crc=...`;
  - tolerância a colunas extras (ex.: tamanho) sem abortar o parse.
- O download de manifesto remoto passou a usar arquivo temporário (`assets-manifest.txt.tmp`) e só promove para o arquivo final após parse válido, evitando corromper cache local.
- `IsDataReady()` agora força rechecagem do downloader quando o manifesto local está envelhecido (refresh periódico de 6 horas).
- Foi implementado fluxo de pacote compactado `.zip` com extração no Android:
  - `GameDownloader` detecta entradas de pacote por extensão `.zip` e tokens de manifesto (`archive=...`, `extract=...`);
  - extração é feita via JNI em `MainActivity.extractZipArchive(...)` (ZipInputStream);
  - proteção contra path traversal por canonical path no lado Java;
  - marcador local `.extracted` controla se a extração já foi concluída para o pacote.
- Foi adicionado template de manifesto para publicação no servidor:
  - `docs/assets-server/Data/assets-manifest.txt`;
  - inclui exemplos de formatos suportados, entradas `.zip`, `archive=1`, `extract=...` e CRC.
- Foi adicionada ferramenta para gerar manifesto de produção automaticamente:
  - `tools/generate_assets_manifest.cpp` + `tools/generate_assets_manifest.sh`;
  - varre `Data/`, calcula CRC32 e escreve `Data/assets-manifest.txt`;
  - marca arquivos `.zip` com `archive=1` e `extract=...` no manifesto.
- Builds validados:
  - `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon`
  - `./gradlew assembleDebug --no-daemon`

## Evidências verificadas no código

### Compatibilidade Win32/UI em Android

Arquivo validado: `Main/source/Platform/AndroidWin32Compat.h`

Foram confirmados no código:

- `GWL_WNDPROC` e `GWL_USERDATA`
- mensagens IME (`WM_IME_*`) e `IMN_SETOPENSTATUS`
- stubs `ImmGetCompositionWindow` / `ImmSetCompositionWindow`
- `OffsetRect`
- `_vsntprintf`
- `GetKeyState`

Conclusão: o bloco de compatibilidade de UI/IME descrito no relatório de 2026-04-12 está efetivamente presente.

### Limpeza de módulo Win32 puro no build Android

Arquivo validado: `Main/android/app/src/main/cpp/CMakeLists.txt`

- `CBTMessageBox.cpp` não está listado nas fontes Android.

Conclusão: a remoção do módulo Win32-only do pipeline Android está aplicada.

### DirectSound guard no Android

Arquivo validado: `Main/source/DSplaysound.cpp`

- implementação protegida por `#ifndef __ANDROID__`.

Conclusão: comportamento esperado para excluir DirectSound no Android está ativo.

### Include chain do backend GLES no util OpenGL

Arquivos validados:

- `Main/source/ZzzOpenglUtil.cpp`
- `Main/source/StdAfx.h`

Foi confirmado:

- `ZzzOpenglUtil.cpp` inclui `stdafx.h` como primeiro include;
- `StdAfx.h` inclui `Platform/GLFixedFunctionStubs.h`.

Conclusão: item de verificação de includes para `ZzzOpenglUtil.cpp` pode ser tratado como concluído.

## Pendências ainda abertas (confirmadas)

### Build / toolchain

- Build limpo `./gradlew assembleDebug` comprovado como verde nesta rodada.

### Render e gameplay visual

- `ZzzBMD.cpp` (modelos 3D)
- `ZzzLodTerrain.cpp` (terrain)
- `ZzzEffect*.cpp` (efeitos/partículas)

### Input / texto

- Caminho Android de `Input.cpp` e atualização por frame estão ativos.
- Bridge de teclado virtual Android (`showSoftKeyboard/hideSoftKeyboard`) está ativa via foco dos controles `edit`.
- `CGMFontLayer.cpp` ainda contém trecho GDI (`GetTextMetricsW`, `GetFontData`) como pendência do checklist.

### Assets/áudio

- pasta `Main/dependencies/include/minimp3/` não encontrada.
- fonte em `Main/android/app/src/main/assets/fonts/NotoSans-Regular.ttf` não encontrada.

### File I/O residual

- `Main/source/ZzzAI.cpp` ainda possui uso de `CreateFile`.

### Downloader (1ª execução)

Arquivos existem:

- `Main/source/Platform/GameDownloader.h`
- `Main/source/Platform/GameDownloader.cpp`

Status funcional atual do downloader:

- há estrutura funcional (`IsDataReady`, `DownloadAll`, `SetServerURL`),
- há lista mínima de arquivos obrigatórios com download real via `HTTPConnecter`,
- há suporte a manifesto remoto/local de assets com fallback,
- há parser de manifesto robusto para variações de formato e colunas extras,
- há escrita segura do manifesto remoto via `.tmp` + promoção após parse válido,
- há refresh periódico do manifesto local (6h) para forçar checagem remota,
- há validação CRC32 opcional por sidecar (`.crc32`),
- há fluxo de pacote `.zip` com extração JNI e marcador `.extracted`,
- manifesto completo de assets ainda não finalizado.

Conclusão: manter Fase 7 como em andamento.

## Atualizações aplicadas no checklist principal

Arquivo atualizado: `ANDROID_PORT.md`

Foram ajustados:

- marcação concluída de `ZzzOpenglUtil.cpp` (verificação de includes);
- marcação concluída de `DSplaysound.cpp` guardado para Android;
- inclusão explícita dos shims UI/IME e dos shims residuais (`OffsetRect`, `_vsntprintf`, `GetKeyState`) como concluídos em Win32 replacements;
- inclusão da remoção de `CBTMessageBox.cpp` no `CMakeLists.txt` Android como concluída;
- atualização da linha de "Última atualização" para `2026-04-14`.

## Próximo passo recomendado

1. Fechar `CGMFontLayer.cpp` Android path + bundle da fonte TTF.
2. Rodar `tools/generate_assets_manifest.sh` sobre a árvore real de assets e publicar o `Data/assets-manifest.txt` gerado.
3. Definir política final para CRC obrigatório em produção (hoje está opcional via sidecar quando presente).
4. Validar fluxo de 1ª execução em dispositivo real (download, extração de pacote e retomada após falha).
