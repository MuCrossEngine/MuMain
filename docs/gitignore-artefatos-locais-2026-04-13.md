# Atualizacao do Git Ignore para Artefatos Locais

## Resumo

Durante a continuidade do port Android, o repositorio passou a gerar novos arquivos locais que nao devem ser compartilhados com a branch de trabalho, principalmente caches de build, preferencias Android e keystore debug temporaria.

## Arquivos envolvidos

### `.gitignore`

Foram consolidadas regras para impedir o rastreamento de:

- caches de IDE e configuracoes locais;
- caches de Gradle e CMake Android;
- ferramentas locais de bootstrap;
- diretorios `build/` e `.cxx/`;
- preferencias Android locais em `.android-user/`;
- arquivo `analytics.settings`;
- artefatos temporarios de Visual Studio e builds Windows.

## Motivacao

O objetivo desta limpeza e evitar que arquivos locais de build sejam enviados para a branch atual, especialmente em um fluxo colaborativo com mais de uma pessoa trabalhando ao mesmo tempo no mesmo repositorio.

## Impacto

- reduz ruido no `git status`;
- evita commits acidentais de keystore debug, preferencias Android e caches locais;
- mantem o historico focado em codigo-fonte e documentacao.

## Observacao

Essas regras previnem novos rastreamentos, mas nao removem automaticamente arquivos que ja estejam versionados pelo Git. Se algum artefato local tiver sido adicionado ao indice anteriormente, a limpeza precisa ser feita separadamente de forma controlada.
