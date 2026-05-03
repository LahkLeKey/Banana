## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: Training Launch Surface Expansion aims to improve system capability and user experience by implementing the feature described in the specification.

# Feature Specification: Training Launch Surface Expansion

**Feature Branch**: `[037-training-launch-surface-expansion]`
**Created**: 2026-04-26
**Status**: Draft -- ready to implement
**Type**: SLICE (implementation)
**Hard prerequisites**: SPIKE 033 + specs 035 + 036 (the new
trainers must exist before the F5 surface points at them).
**Source**: `.specify/specs/033-training-launch-profiles-spike/analysis/followup-readiness.md`

## Goal

Expand `.vscode/launch.json` so every trainer in the suite has F5
entries for the `ci` and `local` profiles, with the artifacts
contract from SPIKE 033 enforced via explicit `--output` paths.
Add matching `tasks.json` entries so the same invocations are
reachable from the command palette without the debugger.

## Functional requirements

- **FR-037-01**: `.vscode/launch.json` has per-profile `debugpy`
  entries for every trainer in the current suite:
  - `Train: not-banana (ci profile)` (already present; updated to
    use the artifacts contract path).
  - `Train: not-banana (local profile)` (already present; updated).
  - `Train: banana (ci profile)` (NEW).
  - `Train: banana (local profile)` (NEW).
  - `Train: ripeness (ci profile)` (NEW).
  - `Train: ripeness (local profile)` (NEW).
- **FR-037-02**: Each entry passes
  `--output ${workspaceFolder:(Monorepo) Banana}/artifacts/training/<model>/local`
  (or equivalent runner-local path) per the SPIKE 033 artifacts
  contract -- never overwriting committed seed corpora or
  promoted model images.
- **FR-037-03**: Add `.vscode/tasks.json` entries that mirror the
  F5 set so the same invocations run without the debugger.
- **FR-037-04**: Add a compound entry `Train: all (ci profile)`
  that fans out to all three trainers.
- **FR-037-05**: `python scripts/validate-ai-contracts.py` still
  passes -- CI workflow markers remain literal.
- **FR-037-06**: A short runbook
  (`analysis/local-training-runbook.md`) explains the F5 ->
  promote -> commit loop.

## Out of scope

- Distributed / multi-GPU launch.
- Hyperparameter sweeps.
- Editing CI workflows.
- Test-runner launch entries (deferred to the SPIKE 032 follow-up).

## Success criteria

- F5 -> `Train: banana (ci profile)` writes a fresh artifact to
  `artifacts/training/banana/local/` and exits 0.
- F5 -> `Train: ripeness (ci profile)` writes a fresh artifact to
  `artifacts/training/ripeness/local/` and exits 0.
- The compound entry runs all three trainers to completion.
- `python scripts/validate-ai-contracts.py` passes.
- `bash scripts/validate-spec-tasks-parity.sh --all` passes.

## Validation lane

```
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/validate-ai-contracts.py
# Manual: F5 each entry once.
```

## Downstream

- The local-training runbook unblocks contributors to commit
  deterministic model improvements without leaving the editor.
- Future orchestrator slice picks up the model manifest pointing
  at the channel pointers updated by the manage-*.py scripts.

## In-scope files

- `.vscode/launch.json` (UPDATE).
- `.vscode/tasks.json` (NEW or UPDATE if scaffolded later).
- `.specify/specs/037-training-launch-surface-expansion/**` (NEW).
- `.specify/feature.json` (repoint during execution).
