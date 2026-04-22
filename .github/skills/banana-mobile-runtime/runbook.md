# Mobile Runtime Runbook

## Entry Points

- VS Code Run and Debug profile: `Banana Channels (Container Driven)` (single consolidated launcher profile)
- Standard desktop launcher (prefers Ubuntu local Android emulator, container fallback): `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-electron.sh'`
- Windows shell launcher: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Ubuntu launcher (inside WSL): `scripts/compose-mobile-emulators-wsl2.sh`

Common variants:

- Ephemeral session:
  - `bash -lc 'set -eo pipefail; BANANA_MOBILE_EPHEMERAL_SESSION=1 scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Local WebKit iOS preview:
  - `bash -lc 'set -eo pipefail; BANANA_IOS_PREVIEW_ENGINE=webkit scripts/launch-container-channels-with-wsl2-mobile.sh'`

## Environment Variables

- `BANANA_WSL_DISTRO`: Optional distro override for Windows launcher
- `BANANA_ANDROID_AVD`: Optional explicit AVD name
- `BANANA_ANDROID_RUNTIME_MODE`: Android runtime mode (`auto` default, `local`, or `container`)
- `BANANA_ANDROID_BOOT_TIMEOUT_SEC`: Android boot timeout (seconds)
- `BANANA_ANDROID_PREVIEW_URL`: URL opened inside Android emulator (default `http://10.0.2.2:19006`)
- `BANANA_ANDROID_CONTAINER_PREVIEW_URL`: URL opened inside containerized Android emulator (default `http://react-native-web:8081`)
- `BANANA_ANDROID_CONTAINER_SERVICE`: Compose service name for Android emulator container (default `android-emulator`)
- `BANANA_ANDROID_OPEN_WINDOW`: Auto-open Android noVNC UI in WSLg window when container runtime is used (`1` default, set `0` to disable)
- `BANANA_ANDROID_WSLG_WINDOW_URL`: Android noVNC URL opened in WSLg window (default `http://localhost:6080/vnc.html?autoconnect=1&resize=remote&reconnect=1`)
- `BANANA_ANDROID_NOVNC_TIMEOUT_SEC`: Seconds to wait for noVNC HTTP readiness before attempting auto-open (default `60`)
- `BANANA_OPEN_RUNTIME_WINDOWS`: On Windows shell launches, automatically open API/React/React Native runtime URLs in host browser windows (`1` default, set `0` to disable)
- `BANANA_IOS_PREVIEW_URL`: URL opened for iOS-style preview (default `http://localhost:19006`)
- `BANANA_IOS_PREVIEW_ENGINE`: Local iOS preview engine (`chromium` default, `webkit` optional)
- `BANANA_SKIP_ANDROID_EMULATOR=1`: Skip Android launch (applies to both standard and mobile launchers)
- `BANANA_SKIP_IOS_PREVIEW=1`: Skip iOS-style preview launch
- `BANANA_MOBILE_EPHEMERAL_SESSION=1`: Keep session ephemeral and tear down compose stack when launcher exits

## Platform Contract

- Android emulator supports both Ubuntu local SDK runtime and containerized runtime via Docker Compose service `android-emulator`.
- Standard launcher now defaults to `BANANA_ANDROID_RUNTIME_MODE=auto`: local Ubuntu SDK emulator first, then container fallback.
- Standard launcher attempts to open the Android noVNC UI in a WSLg window when container runtime is used.
- If Ubuntu has no GUI browser handler for `http`, standard launcher falls back to opening noVNC in the host Windows browser.
- Apple iOS Simulator is not supported on Ubuntu; use iOS-style web preview fallback.
- iOS preview must remain local-only in this workflow (no remote macOS dependency); use Chromium fallback or local WebKit preview.

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
- Local WebKit preview not launching:
  - Install `epiphany-browser` in Ubuntu, or install Node.js so `npx playwright` can launch WebKit locally.

## Native ML Domain Contract (2026-04)

- Keep public ML API declarations in `src/native/core/domain/banana_ml_models.h` and wrapper exports in `src/native/wrapper/banana_wrapper.h` stable unless a contract change is explicit.
- Keep model internals split under `src/native/core/domain/ml/{shared,regression,binary,transformer}`.
- Keep wrapper ML bridges split under `src/native/wrapper/domain/ml/{shared,regression,binary,transformer}`.
- When ML model source files move, update `BANANA_CORE_SOURCES` and `BANANA_WRAPPER_SOURCES` in `CMakeLists.txt` in the same change.
- Validate ML refactors with `Build Native Library` and `ctest --test-dir build/native -C Release --output-on-failure`.

## Not-Banana Training Contract (2026-04)

- Keep `data/not-banana/corpus.json` as the canonical labeled corpus for not-banana vocabulary training.
- Use `scripts/train-not-banana-model.py` to regenerate metrics and model artifacts.
- Use `.github/workflows/train-not-banana-model.yml` as the CI training and drift-check path.
- Track any vocabulary drift between training outputs and runtime classifiers explicitly, especially `src/native/core/domain/banana_not_banana.c` and `src/typescript/api/src/domains/not-banana/routes.ts`.