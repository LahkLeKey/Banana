# Legacy Baseline — React Native Mobile

**Paths**: `src/typescript/react-native/`

## What exists

- Expo-managed RN app: `App.tsx`, `app.json`, `babel.config.js`, `metro.config.js`.
- Output: `dist/`. Local Expo state: `.expo/`.
- npm-managed (`package-lock.json`).

## Hard contracts

- Android emulator may launch through WSLg when Android SDK tools are installed.
- Apple iOS Simulator is macOS-only; on Ubuntu WSL2 it must remain a web-preview fallback.
- Windows entry: `scripts/launch-container-channels-with-wsl2-mobile.sh`.
- Ubuntu entry: `scripts/compose-mobile-emulators-wsl2.sh`.
- `docker compose ps` for `android-emulator` requires explicit profile flags: `--profile apps --profile android-emulator`.

## What works

- Expo + Metro bring up reliably under emulator profile.
- Web-preview fallback keeps iOS visible from Linux hosts.

## Vibe drift / pain points

- Shared UI from `src/typescript/shared/ui` is web-Tailwind and not directly consumable by RN — split UI strategy is undocumented.
- Mixed managers again (npm here vs Bun in api/react).
- Emulator preflight is fragile when WSLg + Android SDK paths drift.
- `dist/` checked-in artifacts can mask source-vs-build-output drift.

## Cross-domain dependencies

- Consumes: `007-aspnet-pipeline` HTTP and/or `008-typescript-api`.
- Consumed by: `012-compose-runtime` mobile emulator services.

## v1 entry points to preserve in v2

- Android emulator launch flow + WSLg path
- iOS web-preview fallback contract
- Profile-gated compose service names
