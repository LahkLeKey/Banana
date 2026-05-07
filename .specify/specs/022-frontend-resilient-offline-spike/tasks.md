# Tasks: Frontend Resilient Offline SPIKE

**Branch**: `022-frontend-resilient-offline-spike` | **Created**: 2026-04-26
**Type**: SPIKE -- investigation only, no production code changes.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` ->
  `022-frontend-resilient-offline-spike`.
- [x] T002 Author `README.md` execution tracker.

## Phase 2 -- Failure-mode inventory

- [x] T003 For React (web): document today-behavior on
  (a) offline submit, (b) API timeout, (c) channel close/reopen.
  Record in `analysis/failure-mode-matrix.md`.
- [x] T004 For Electron: document the same three failure modes.
  Append to `analysis/failure-mode-matrix.md`.
- [x] T005 For React Native: document the same three failure modes.
  Append to `analysis/failure-mode-matrix.md`.

## Phase 3 -- Decision

- [x] T006 Draft `analysis/primitive-options.md` describing options
  A (shared TS package), B (per-channel), C (service worker + RN).
  Score each on: shared-code reuse, channel-native fit, retro-fit
  cost, deferred-item exposure.
- [x] T007 Recommend exactly one option. Tie the recommendation to
  specific failure-mode-matrix rows.
- [x] T008 Define the queue + persistence contract in
  `analysis/storage-contract.md`, including the per-channel storage
  adapter shape (interface + lifecycle).

## Phase 4 -- Readiness packets

- [x] T009 Author `analysis/followup-readiness-shared.md` (slice 029):
  in-scope files, validation commands
  (`bun run --cwd src/typescript/react tsc --noEmit`,
  vitest for the shared primitive), estimated task count (<=20).
  React adoption is folded into this slice's acceptance.
- [x] T010 Author `analysis/followup-readiness-electron.md`
  (slice 030): in-scope files, validation commands
  (`docker compose --profile apps run --rm electron-example npm run smoke`,
  any new vitest target), estimated task count (<=15). MUST declare
  slice 029 as a hard prerequisite.
- [x] T011 Author `analysis/followup-readiness-react-native.md`
  (slice 031): in-scope files, validation commands
  (`bun run --cwd src/typescript/react-native tsc --noEmit`),
  estimated task count (<=15). MUST declare slice 029 as a hard
  prerequisite.
- [x] T012 Author `analysis/deferred-registry.md` -- cross-device
  sync, conflict resolution, and any other deferred items with
  explicit triggers.

## Phase 5 -- Close-out

- [x] T013 Update `README.md` to COMPLETE; mark all tasks `[x]`;
  cross-link slices 029, 030, 031 from this SPIKE's deliverables.

## Out of scope

- Writing any queue, persistence, or adapter code (slices 029-031
  own that).
- Cross-device sync (deferred with trigger).
- Conflict resolution (deferred with trigger).
- Customer journey or design system decisions (SPIKEs 020, 021).
