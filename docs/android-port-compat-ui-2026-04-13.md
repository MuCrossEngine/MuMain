# Continuidade do Port Android: Verificação pós-git pull (atualizado em 2026-04-16)

## Resumo

Após o `git pull` no branch `mobile`, foi realizada uma auditoria de consistência entre:

- código-fonte atual,
- checklist de progresso em `ANDROID_PORT.md`,
- e o relatório anterior de compatibilidade de UI/texto.

O objetivo foi confirmar o que realmente já está implementado no port Android e atualizar o status sem marcar itens de forma prematura.

## Atualização do dia (2026-04-16)

Rodada focada no bloqueio reportado de tela preta sem login/server select:

- Corrigido HWID Android: `create_hwid_system()` deixou de usar string fixa e passou a buscar HWID determinístico via JNI (`MainActivity.getAndroidHardwareId`).
- Runtime validado com resposta real do login server:
  - `Packet head=0xF4 ...`
  - `ReceiveServerList total=1`.
- Corrigido bloqueio de grupo vazio na server list:
  - servidor retornava `idx=0` sem mapeamento em `ServerList.bmd`;
  - adicionado fallback em `ServerListManager::MakeServerGroup` para criar grupo mínimo compatível no Android.
- Resultado observado em log:
  - `serverGroups=1`,
  - `loginMain=1`,
  - `serverSel=1`.
- Ajuste visual aplicado no login leve (`SceneLogin=1`):
  - fundo/logo do login original renderizados no Android, removendo tela totalmente preta.

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
- Foi gerado manifesto completo a partir da árvore real local `Data/`:
  - arquivo gerado: `Data/assets-manifest.txt`;
  - total de entradas: 2117.
- Builds validados:
  - `./gradlew ':app:buildCMakeDebug[arm64-v8a]' --no-daemon`
  - `./gradlew assembleDebug --no-daemon`

### Atualização complementar (2026-04-14, ciclo de estabilização HTTP)

Rodada adicional focada exclusivamente em estabilidade de download no runtime Android (emulador):

- `Main/source/Platform/AndroidWin32Compat.h`
  - adicionados retries internos e instrumentação de erro para `AndroidHttpFetch`;
  - adicionada telemetria de falha de `recv` com bytes parciais e estado de header;
  - testados caminhos alternativos de transporte (bridge Java `HttpURLConnection` e fallback experimental), mantendo no fluxo final o caminho nativo para continuidade do port.
- `Main/source/GameShop/FileDownloader/HTTPConnecter.cpp`
  - status HTTP tratado como faixa `2xx` e mensagem de erro com código retornado.
- `Main/source/GameShop/FileDownloader/FileDownloader.cpp`
  - reforço no ciclo de vida de handles para evitar regressões de fechamento.
- `Main/android/app/src/main/java/com/mucrossengine/client/MainActivity.java`
  - adicionados métodos auxiliares de HTTP para diagnóstico e logs Java (`MUAssetsJava`) durante investigação.
- `Main/android/app/src/main/AndroidManifest.xml`
  - `usesCleartextTraffic=true` para cenários de servidor HTTP sem TLS.

Resultado do ciclo:

- Build Android segue verde (`assembleDebug`).
- O bloqueio funcional permanece no emulador:
  - downloads iniciais funcionam;
  - falha recorrente em `Data/Custom/NPC/Monster1000.bmd` com `Software caused connection abort` seguido de timeout nas tentativas seguintes.
- Conclusão: Fase 7 permanece **em andamento** até estabilização desse ponto de transporte.

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
- há manifesto completo de assets gerado localmente em `Data/assets-manifest.txt`.

Conclusão: manter Fase 7 como em andamento.

## Atualizações aplicadas no checklist principal

Arquivo atualizado: `ANDROID_PORT.md`

Foram ajustados:

- marcação concluída de `ZzzOpenglUtil.cpp` (verificação de includes);
- marcação concluída de `DSplaysound.cpp` guardado para Android;
- inclusão explícita dos shims UI/IME e dos shims residuais (`OffsetRect`, `_vsntprintf`, `GetKeyState`) como concluídos em Win32 replacements;
- inclusão da remoção de `CBTMessageBox.cpp` no `CMakeLists.txt` Android como concluída;
- atualização da linha de "Última atualização" para `2026-04-14`.

## Atualizacao final do dia (2026-04-14)

Diagnostico e correcao aplicados para o sintoma "tela preta e fechamento antes de login/select server":

- Causa identificada: quando `gmProtect->SceneLogin` vinha em modo custom (`2/3/4`), o caminho de login chamava `gMapManager->LoadWorld(...)`; em falha de arquivo de mundo no Android, esse caminho envia `WM_DESTROY` e encerra o app.
- Correcao aplicada: `CreateLogInScene()` em `Main/source/ZzzScene.cpp` agora forca `SceneLogin=1` no Android para modo de compatibilidade (logo scene), preservando o fluxo original de login sem carregar mundos custom instaveis.
- Validacao executada:
  - `./gradlew assembleDebug --no-daemon` com sucesso;
  - `adb install -r` + `adb shell pm clear com.mucrossengine.client`;
  - logs confirmando `WEBZEN_SCENE -> LOG_IN_SCENE`, mensagem de fallback `forcing Android compatibility mode=1`, primeiro render bem-sucedido no login e processo ativo via `pidof`.

