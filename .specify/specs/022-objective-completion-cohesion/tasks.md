# Tasks: Objective Completion Cohesion

**Input**: `.specify/specs/022-objective-completion-cohesion/spec.md`
**Plan**: `.specify/specs/022-objective-completion-cohesion/plan.md`

## Legend

- `[ ]` Not started
- `[x]` Complete

## Phase 0: Setup + Inventory

- [x] T001 Create runtime evidence folder and seed runtime evidence doc
- [x] T002 Create API evidence folder and seed API evidence doc
- [x] T003 Capture baseline validation command inventory in both evidence docs

## Phase 1: Canonical Objective Completion Ordering (US1)

- [x] T004 Audit `objectiveCompletionIds` canonicalization seams in continuity surfaces
- [x] T005 Enforce non-canonical ordering reject path in owning API continuity service
- [x] T006 Add unit + route + drift integration tests for ordering reject paths
- [x] T007 Record US1 evidence output

## Phase 2: Replay Determinism (US2)

- [x] T008 Audit replay lineage behavior under ordering-drift retries
- [x] T009 Expand deterministic diagnostics coverage for ordering-drift retry family
- [x] T010 Add integration test for authoritative replay lineage after ordering retries
- [x] T011 Confirm route contract coverage for deterministic ordering rejects
- [x] T012 Run focused API continuity suites for replay determinism lane
- [x] T013 Record US2 evidence output

## Phase 3: Growth Safety (US3)

- [x] T014 Introduce additive `checkpointReconciliationLaneTag` in owning schema path
- [x] T015 Update deterministic signature/replay behavior for additive attribute
- [x] T016 Add mutation safety + integration persistence coverage for additive workflow
- [x] T017 Record US3 evidence output

## Phase 4: Hardening + Closeout

- [x] T018 Run focused native coherence/transition suites and capture outputs
- [x] T019 Run focused API continuity contract suite and capture outputs
- [x] T020 Run focused API continuity integration suite and capture outputs
- [x] T021 Update `spec.md` status to Complete with implementation outcomes
- [x] T022 Update `plan.md` with final command set and completion status
- [x] T023 Update execution dashboard completed counts/status
- [x] T024 Prepare final evidence index and handoff summary
