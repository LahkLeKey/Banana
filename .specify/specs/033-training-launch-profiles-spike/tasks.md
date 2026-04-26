# Tasks: Training Launch Profiles SPIKE

**Branch**: `033-training-launch-profiles-spike` | **Created**: 2026-04-26
**Status**: Draft.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `033-training-launch-profiles-spike`.

## Phase 2 -- Investigation

- [ ] T002 Author `analysis/trainer-surface-matrix.md` -- script,
  profiles, flags, outputs, env requirements per trainer.
- [ ] T003 Author `analysis/launch-contract-decision.md` --
  per-profile vs input-variable; debugpy interpreter selection.
- [ ] T004 Author `analysis/artifacts-contract.md` -- where local
  F5 trainer runs write outputs.

## Phase 3 -- Close-out

- [ ] T005 Author `analysis/followup-readiness.md` for the slice
  that lands the expanded F5 surface + new trainers.
- [ ] T006 Run `bash scripts/validate-spec-tasks-parity.sh --all`.
- [ ] T007 Run `python scripts/validate-ai-contracts.py`.
- [ ] T008 Mark all tasks `[x]` and update README to COMPLETE.

## Out of scope

- Implementing new trainers (waits on SPIKE 034 picking the model
  suite).
- Distributed / multi-GPU launch.
- Hyperparameter sweeps.
- Editing CI workflows (markers must remain literal).
