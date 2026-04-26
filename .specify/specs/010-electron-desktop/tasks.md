---
description: "Tasks for v2 Electron desktop regeneration"
---

# Tasks: Electron Desktop (v2)

## Phase 0 — Bridge audit

- [ ] T001 Document each export from `nativeBridge.js` and `nativeCli.js` and consumers.
- [ ] T002 Decide: merge or formalize roles.

## Phase 1 — Process split

- [ ] T010 Ensure `preload.js` exposes only the safe API to renderer.
- [ ] T011 [P] Renderer imports from `@banana/ui` public surface only.

## Phase 2 — WSL2/WSLg contract

- [ ] T020 Verify Windows entry `scripts/launch-container-channels-with-wsl2-electron.sh` distro order.
- [ ] T021 Verify Ubuntu entry `scripts/compose-electron-desktop-wsl2.sh` preflight + exit `42`.
- [ ] T022 Smoke vs desktop labels in compose explicit; README updated.

## Phase 3 — Container build

- [ ] T030 Pin base image digests in `docker/electron.Dockerfile`.
- [ ] T031 Confirm `npm ci --omit=dev` + `CXXFLAGS=-fpermissive` retained.
- [ ] T032 Document libGL prerequisites per `/memories/repo/electron-rendering-libgl.md`.
