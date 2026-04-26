---
description: "Tasks for v2 React Native mobile regeneration"
---

# Tasks: React Native Mobile (v2)

## Phase 0 — Inventory

- [ ] T001 List `@banana/ui` imports in RN source; mark web-only ones.
- [ ] T002 Document expected emulator preflight steps (WSLg, SDK paths).

## Phase 1 — Cross-platform discipline

- [ ] T010 Add lint rule: only `cross`-labeled imports from `@banana/ui`.
- [ ] T011 Replace web-only imports with RN-friendly alternatives.

## Phase 2 — Launch contract

- [ ] T020 Audit `scripts/launch-container-channels-with-wsl2-mobile.sh` distro order matches `010`.
- [ ] T021 Audit `scripts/compose-mobile-emulators-wsl2.sh` preflight + exit semantics.
- [ ] T022 Document `--profile apps --profile android-emulator` requirement in quickstart.

## Phase 3 — Artifact hygiene

- [ ] T030 Decide which `dist/` outputs are reproducible build artifacts vs source.
- [ ] T031 Add to `.gitignore` where safe; keep checked-in only when intentionally shipped.

## Phase 4 — iOS fallback

- [ ] T040 Document web-preview fallback path on Linux in quickstart.
- [ ] T041 Add explicit error if a developer attempts iOS Simulator on Linux.
