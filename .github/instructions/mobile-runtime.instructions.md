---
name: Mobile Runtime Standards
description: Guidance for Banana Android emulator and iOS preview runtime orchestration on Ubuntu WSL2/WSLg.
applyTo: "{scripts/compose-mobile-emulators-wsl2.sh,scripts/launch-container-channels-with-wsl2-mobile.sh,.vscode/launch.json,.github/ubuntu-wsl2-runtime-contract.md}"
---

# Mobile Runtime Standards

> Mobile client runtime is deprecated in favor of DX12-native and web management surfaces.
> Keep this guidance only for historical emulator orchestration scripts until fully retired.

- Use `mobile-runtime-agent` for Android emulator and iOS preview launch behavior under Ubuntu WSL2.
- Keep `scripts/launch-container-channels-with-wsl2-mobile.sh` as the Windows-shell entry point and `scripts/compose-mobile-emulators-wsl2.sh` as the Ubuntu entry point.
- Reuse existing compose stack bring-up via `scripts/compose-apps.sh` and `scripts/compose-apps-down.sh` before launching emulators.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO`, then `Ubuntu-24.04`, then `Ubuntu`.
- Keep WSLg preflight checks strict and actionable (`/mnt/wslg`, `DISPLAY`, and related runtime env assumptions).
- Keep Android launch deterministic: explicit AVD selection, timeout handling, and clear failure messages when SDK tools are missing.
- Keep iOS behavior explicit: Ubuntu cannot run Apple iOS Simulator; use iOS-style web preview fallback without claiming native simulator parity.
- If runtime entry points change, update `.github/ubuntu-wsl2-runtime-contract.md`, `.github/skills/banana-mobile-runtime`, and nearest prompts.

## Shared Frontend Contract

- If a task touches src/typescript/react or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Runtime Contract Lessons (2026-04)

- Keep mobile emulator channels explicit in Ubuntu WSL2: Android emulator may launch through WSLg when SDK tools are installed; Apple iOS Simulator remains macOS-only and must stay a web-preview fallback on Ubuntu.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.

