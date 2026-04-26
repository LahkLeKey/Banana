# Tasks: Validation Lane Enforcement SPIKE

**Branch**: `018-validation-lane-enforcement-spike` | **Created**: 2026-04-26
**Type**: SPIKE -- investigation only, no production code changes.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` ->
  `018-validation-lane-enforcement-spike`.

## Phase 2 -- Audit

- [ ] T002 For each slice in 010..017, extract the `spec.md` Validation
  lane / Success criteria block and the `tasks.md` Validation phase
  tasks. Record in `analysis/drift-audit.md` as a slice x
  missing-validator matrix.
- [ ] T003 Classify each gap by failure mode (template omission /
  author shorthand / copy-paste decay / no parity check). Record in
  `analysis/failure-modes.md`.

## Phase 3 -- Decision

- [ ] T004 Draft `analysis/enforcement-options.md` describing options
  A (lint script), B (mandatory close-out task), C (finalize hook).
  Score each on: enforcement strength, author friction, CI cost,
  retro-fit cost.
- [ ] T005 Recommend exactly one option. Tie the recommendation to
  specific drift-audit rows.

## Phase 4 -- Readiness packet

- [ ] T006 Author `analysis/followup-readiness-packet.md` for slice
  019: in-scope files, validation commands, estimated task count
  (<=15), and the slice 017 coverage gate backfill as the first
  acceptance test.
- [ ] T007 Author `analysis/deferred-registry.md` -- list any items
  pushed past slice 019 with explicit triggers.

## Phase 5 -- Close-out

- [ ] T008 Update `README.md` to COMPLETE; mark all tasks `[x]`;
  cross-link slice 019 from this SPIKE's deliverables.

## Out of scope

- Writing the lint / hook / template (slice 019 owns that).
- Changing the slice 017 close-out (backfilled in slice 019 PR-1).
- Touching unrelated `.specify/` workflows.
