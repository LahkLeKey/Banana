# Tasks: Training Launch Surface Expansion

**Branch**: `037-training-launch-surface-expansion` | **Created**: 2026-04-26
**Status**: Draft. Hard prerequisites: SPIKE 033 + specs 035 + 036.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `037-training-launch-surface-expansion`.
- [ ] T002 Create branch `feature/037-training-launch-surface-expansion`
  from spec 036 head.

## Phase 2 -- F5 entries

- [ ] T003 Update `.vscode/launch.json`: add explicit `--output`
  args to the existing not-banana entries per the SPIKE 033
  artifacts contract.
- [ ] T004 Add `Train: banana (ci profile)` and
  `Train: banana (local profile)` `debugpy` entries.
- [ ] T005 Add `Train: ripeness (ci profile)` and
  `Train: ripeness (local profile)` `debugpy` entries.
- [ ] T006 Add a compound `Train: all (ci profile)` entry that
  fans out to all three trainers.

## Phase 3 -- Tasks parity

- [ ] T007 Add `.vscode/tasks.json` entries mirroring the F5 set
  (no debugger, command-palette runnable).

## Phase 4 -- Docs + close-out

- [ ] T008 Author `analysis/local-training-runbook.md` -- step by
  step F5 -> promote -> commit loop.
- [ ] T009 Run `python scripts/validate-ai-contracts.py` and
  `bash scripts/validate-spec-tasks-parity.sh --all`.
- [ ] T010 Mark all tasks `[x]` and update README to COMPLETE.

## Out of scope

- Distributed / multi-GPU launch.
- Hyperparameter sweeps.
- Editing CI workflows.
- Test-runner launches (deferred to SPIKE 032 follow-up).
