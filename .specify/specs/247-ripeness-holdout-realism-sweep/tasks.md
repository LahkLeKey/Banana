# Tasks: Ripeness Holdout Realism Sweep

**Branch**: `247-ripeness-holdout-realism-sweep` | **Created**: 2026-05-05

## Phase 1 -- Setup

- [ ] T001 [FR-247-05] Repoint `.specify/feature.json` to `247-ripeness-holdout-realism-sweep` during execution.
- [ ] T002 [FR-247-04] Confirm current `splits.eval_ids` composition and capture baseline metrics snapshot.

## Phase 2 -- Eval Split Hardening

- [ ] T003 [FR-247-01] Expand `data/ripeness/corpus.json` eval IDs to >=12 with >=3 per label.
- [ ] T004 [FR-247-02] Add boundary-focused eval samples and mark with source metadata (`hard-boundary-v*`).
- [ ] T005 [FR-247-05] Verify deterministic eval ordering and reproducible holdout composition.

## Phase 3 -- Validation Guardrails

- [ ] T006 [FR-247-03] Add `scripts/check-ripeness-eval-split.py` for size/balance/boundary-ratio checks.
- [ ] T007 [FR-247-04] Wire split summary fields into ripeness metrics output.
- [ ] T008 [FR-247-03] Add negative fixture test path for validator failure behavior.

## Phase 4 -- Verification and Close-out

- [ ] T009 [FR-247-01] Retrain ripeness (`ci`, `single`) and record non-trivial confusion matrix evidence.
- [ ] T010 [FR-247-05] Run `python scripts/validate-ai-contracts.py`.
- [ ] T011 [FR-247-05] Run `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/247-ripeness-holdout-realism-sweep/spec.md`.
- [ ] T012 [FR-247-05] Update README/analysis notes and mark tasks complete.
