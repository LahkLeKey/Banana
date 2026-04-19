# Mobile Runtime Runbook

## Entry Points

- Windows shell launcher: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Ubuntu launcher (inside WSL): `scripts/compose-mobile-emulators-wsl2.sh`

## Environment Variables

- `BANANA_WSL_DISTRO`: Optional distro override for Windows launcher
- `BANANA_ANDROID_AVD`: Optional explicit AVD name
- `BANANA_ANDROID_BOOT_TIMEOUT_SEC`: Android boot timeout (seconds)
- `BANANA_ANDROID_PREVIEW_URL`: URL opened inside Android emulator (default `http://10.0.2.2:19006`)
- `BANANA_IOS_PREVIEW_URL`: URL opened for iOS-style preview (default `http://localhost:19006`)
- `BANANA_SKIP_ANDROID_EMULATOR=1`: Skip Android launch
- `BANANA_SKIP_IOS_PREVIEW=1`: Skip iOS-style preview launch

## Platform Contract

- Android emulator is supported on Ubuntu WSL2/WSLg when Android SDK tools and an AVD are installed.
- Apple iOS Simulator is not supported on Ubuntu; use iOS-style web preview fallback.

## Troubleshooting

- `WSLg mount /mnt/wslg was not found`:
  - Start Ubuntu with WSLg enabled and retry.
- `Android SDK tools were not found`:
  - Install Android SDK emulator + platform-tools in Ubuntu.
- `No Android AVD was found`:
  - Create an AVD with Android Studio Device Manager or `avdmanager`.
- `Android emulator did not finish booting`:
  - Inspect `/tmp/banana-android-emulator.log` and increase `BANANA_ANDROID_BOOT_TIMEOUT_SEC`.
- React Native endpoint not reachable:
  - Ensure `scripts/compose-apps.sh` started and `http://localhost:19006` responds.