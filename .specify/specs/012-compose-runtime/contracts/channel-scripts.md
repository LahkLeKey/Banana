# Contract — Channel Scripts & Build Hygiene

## Canonical channel scripts

| Channel | Windows entry | Ubuntu/WSL2 entry |
|---------|---------------|-------------------|
| Electron desktop | `scripts/launch-container-channels-with-wsl2-electron.sh` | `scripts/compose-electron-desktop-wsl2.sh` |
| Mobile emulator  | `scripts/launch-container-channels-with-wsl2-mobile.sh`  | `scripts/compose-mobile-emulators-wsl2.sh` |
| Apps stack       | `scripts/compose-apps.sh` (up) / `scripts/compose-apps-down.sh` (down) | same |

## GH CLI orchestration

| Purpose | Script |
|---------|--------|
| SDLC orchestration | `scripts/workflow-orchestrate-sdlc.sh` |
| Triaged-item PR    | `scripts/workflow-orchestrate-triaged-item-pr.sh` |
| Wiki sync          | `scripts/workflow-sync-wiki.sh` (worktree default `artifacts/wiki-sync/...`; refuses `.wiki`) |

## Build hygiene invariants

- `.gitattributes` MUST contain `*.sh text eol=lf`.
- Dockerfiles copying repo scripts MUST run `sed -i 's/\r$//'` on copied scripts.
- `.dockerignore` MUST exclude `**/node_modules`.
- `docker/react.Dockerfile` MUST install shared UI deps before app deps (`bun install --cwd /workspace/src/typescript/shared/ui` first).
- `docker/electron.Dockerfile` MUST use `npm ci --omit=dev` and `CXXFLAGS=-fpermissive`.

## Failure semantics

- Exit code `42` reserved for integration preflight failures.
- Two-phase compose: remove target service container before `--no-recreate up` to avoid "network not found".

## Environment contracts

`BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, `VITE_BANANA_API_BASE_URL`,
`DATABASE_URL`, `BANANA_WSL_DISTRO`. See parent contract for ownership.
