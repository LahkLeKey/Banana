---
description: "Cross-slice tracking for v2 regeneration"
---

# Tasks: v2 Regeneration (Parent)

**Input**: `.specify/specs/005-v2-regeneration/{spec.md,plan.md}` and child specs `006..014`.

**Tests**: This parent ships docs only; per-slice tests live in child `tasks.md` files.

## Phase 1: Scaffold (this PR)

- [X] T001 Create `.specify/legacy-baseline/` with README and 9 domain files
- [X] T002 Create `.specify/specs/005-v2-regeneration/` parent speckit set
- [ ] T003 [P] Create `.specify/specs/006-native-core/` full speckit set
- [ ] T004 [P] Create `.specify/specs/007-aspnet-pipeline/` full speckit set
- [ ] T005 [P] Create `.specify/specs/008-typescript-api/` full speckit set
- [ ] T006 [P] Create `.specify/specs/009-react-shared-ui/` full speckit set
- [ ] T007 [P] Create `.specify/specs/010-electron-desktop/` full speckit set
- [ ] T008 [P] Create `.specify/specs/011-react-native-mobile/` full speckit set
- [ ] T009 [P] Create `.specify/specs/012-compose-runtime/` full speckit set
- [ ] T010 [P] Create `.specify/specs/013-ai-orchestration/` full speckit set
- [ ] T011 [P] Create `.specify/specs/014-test-coverage/` full speckit set
- [ ] T012 Run `python scripts/validate-ai-contracts.py` and confirm `"issues": []`

## Phase 2: Per-slice planning (open one PR per slice as picked up)

- [ ] T020 [006] Refine native-core plan + tasks before any v2 native work
- [ ] T021 [007] Refine aspnet-pipeline plan + tasks
- [ ] T022 [008] Refine typescript-api plan + tasks
- [ ] T023 [009] Refine react-shared-ui plan + tasks
- [ ] T024 [010] Refine electron-desktop plan + tasks
- [ ] T025 [011] Refine react-native-mobile plan + tasks
- [ ] T026 [012] Refine compose-runtime plan + tasks
- [ ] T027 [013] Refine ai-orchestration plan + tasks
- [ ] T028 [014] Refine test-coverage plan + tasks

## Phase 3: Per-slice regeneration (each on its own feature branch)

Implementation tasks live in each child `tasks.md`. Cutover order follows the
topological order in `spec.md`: 006 → 008 → 007 → 009 → 010, 011 → 012 → 014 → 013.

## Phase 4: Cutover & legacy removal

- [ ] T040 Per slice: swap consumers to v2 entry points
- [ ] T041 Per slice: remove legacy paths once no consumer remains
- [ ] T042 Update root README + onboarding docs to point at v2 layout
- [ ] T043 Update wiki allowlist if any new human-facing page is genuinely needed
- [ ] T044 Final AI contract guard run

## Dependencies & Execution Order

- Phase 1 tasks T003..T011 can run in parallel (different directories).
- Phase 2 and 3 tasks for each slice respect the dependency edges in `spec.md`.
- Phase 4 cutover for slice X requires all slices that consume X to have cut over first.
