# Contract — Mobile Runtime Channels

## Channels

| Channel | Purpose | Entry |
|---------|---------|-------|
| Android emulator (Windows) | One-click via WSL2/WSLg | `scripts/launch-container-channels-with-wsl2-mobile.sh` |
| Android emulator (Ubuntu) | From inside Ubuntu shell | `scripts/compose-mobile-emulators-wsl2.sh` |
| iOS Simulator | macOS only | Native Xcode Simulator |
| iOS web-preview (Linux) | Documented fallback | Web build of the RN app served as a preview |
| Local Expo dev | Non-container | `npx expo start` |

## Compose visibility

Emulator service is profile-gated. Required flags:

```bash
docker compose --profile apps --profile android-emulator ps
docker compose --profile apps --profile android-emulator up android-emulator
```

Without both profiles, the service is invisible to `ps` and `up`.

## iOS policy

- macOS host: native Simulator allowed.
- Linux host: web-preview fallback only. Attempting native iOS Simulator on Linux MUST fail with a clear message pointing at the fallback.

## Distro selection

Shared with `010-electron-desktop`: `BANANA_WSL_DISTRO` → `Ubuntu-24.04` → `Ubuntu`.

## Cross-platform UI

RN consumes only `cross`-labeled exports from `@banana/ui` (`009`). Web-only
Tailwind components are forbidden in RN source.
