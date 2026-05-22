# Steam Playable Binary Contract

## Purpose

Define the minimum packaging and launch contract for a Banana playable binary that can be started from the Steam library and used for UAT.

## Contract Version

- Version: 0.1
- Date: 2026-05-21
- Feature: 001-steamworks-release-readiness

## Steam Target Identity

- Steam partner app landing: `https://partner.steamgames.com/apps/landing/4767150`
- Steam AppID: `4767150`

## Inputs

- Source workspace root: `c:/Github/Banana`
- Gameplay feature branch with native/API/React gameplay slice integrated
- Steam marketing/library assets under `artifacts/steam-placeholders/`

## Required Output Shape

- Root output directory: `artifacts/steam-build/<build-id>/`
- Expected launchable executable:
  - `artifacts/steam-build/<build-id>/BananaEngineer.exe`
- Expected runtime payload directories (minimum):
  - `artifacts/steam-build/<build-id>/resources/`
  - `artifacts/steam-build/<build-id>/runtime/`

## Launch Contract

- Steam launch target should resolve to `BananaEngineer.exe`.
- App must start without requiring developer-only shell context.
- App should open directly to playable flow (`/session-room` then `/game-engine` equivalent UX).

## UAT Acceptance Contract

Steam-launched build must satisfy all checks in:

- `.specify/specs/001-steamworks-release-readiness/checklists/steam-uat-readiness.md`

Minimum validated behaviors:

- Join session and reach playable viewport.
- Execute class/build/gear/combo interactions with visible feedback.
- Capture evidence artifacts (screenshots and test/log summary).

## Current Blockers

- Electron packaging channel is currently scaffold-level (`src/typescript/electron/package.json` uses placeholder build scripts).
- Steam deployment/upload automation is not yet implemented in repository scripts.

## Next Implementation Slice

- Implement real desktop packaging scripts in `src/typescript/electron`.
- Add Steam depot packaging/upload orchestration entrypoint.
- Produce first `artifacts/steam-build/<build-id>/` playable candidate and run UAT checklist end-to-end.