Conclusao do dia: bootstrap e login render estao estaveis no Android sem crash fatal imediato no fluxo validado.

### Complemento da rodada (2026-04-14, foco UI preta)

Correcao adicional aplicada para o sintoma "sem loading/select server/login visivel":

- `UIMng::RenderTitleSceneUI` ajustado para Android sem `SwapBuffers(hDC)` quando `hDC` e nulo.
- `WebzenScene` voltou a chamar `RenderTitleSceneUI` no caminho Android (antes havia guarda `if (hDC)` que pulava o desenho da tela de titulo/loading).
- `CreateLogInScene` passou a forcar exibicao inicial de `LoginMainWin` e `ServerSelWin`, evitando tela preta mesmo antes da resposta completa do servidor.
- `wsctlc.cpp` atualizado para nao fechar socket em erros transitórios de connect não-bloqueante (`WSAENOTCONN`, `WSAEINPROGRESS`, `WSAEALREADY`) durante `sSend/FDWriteSend`.
- `CreateLogInScene` passou a solicitar `SendRequestServerList()` junto com o bootstrap de conexão para reduzir janela sem lista.

Validacao em adb (instalacao limpa) confirmou:

- `LOG_IN_SCENE: forced initial login UI visibility (LoginMainWin + ServerSelWin)`
- `LOG_IN_SCENE: ReconnectCreateConnection success (...), requested server list`
- `First successful render in LOG_IN_SCENE`
- processo ativo via `pidof` sem crash fatal.

## Próximo passo recomendado

1. Fechar `CGMFontLayer.cpp` Android path + bundle da fonte TTF.
2. Publicar o `Data/assets-manifest.txt` gerado no servidor definitivo de assets.
3. Definir política final para CRC obrigatório em produção (hoje está opcional via sidecar quando presente).
4. Validar fluxo de 1ª execução em dispositivo real (download, extração de pacote e retomada após falha).

## Atualizacao de continuidade (2026-04-22, foco login/server select Android)

Refinamento aplicado para reduzir workarounds temporários e estabilizar o caminho principal de login no Android:

- `Main/source/WSclient.cpp`
  - removido estado de debug em memória (`g_AndroidServerListDebugLine1/2/3`) e respectivos `snprintf` auxiliares;
  - mantido `g_AndroidServerListSelectionRequested` como gatilho único da transição para `ServerSelWin` após resposta de lista;
  - `ReceiveServerList` não depende mais de `m_CreditWin.IsShow()` para o fluxo Android de seleção;
  - mantido tratamento defensivo para resposta vazia (`TotalServer <= 0`) preservando lista anterior quando já existe cache local.

- `Main/source/ZzzScene.cpp`
  - removido fallback agressivo de reconnect em loop no `NewMoveLogInScene` (tentativas forçadas com `SocketClient.Close()` + reconexão);
  - mantido retry leve de `SendRequestServerList()` apenas quando há seleção pendente (`g_AndroidServerListSelectionRequested`) e lista ainda vazia;
  - removidos contadores auxiliares de reconnect do scene loop (`g_AndroidLoginSceneEnterTick`, `g_AndroidLoginReconnectAttempts`).

Resultado esperado desse ajuste:

- menor acoplamento com caminho legado de `CreditWin` no Android;
- menos ruído e menos estado transitório de diagnóstico no runtime;
- fluxo de seleção de servidor continua resiliente sem reconexões forçadas no loop da cena.

### Validacao de continuidade (2026-04-22, build + smoke adb)

Rodada executada após o cleanup do fluxo Android:

- Build: `Main/android` `./gradlew assembleDebug --no-daemon` concluído com sucesso.
- Deploy: `adb install -r` + `adb shell pm clear com.mucrossengine.client` concluídos com sucesso.
- Runtime: processo estável em foreground (`com.mucrossengine.client/.MainActivity`, `pid` ativo), sem assinatura de crash/ANR na rodada.
- Evidência funcional capturada em log (processo do app):
  - `CreateLogInScene: forcing Android compatibility mode ...`
  - `ReceiveServerList[0] ...`
  - `ReceiveServerList total=1`

Limitação observada nesta rodada:

- Automação por toque genérico (`adb input`/`monkey`) não foi determinística para atravessar clique de servidor e conectar até os marcos `ServerSelWin click`/`ReceiveServerConnect` em todas as execuções.

Próximo passo prático recomendado:

- Fazer uma passada de validação guiada por coordenadas reais da UI (ou instrumentação temporária mínima de marcos de clique) para fechar evidência end-to-end: LoginMain -> ServerSel click -> ReceiveServerConnect.

### Fechamento da validacao guiada (2026-04-22)

Foi executada uma validação guiada com instrumentação temporária mínima (apenas para calibração de coordenadas de toque), seguida de limpeza completa dessa instrumentação.

Evidência end-to-end capturada em log:

- `ServerSelWin click: ... connectIndex=1 percent=0`
- `ReceiveServerConnect: target=74.63.218.132:55901`
- `ReceiveServerConnect: Android compat open LoginWin after connect`

Após a captura, toda a marcação temporária (`SMOKE_TMP`) foi removida dos fontes.
