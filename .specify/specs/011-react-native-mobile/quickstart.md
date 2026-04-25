# Quickstart — React Native Mobile (v2)

## Prerequisites

- Windows host with Docker Desktop + Ubuntu WSL2 (matching `010` distro order).
- Android SDK tools installed in the WSL distro (for emulator).
- macOS only for native iOS Simulator; on Linux use web-preview fallback.

## Android emulator (Windows entry)

```bash
scripts/launch-container-channels-with-wsl2-mobile.sh
```

## Android emulator (Ubuntu entry)

```bash
scripts/compose-mobile-emulators-wsl2.sh
```

## See compose service (profile flags REQUIRED)

```bash
docker compose --profile apps --profile android-emulator ps
```

Without both profile flags the `android-emulator` service will appear absent.

## iOS on Linux

iOS Simulator is macOS-only. On Linux, follow the web-preview fallback
documented in [contracts/mobile-runtime-channels.md](./contracts/mobile-runtime-channels.md).

## Local Expo dev (non-container)

```bash
cd src/typescript/react-native
npm install
npx expo start
```

## Scope guardrails

- Import only `cross`-labeled symbols from `@banana/ui`.
- Don't introduce web-only Tailwind components into RN source.
- Don't change profile-gated service names.
