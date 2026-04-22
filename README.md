# Banana

Pass the Banana from native C into API, desktop, web, and mobile channels.

This repo is a practical playground for one core idea: keep domain behavior in native C, then project it cleanly into multiple runtimes without rewriting business logic per app.

## Vibe Code Quickstart (5 Minutes)

1. Open this workspace in VS Code on Windows with Docker Desktop + WSL2 enabled.
2. In Run and Debug, pick `Demo Channel` from `.vscode/launch.json`.
3. Press F5 and wait for compose startup plus desktop/mobile launch hooks.
4. Open these endpoints:
   - API health: `http://localhost:8080/health`
   - React app: `http://localhost:5173`
   - React Native web: `http://localhost:19006`
5. Stop everything when done: `bash scripts/compose-apps-down.sh`.

## One-Click Runtime Channels

- Primary profile: `Demo Channel`
  - Starts API + app surfaces through Docker Compose.
  - Includes Electron desktop via WSL2/WSLg forwarding.
- Focused profiles:
  - `Build, API + Electron`
  - `Build, API + React-Native[Web]`
  - `Build, API + React-Native[Android]`
  - `Build, API + React`

## Mobile Runtime Behavior (WSL2/WSLg)

- Android startup path for the standard profile:
  - Tries Ubuntu Android SDK emulator first (native WSLg window).
  - Falls back to containerized `android-emulator` when local SDK launch is unavailable.
- noVNC fallback URL:
  - `http://localhost:6080/vnc.html?autoconnect=1&resize=remote&reconnect=1`
- iOS note:
  - iOS Simulator is macOS-only; Ubuntu channel uses iOS-style web preview fallback.

Optional helper launches:

- iOS-style preview + mobile helpers:
  - `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Ephemeral mobile session:
  - `bash -lc 'set -eo pipefail; BANANA_MOBILE_EPHEMERAL_SESSION=1 scripts/launch-container-channels-with-wsl2-mobile.sh'`

Useful environment overrides:

- `BANANA_SKIP_ANDROID_EMULATOR=1`
- `BANANA_ANDROID_OPEN_WINDOW=0`
- `BANANA_ANDROID_WSLG_WINDOW_URL=<url>`
- `BANANA_ANDROID_NOVNC_TIMEOUT_SEC=<seconds>`
- `BANANA_OPEN_RUNTIME_WINDOWS=0`
- `BANANA_ANDROID_RUNTIME_MODE=auto|local|container`
- `BANANA_IOS_PREVIEW_ENGINE=webkit`
- `BANANA_COMPOSE_APPS_SERVICES="api react-app"`

## Vibe Coding Guardrails

Keep these contracts steady while you iterate quickly:

- Architecture flow: `controller -> service -> pipeline -> native interop`.
- Native runtime path for managed integration: `BANANA_NATIVE_PATH`.
- PostgreSQL-backed native flows: `BANANA_PG_CONNECTION`.
- Frontend API base URL contract: `VITE_BANANA_API_BASE_URL`.

## Fast Developer Loop

1. Make a small change in one domain.
2. Reuse existing entry points instead of ad-hoc scripts:
   - Native build: `Build Native Library` task
   - API build: `Build Banana API` task
3. Run narrow tests first, then widen only when contract edges move.

## Learn More

- Wiki hub: [Banana Wiki](https://github.com/LahkLeKey/Banana/wiki)
- Onboarding and architecture: `docs/developer-onboarding.md`
- Build/test command index: `.wiki/Build-Run-Test-Commands.md`

> Banana is a prototype for Poly: a polymorphic, multi-platform runtime approach where core behavior starts in C and can be projected into API, desktop, frontend, and mobile delivery channels.
