# Ubuntu WSL2 Reproducible Runtime Contract (2026-04)

Use this as the default, repeatable local runtime contract for Banana desktop and mobile demos in contributor onboarding.

## Baseline Environment

- Windows host.
- Docker Desktop with `Use the WSL 2 based engine` enabled.
- Ubuntu distro installed from Microsoft Store (`Ubuntu-24.04` preferred, `Ubuntu` acceptable).
- Docker Desktop `Settings > Resources > WSL Integration` enabled for the target Ubuntu distro.

## Required Entry Points

- Windows-shell launcher: `scripts/launch-container-channels-with-wsl2-electron.sh`
- Ubuntu launcher: `scripts/compose-electron-desktop-wsl2.sh`
- Windows-shell launcher (mobile): `scripts/launch-container-channels-with-wsl2-mobile.sh`
- Ubuntu launcher (mobile): `scripts/compose-mobile-emulators-wsl2.sh`

Do not introduce parallel runtime launch paths unless explicitly required.

## Canonical Compose Profile Entry Point

- `scripts/compose-run-profile.sh` is the canonical profile start/stop path for supported local run profiles.
- `scripts/compose-profile-ready.sh` is the canonical readiness gate after profile startup.
- Existing entrypoint wrappers remain supported but MUST delegate to these canonical profile scripts.

## Distro Selection Contract

- Auto-select order for Windows-shell launch:
  1. `BANANA_WSL_DISTRO` override if set.
  2. `Ubuntu-24.04`.
  3. `Ubuntu`.

If no supported Ubuntu distro is available, fail with actionable setup guidance.

## Rendering Contract

- Desktop rendering must remain direct container-to-WSLg (Wayland/X11).
- When container Android runtime is used, standard profile access should attempt to open the noVNC UI (`http://localhost:6080`) in a WSLg window.
- iOS-style web preview windows (when using the mobile launcher) must render directly through WSLg.
- Keep strict fail-fast behavior when display sockets are unavailable.

## Mobile Platform Contract

- Standard Banana launcher (`scripts/launch-container-channels-with-wsl2-electron.sh`) defaults Android runtime to `auto`: prefer Ubuntu local SDK emulator with WSLg, then fallback to Docker Compose service `android-emulator` (profile `android-emulator`).
- Optional standard-launcher override remains supported with `BANANA_SKIP_ANDROID_EMULATOR=1`.
- Optional Android WSLg auto-open override remains supported with `BANANA_ANDROID_OPEN_WINDOW=0`.
- Explicit runtime override remains supported with `BANANA_ANDROID_RUNTIME_MODE=local` or `BANANA_ANDROID_RUNTIME_MODE=container`.
- Apple iOS Simulator is not supported on Ubuntu and must remain documented as macOS + Xcode only.
- Ubuntu mobile path should provide an iOS-style web preview fallback without claiming native iOS simulator parity.
- iOS preview execution in this contract remains local to Windows + Ubuntu WSL2 (no remote macOS dependency).
- Optional local iOS preview engine override: `BANANA_IOS_PREVIEW_ENGINE=webkit` when Ubuntu has a local WebKit launcher.
- Optional ephemeral lifecycle control: `BANANA_MOBILE_EPHEMERAL_SESSION=1` to tear down compose apps when the session ends.

## Docker Integration Preflight Contract

Inside Ubuntu, these checks are required before desktop launch:

1. `command -v docker` resolves to an integrated Linux CLI path for the distro runtime.
2. `docker version` returns both Client and Server details.
3. `/var/run/docker.sock` exists.

If preflight fails, return clear integration guidance and fail with exit code `42`.

## Runtime Diagnostic Messaging Contract

- Preflight failures MUST classify missing Docker server/socket as environment contract failures.
- Desktop and mobile Ubuntu launchers MUST provide actionable remediation when `docker version` fails or `/var/run/docker.sock` is unavailable.
- Readiness failures MUST identify profile/service and health-check target when available.

## Troubleshooting Classification

- Missing Docker server/socket in Ubuntu is an environment contract failure, not an application logic regression.
- Keep diagnostics explicit about distro name and detected Docker CLI path.

## Demo-Ready Runbook

1. `wsl --set-default Ubuntu-24.04` (or `Ubuntu`)
2. In Docker Desktop, enable WSL Integration for the selected Ubuntu distro.
3. `wsl --shutdown`
4. Launch Banana from Windows shell using `scripts/launch-container-channels-with-wsl2-electron.sh` (includes Android emulator container channel).
5. Launch Banana mobile channels from Windows shell using `scripts/launch-container-channels-with-wsl2-mobile.sh` when iOS-style preview and mobile helper flows are needed.
