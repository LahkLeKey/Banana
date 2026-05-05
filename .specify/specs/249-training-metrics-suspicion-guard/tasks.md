# Tasks: Training Metrics Suspicion Guard

**Branch**: `249-training-metrics-suspicion-guard` | **Created**: 2026-05-05

## Phase 1 -- Setup

- [ ] T001 [FR-249-05] Repoint `.specify/feature.json` to `249-training-metrics-suspicion-guard` during execution.
- [ ] T002 [FR-249-01] Collect baseline artifacts from `banana`, `not-banana`, and `ripeness` lanes.

## Phase 2 -- Checker Implementation

- [ ] T003 [FR-249-01] Add `scripts/check-training-metrics-suspicion.py` with lane-aware thresholds.
- [ ] T004 [FR-249-02] Implement tiny-holdout + perfect-score suspicion rules.
- [ ] T005 [FR-249-03] Implement confusion-matrix triviality and missing-boundary heuristics.
- [ ] T006 [FR-249-02] Add override flags/environment contract for intentional exceptions.

## Phase 3 -- Test and Wire-in

- [ ] T007 [FR-249-02] Add fixture tests for pass/fail cases under `tests/`.
- [ ] T008 [FR-249-05] Wire checker into validation scripts/workflow jobs after training lanes run.
- [ ] T009 [FR-249-04] Ensure failure output includes remediation steps and offending lane/file.

## Phase 4 -- Verification and Close-out

- [ ] T010 [FR-249-02] Run checker against current artifacts and confirm expected pass/fail behavior.
- [ ] T011 [FR-249-05] Run `python scripts/validate-ai-contracts.py`.
- [ ] T012 [FR-249-05] Run `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/249-training-metrics-suspicion-guard/spec.md`.
- [ ] T013 [FR-249-05] Mark tasks complete and document rollout notes.
