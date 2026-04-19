#### Pass the Banana from C into various different Applications. (APIs, Desktop, Frontend, Mobile, etc.)
------------------

## Local Runtime Launch (VS Code) + Docker Desktop [Press F5]

- Use Run and Debug profile `Demo Channel` from `.vscode/launch.json`.
- This one-click profile launches the demo-ready Banana channel through Docker Compose, including the Electron desktop window via WSL2/WSLg forwarding.
- Android startup in the standard profile now prefers an Ubuntu-installed Android SDK emulator (WSLg native window) and falls back to the Android emulator container channel when local SDK launch is unavailable.
- When container fallback is used, the launcher attempts to open the emulator noVNC UI with auto-connect (`http://localhost:6080/vnc.html?autoconnect=1&resize=remote&reconnect=1`) in a WSLg window automatically.
- If Ubuntu has no GUI browser handler configured, the launcher falls back to opening the noVNC URL in the host Windows browser.
- Additional targeted Run and Debug workflows are available:
   - `Build, API + Electron`
   - `Build, API + React-Native[Web]`
   - `Build, API + React-Native[Android]`
   - `Build, API + React`
- For optional mobile helper flows, launch from a terminal:
   - iOS-style preview + mobile helpers: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
   - Ephemeral mobile session: `bash -lc 'set -eo pipefail; BANANA_MOBILE_EPHEMERAL_SESSION=1 scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Expected local endpoints after startup:
   - API health: `http://localhost:8080/health`
   - React app: `http://localhost:5173`
   - React Native web: `http://localhost:19006`
- Requirements for desktop window channel:
   - WSL2 with WSLg enabled
   - Docker Desktop with WSL integration enabled
- Optional Android startup override for the standard profile: set `BANANA_SKIP_ANDROID_EMULATOR=1`.
- Optional Android WSLg window override for the standard profile: set `BANANA_ANDROID_OPEN_WINDOW=0`.
- Optional Android WSLg window URL override: set `BANANA_ANDROID_WSLG_WINDOW_URL=<url>` (default `http://localhost:6080/vnc.html?autoconnect=1&resize=remote&reconnect=1`).
- Optional Android noVNC readiness timeout override (seconds): set `BANANA_ANDROID_NOVNC_TIMEOUT_SEC=<seconds>`.
- Optional runtime window fan-out override for the standard profile: set `BANANA_OPEN_RUNTIME_WINDOWS=0` to disable automatic opening of API/React/React Native windows in the host browser.
- Optional targeted compose service override: set `BANANA_COMPOSE_APPS_SERVICES="api react-app"` (space-delimited services) before running `scripts/compose-apps.sh`.
- Requirements for mobile emulator window channel:
   - WSL2 with WSLg enabled
   - Ubuntu Android SDK + at least one AVD for local emulator mode
   - Docker Desktop WSL integration for container fallback mode (`android-emulator` service)
   - iOS Simulator note: Apple iOS Simulator is macOS-only; Ubuntu channel provides iOS-style web preview fallback
- Android runtime mode options:
   - `BANANA_ANDROID_RUNTIME_MODE=auto` (default): try Ubuntu local SDK emulator first, then fallback to container
   - `BANANA_ANDROID_RUNTIME_MODE=local`: require Ubuntu local SDK emulator
   - `BANANA_ANDROID_RUNTIME_MODE=container`: force containerized emulator + noVNC
- Optional local iOS preview engine: set `BANANA_IOS_PREVIEW_ENGINE=webkit` (requires local Ubuntu WebKit launcher such as Epiphany or Playwright)
- To stop containers, run `bash scripts/compose-apps-down.sh` (or the `Stop Compose Apps` task).

> See the [Wiki](https://github.com/LahkLeKey/Banana/wiki) for more details

> Prototype for "Poly", a polymorphic multi-platform runtime for native code, starting with C and expanding to other languages as needed. The goal is to enable core logic in C to be reused across different application types while maintaining performance and consistency.
