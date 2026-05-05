# Tasks: Ripeness Boundary Corpus Expansion

**Branch**: `248-ripeness-boundary-corpus-expansion` | **Created**: 2026-05-05

## Phase 1 -- Setup

- [x] T001 [FR-248-05] Repoint `.specify/feature.json` to `248-ripeness-boundary-corpus-expansion` during execution.
- [x] T002 [FR-248-05] Snapshot current per-label recall and confusion matrix from ripeness metrics.

## Phase 2 -- Corpus Expansion

- [x] T003 [FR-248-01] Add balanced samples to reach >=12 per label in `data/ripeness/corpus.json`.
- [x] T004 [FR-248-02] Add paired boundary phrases for `unripe<->ripe` transitions.
- [x] T005 [FR-248-02] Add paired boundary phrases for `ripe<->overripe` transitions.
- [x] T006 [FR-248-02] Add spoiled lookalike negatives that include soft/yellow overlap but clear spoilage signals.

## Phase 3 -- Challenge Set + Scoring

- [x] T007 [FR-248-03] Create `data/ripeness/challenge-set.json` with >=24 labeled prompts.
- [x] T008 [FR-248-04] Implement `scripts/score-ripeness-challenge-set.py`.
- [x] T009 [FR-248-05] Extend ripeness training/report output with challenge-set summary fields.

## Phase 4 -- Verification and Close-out

- [x] T010 [FR-248-01] Retrain and verify holdout + challenge-set thresholds.
- [x] T011 [FR-248-05] Run `python scripts/validate-ai-contracts.py`.
- [x] T012 [FR-248-05] Run `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/248-ripeness-boundary-corpus-expansion/spec.md`.
- [x] T013 [FR-248-05] Mark tasks complete and capture before/after evidence in spec analysis notes.
