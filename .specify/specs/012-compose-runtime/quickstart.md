# Quickstart — Compose Runtime (v2)

## Prerequisites

- Windows + Docker Desktop + Ubuntu (Microsoft Store) WSL2 distro.
- GH CLI (`gh`) for workflow orchestration scripts.

## Bring up the apps stack

```bash
bash scripts/compose-apps.sh
```

VS Code task: **Start Compose Apps**. Tear down: **Stop Compose Apps** (`bash scripts/compose-apps-down.sh`).

## Channel-specific entries

| Channel | Script |
|---------|--------|
| Electron desktop (Windows) | `scripts/launch-container-channels-with-wsl2-electron.sh` |
| Electron desktop (Ubuntu) | `scripts/compose-electron-desktop-wsl2.sh` |
| Mobile emulator (Windows) | `scripts/launch-container-channels-with-wsl2-mobile.sh` |
| Mobile emulator (Ubuntu)  | `scripts/compose-mobile-emulators-wsl2.sh` |

## Profile-gated services

```bash
docker compose --profile apps --profile android-emulator ps
```

Profile flags are required to see profile-gated services.

## Workflow orchestration

```bash
bash scripts/workflow-orchestrate-sdlc.sh
bash scripts/workflow-orchestrate-triaged-item-pr.sh
bash scripts/workflow-sync-wiki.sh
```

`workflow-sync-wiki.sh` defaults its worktree to `artifacts/wiki-sync/...`
and refuses to use the repo `.wiki` directory directly.

## Failure modes

- Exit code `42`: preflight failure (Docker server/socket/WSLg/distro).
- "network not found" on `--no-recreate up`: remove target service container first, then bring up.
- `/usr/bin/env: 'bash\r'`: a copied script slipped CRLF; ensure `.gitattributes` rule is honored and Dockerfile normalizes.
