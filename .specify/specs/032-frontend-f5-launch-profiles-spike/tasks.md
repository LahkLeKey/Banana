# Tasks: Frontend F5 Launch Profiles SPIKE

**Branch**: `032-frontend-f5-launch-profiles-spike` | **Created**: 2026-04-26
**Status**: Draft.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `032-frontend-f5-launch-profiles-spike`.
- [ ] T002 Confirm clean tree on `feature/032-launch-profiles-and-model-suite-spikes`.

## Phase 2 -- Investigation

- [ ] T003 Author `analysis/launch-surface-matrix.md` -- per-channel
  cwd, command, env vars, prerequisites.
- [ ] T004 Author `analysis/f5-contract-decision.md` -- compound vs
  single, env-var seam, container-contract boundary.

## Phase 3 -- Initial F5 contract

- [ ] T005 Add `.vscode/launch.json` with four single-channel
  configurations (`API: Bun dev`, `React: Vite dev`,
  `Electron: smoke`, `React Native: Expo web`) and three compound
  entries (`API + React`, `API + Electron`, `API + React Native`).
- [ ] T006 Manual smoke -- F5 `API + React`, hit Vite URL, confirm
  classifier round-trip works.

## Phase 4 -- Close-out

- [ ] T007 Author `analysis/followup-readiness.md` for slice 035
  (debugger attach + test runner launches).
- [ ] T008 Run `bash scripts/validate-spec-tasks-parity.sh --all`.
- [ ] T009 Mark all tasks `[x]` and update README to COMPLETE.

## Out of scope

- Debugger attach to renderer / Chromium.
- Container-side launch (owned by WSL2 launch scripts).
- Mobile emulator launch (owned by 033 + mobile runtime contract).
