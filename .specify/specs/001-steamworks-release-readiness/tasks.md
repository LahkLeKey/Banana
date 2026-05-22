# Tasks: Gameplay to Steam/UAT Readiness

## Phase 1: Gameplay Slice Stabilization (Completed)

- [x] Implement native player build/class/gear/combo runtime.
- [x] Integrate API endpoints for build/class/gear/combo actions.
- [x] Integrate React Build Lab controls and status messaging.
- [x] Add focused native/API/React test coverage for gameplay contracts.
- [x] Validate visual gameplay flow through session-room and game-engine routes.

## Phase 2: UAT Harness Preparation

- [x] Define UAT scenario matrix for Steam-playable build:
  - [x] Session join and rejoin behavior.
  - [x] Movement and realtime snapshot sync.
  - [x] Class selection and stat baseline validation.
  - [x] Allocation + gear impact validation.
  - [x] Combo trigger and bonus verification.
  - [x] Error/recovery path checks.
- [x] Add evidence capture checklist (screenshots/logs/test outputs).
- [x] Produce tester instructions for local fallback and container channels.
- [x] Add bulk readiness runner script: `scripts/steam-uat-readiness.sh`.
- [x] Add timestamped readiness report generator: `scripts/generate-steam-uat-report.sh`.
- [x] Add Steam library UAT helper script: `scripts/steam-library-uat.ps1`.

## Phase 3: Steam Build Pipeline Focus

- [x] Define Steam binary packaging contract (inputs, output locations, launch args).
- [x] Add Steam UAT acceptance checklist aligned to feature requirements.
- [x] Produce first playable Windows binary candidate at `artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe`.
- [x] Validate candidate launchability with local process smoke check.
- [x] Add SteamPipe first-build upload helper script for AppID 4767150.
- [x] Add on-demand local publish orchestrators (`scripts/steam-publish-on-demand.sh`, `scripts/steam-publish-on-demand.ps1`).
- [x] Add CI/CD workflow-dispatch publish lane (`.github/workflows/steam-publish.yml`).
- [ ] Create first Steamworks build and depot assignment for AppID 4767150.
- [ ] Publish pending Steamworks changes after first build verification.
- [ ] Validate first Steam-library playable binary with the Phase 2 scenario matrix.
- [x] Record blocking issues and stabilization deltas for release branch (`artifacts/uat/steam/blockers.md`).

## Validation Commands

```bash
# Native gameplay lane
ctest --test-dir out/v3-native -C Debug -R "player_builds|runtime_player_builds_integration" --output-on-failure

# API gameplay lane
cd src/typescript/api && bun test src/routes/game-session.test.ts

# React gameplay lane
cd src/typescript/react && bun test src/lib/api.test.ts src/components/game/OverworldHud.test.tsx
```
