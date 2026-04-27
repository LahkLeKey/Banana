# WSL2 Mobile Runtime Channels

> Read the [Wiki Home](Home.md) for more details.

Related pages: [Build, Run, and Test Commands](Build-Run-Test-Commands.md), [CI and Compose Notes](CI-Compose-Notes.md)

This page documents how Banana launches mobile runtime channels on Windows + Ubuntu WSL2.

## Launch Entry Points

Windows-shell launcher:

```bash
bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'
```

Ubuntu launcher (called by the Windows launcher):

```bash
scripts/compose-mobile-emulators-wsl2.sh

Canonical mobile profile commands (inside Ubuntu):

```bash
bash scripts/compose-run-profile.sh --profile mobile --action up --service android-emulator
bash scripts/compose-profile-ready.sh --profile mobile --service android-emulator
```
```

## What The Launcher Does

1. Stops and restarts the compose apps stack via `scripts/compose-apps-down.sh` and `scripts/compose-apps.sh`.
2. Resolves Ubuntu distro in this order:
   - `BANANA_WSL_DISTRO` override
   - `Ubuntu-24.04`
   - `Ubuntu`
3. Runs the Ubuntu-side launcher in WSL.
4. Waits for `http://localhost:19006`.
5. Launches Android emulator if enabled.
6. Launches iOS-style preview window if enabled.

## Platform Contract

- Android emulator is supported on Ubuntu WSL2/WSLg when Android SDK tools and at least one AVD are installed.
- Apple iOS Simulator is macOS + Xcode only. Ubuntu path uses iOS-style web preview fallback.

## Required Prerequisites

- Windows host with Docker Desktop using WSL2 engine.
- Ubuntu distro installed from Microsoft Store.
- Docker Desktop WSL integration enabled for that Ubuntu distro.
- WSLg available in Ubuntu (`/mnt/wslg` exists).
- For Android launch: emulator + adb installed under Ubuntu SDK path.

## Useful Environment Overrides

- `BANANA_WSL_DISTRO`: select distro explicitly.
- `BANANA_ANDROID_AVD`: choose a specific AVD name.
- `BANANA_ANDROID_BOOT_TIMEOUT_SEC`: increase Android boot wait.
- `BANANA_ANDROID_PREVIEW_URL`: URL opened inside Android emulator.
- `BANANA_IOS_PREVIEW_URL`: URL used for iOS-style preview window.
- `BANANA_SKIP_ANDROID_EMULATOR=1`: skip Android launch.
- `BANANA_SKIP_IOS_PREVIEW=1`: skip iOS-style preview launch.

## Fast Troubleshooting

- `WSLg mount /mnt/wslg was not found`:
  - Start Ubuntu with WSLg enabled and retry.
- `Android SDK tools were not found`:
  - Install Android emulator and platform-tools in Ubuntu.
- `No Android AVD was found`:
  - Create an AVD in Android Studio Device Manager or with `avdmanager`.
- React Native endpoint not reachable:
  - Check compose app startup and verify `http://localhost:19006`.