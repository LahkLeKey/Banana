# Legacy Baseline — Compose Runtime & Local Orchestration

**Paths**: `docker-compose.yml`, `docker/*.Dockerfile`, `scripts/*.sh`, `.vscode/`

## What exists

- **Dockerfiles** (`docker/`): `api.Dockerfile`, `api-fastify.Dockerfile`, `electron.Dockerfile`, `react.Dockerfile`, `react-native.Dockerfile`, `native-builder.Dockerfile`, `tests.Dockerfile`, `workflow-local.Dockerfile`.
- **Compose**: root `docker-compose.yml` with services + profiles (apps, android-emulator, etc.).
- **Scripts** (`scripts/`): launch wrappers, compose helpers, validation, wiki sync, workflow orchestration.
- **VS Code**: `.vscode/launch.json` profiles, workspace tasks (Build Native, Build API, Start/Stop Compose Apps).

## Hard contracts

- Standard runtime: Windows + Docker Desktop + Ubuntu (Microsoft Store) WSL2 (see [.github/ubuntu-wsl2-runtime-contract.md](../../.github/ubuntu-wsl2-runtime-contract.md)).
- `*.sh text eol=lf` enforced via `.gitattributes`; Dockerfiles also normalize CRLF in copied scripts (`sed -i 's/\r$//'`) to avoid `/usr/bin/env: 'bash\r'`.
- Two-phase compose launches with stale containers can hit "network not found" — remove target service container before `--no-recreate up`.
- Profile-gated services require explicit `--profile` flags for `docker compose ps`.
- Missing Ubuntu Docker server/socket → environment contract failure with exit code `42`.
- GH CLI orchestration entry points: `scripts/workflow-orchestrate-triaged-item-pr.sh`, `scripts/workflow-orchestrate-sdlc.sh`.
- Wiki sync orchestration: `scripts/workflow-sync-wiki.sh` (default worktree now `artifacts/wiki-sync/...`, refuses `.wiki` directly).

## What works

- One-click container channels for Electron and Mobile via Windows shell entry points.
- Image rebuilds reproducible with pinned digests + lockfile installs.
- `BANANA_PG_CONNECTION` consistently gates DB-touching paths.

## Vibe drift / pain points

- Many overlapping launch scripts; the canonical entry point per channel is not always obvious from filenames.
- Two API Dockerfiles (`api` + `api-fastify`) reflect the dual-API drift in source.
- Base image highs on Debian slim remain — mitigation is digest pinning + tracked platform risk, not a real fix.
- Compose `electron-example` (smoke) vs the Electron desktop UI channel are easy to confuse.

## Cross-domain dependencies

- Builds: every other domain.
- Consumed by: `013-ai-orchestration` workflows (build/test/coverage stages).

## v1 entry points to preserve in v2

- Canonical channel scripts named `launch-container-channels-with-wsl2-*.sh` and `compose-*-wsl2.sh`
- Exit code `42` for preflight failures
- Profile names used by emulator/apps services
- `BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, `VITE_BANANA_API_BASE_URL`, `DATABASE_URL`, `BANANA_WSL_DISTRO` env contracts
