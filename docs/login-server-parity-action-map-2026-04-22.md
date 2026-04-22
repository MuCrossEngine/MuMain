# Login/Server Select parity map (source vs sourceOld) — 2026-04-22

## Escopo
Arquivos avaliados no fluxo de login, lista de servidores e conexão inicial:

- `LoginMainWin.cpp`
- `ServerSelWin.cpp`
- `ServerSelWin.h`
- `UIMng.cpp`
- `WSclient.cpp`
- `wsclientinline.h`
- `wsctlc.cpp`
- `ZzzScene.cpp`
- `ServerListManager.cpp`
- `NewUISystem.cpp`
- `NewUIReconnect.cpp`

## Método
Classificação feita por diff `Main/source` -> `Main/sourceOld` com contagem de linhas alteradas em 3 grupos:

- **android-lines**: mudanças com marcadores Android (`__ANDROID__`, `android`, `AInput`, `ANativeWindow`, `AndroidRequest*`, etc.).
- **non-android-lines**: mudanças funcionais/estilo fora desses marcadores.
- **classificação**:
  - `mixed`: mistura de Android + funcional.
  - `functional-or-style`: sem sinal Android relevante.

## Resultado quantitativo

| Arquivo | total | android-lines | non-android-lines | classe |
|---|---:|---:|---:|---|
| `LoginMainWin.cpp` | 91 | 6 | 85 | mixed |
| `ServerSelWin.cpp` | 48 | 5 | 43 | mixed |
| `ServerSelWin.h` | 0 | 0 | 0 | same |
| `UIMng.cpp` | 17 | 3 | 14 | mixed |
| `WSclient.cpp` | 167 | 18 | 149 | mixed |
| `wsclientinline.h` | 9 | 3 | 6 | mixed |
| `wsctlc.cpp` | 85 | 2 | 83 | mixed |
| `ZzzScene.cpp` | 206 | 10 | 196 | mixed |
| `ServerListManager.cpp` | 24 | 2 | 22 | mixed |
| `NewUISystem.cpp` | 49 | 3 | 46 | mixed |
| `NewUIReconnect.cpp` | 11 | 0 | 11 | functional-or-style |

## Mapa de ação recomendado

### 1) Manter divergente (compatibilidade/estabilidade)

- `WSclient.cpp` — alta concentração de mudança funcional + compat Android no receive/connect/login path.
- `ZzzScene.cpp` — diferenças extensas no controle de cena/retry durante login.
- `LoginMainWin.cpp` — diferenças de entrada e transição de UI no Android.
- `ServerSelWin.cpp` — diferenças no handling da seleção e acionamento de conexão.
- `UIMng.cpp` — contém branch Android para `SwapBuffers` e fluxo de janela de login.
- `wsctlc.cpp` — divergências de ciclo de vida de socket/close/connect relevantes para runtime Android.
- `wsclientinline.h` — declarações/headers ligados ao comportamento atual de rede/Android.
- `ServerListManager.cpp` — mudanças funcionais de montagem/mapeamento de grupos de servidor.
- `NewUISystem.cpp` — proteções de ponteiros nulos/lazy init usadas no estado atual.
- `NewUIReconnect.cpp` — em `source` há inicialização explícita (memset) e `snprintf`; `sourceOld` usa `wsprintf` + comparação de ponteiro, menos segura.

### 2) Alinhamento cosmético concluído

- `ServerSelWin.h` — alinhado; status atual `same`.

## Conclusão objetiva

- **Não está igual ao `sourceOld`** no bloco login/server-select.
- A maior parte da divergência é **mista** (Android + funcional), não apenas `#ifdef`.
- Backport para “igualar ao legacy” deve ser **cirúrgico por hunk**, começando somente por casos cosméticos; arquivos críticos acima devem permanecer como estão até existir evidência de regressão funcional no Android.

## Próximo passo sugerido

Fase 1 foi concluída. Próximo passo é uma **fase 2 opcional**: selecionar 1-2 hunks pequenos de baixo risco (fora `WSclient.cpp`/`ZzzScene.cpp`) para avaliação manual antes de qualquer backport funcional.

### Shortlist fase 2 (baixo risco)

Saída atual de `tools/analyze_sourceold_reuse.py --manual-candidates` indica como primeiros candidatos sem hard-blockers:

1. `CSResourceManager.cpp` (5 linhas de diferença, sem flags)
2. `w_BasePet.cpp` (8 linhas de diferença, sem flags)

Observação: esses candidatos não fazem parte do núcleo login/server-select; podem ser usados para validar processo de alinhamento incremental sem impactar o fluxo crítico Android.
