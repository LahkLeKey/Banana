# Contract — Electron Runtime Channels

## Channels

| Channel | Purpose | Entry |
|---------|---------|-------|
| Desktop UI (Windows) | Full desktop app via WSL2/WSLg | `scripts/launch-container-channels-with-wsl2-electron.sh` |
| Desktop UI (Ubuntu) | Same, run from inside Ubuntu | `scripts/compose-electron-desktop-wsl2.sh` |
| Smoke | Headless sanity in compose | `docker compose run --rm electron-example npm run smoke` |
| Local fallback | Non-container dev | `src/typescript/electron/run-desktop.sh` |

The smoke channel is NOT the desktop UI runtime. Documentation must not equate them.

## Failure semantics

- Exit code `42` is reserved for integration preflight failures (Docker server/socket missing, WSLg unavailable, distro absent).
- Error messages MUST include actionable remediation.

## Distro selection

Order: `BANANA_WSL_DISTRO` env override → `Ubuntu-24.04` → `Ubuntu`. No silent fallback to other distros.

## Container build invariants

- `docker/electron.Dockerfile`:
  - `npm ci --omit=dev` for reproducible installs.
  - `CXXFLAGS=-fpermissive` for native module compatibility.
  - Base image pinned by digest; residual highs tracked as platform risk.
  - libGL prerequisites per repo memory.

## Process boundaries

- `main.js` owns app lifecycle and window creation.
- `preload.js` exposes a typed, narrow API to renderer.
- `renderer/` consumes `@banana/ui` public surface only.
- One canonical native bridge module (or a documented two-module split).
