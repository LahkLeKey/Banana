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

## Distro Selection Contract

- Auto-select order for Windows-shell launch:
  1. `BANANA_WSL_DISTRO` override if set.
  2. `Ubuntu-24.04`.
  3. `Ubuntu`.

If no supported Ubuntu distro is available, fail with actionable setup guidance.

## Rendering Contract

- Desktop rendering must remain direct container-to-WSLg (Wayland/X11).
- Mobile preview windows (Android emulator and iOS-style web fallback) must render directly through WSLg.
- Keep strict fail-fast behavior when display sockets are unavailable.
- Do not change default behavior to browser/noVNC fallback for this standard workflow.

## Mobile Platform Contract

- Android emulator launch on Ubuntu is supported when Android SDK emulator + platform-tools and an AVD are installed in the target distro.
- Apple iOS Simulator is not supported on Ubuntu and must remain documented as macOS + Xcode only.
- Ubuntu mobile path should provide an iOS-style web preview fallback without claiming native iOS simulator parity.

## Docker Integration Preflight Contract

Inside Ubuntu, these checks are required before desktop launch:

1. `command -v docker` resolves to an integrated Linux CLI path for the distro runtime.
2. `docker version` returns both Client and Server details.
3. `/var/run/docker.sock` exists.

If preflight fails, return clear integration guidance and fail with exit code `42`.

## Troubleshooting Classification

- Missing Docker server/socket in Ubuntu is an environment contract failure, not an application logic regression.
- Keep diagnostics explicit about distro name and detected Docker CLI path.

## Demo-Ready Runbook

1. `wsl --set-default Ubuntu-24.04` (or `Ubuntu`)
2. In Docker Desktop, enable WSL Integration for the selected Ubuntu distro.
3. `wsl --shutdown`
4. Launch Banana from Windows shell using `scripts/launch-container-channels-with-wsl2-electron.sh`.
5. Launch Banana mobile channels from Windows shell using `scripts/launch-container-channels-with-wsl2-mobile.sh`.
