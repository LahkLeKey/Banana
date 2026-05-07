# Feature Specification: Training Metrics Suspicion Guard

**Feature Branch**: `[249-training-metrics-suspicion-guard]`
**Created**: 2026-05-05
**Status**: Draft -- ready for planning
**Type**: SLICE (quality gate automation)
**Source**: Follow-up from suspicious perfect ripeness and stop-word poisoning incidents

## Goal

Automatically detect and fail suspiciously perfect training results when evaluation evidence is too weak or too easy.

## In Scope

- Add a lane-agnostic metrics suspicion checker for banana, not-banana, and ripeness artifacts.
- Define explicit heuristics for suspicious perfect metrics and low-evidence evaluations.
- Integrate checker into existing validation and CI workflows with actionable failure messages.

## Functional Requirements

- **FR-249-01**: Add `scripts/check-training-metrics-suspicion.py` that reads `artifacts/training/*/local/metrics.json`.
- **FR-249-02**: Checker MUST fail when `holdout_accuracy==1.0` and `holdout_size` is below a configured minimum (default 8 for binary, 12 for multiclass), unless explicit override is set.
- **FR-249-03**: Checker MUST flag likely triviality signals (single-example-per-class holdout, no boundary IDs present, or confusion matrix diagonal-only with tiny support).
- **FR-249-04**: Checker output MUST include per-lane remediation guidance (expand eval IDs, add boundary samples, rerun training).
- **FR-249-05**: Validation lanes MUST call checker after training tasks and before merge readiness gates.

## Out of Scope

- Changing model scoring logic.
- Replacing existing trainer scripts.
- Post-deploy inference drift monitoring.

## Success Criteria

- Suspicious synthetic fixtures fail with clear diagnostics.
- Current healthy artifacts with adequate holdout pass.
- CI/workflow invocation blocks merges on suspicious metrics.
- `python scripts/validate-ai-contracts.py` remains green after integration.

## Validation Lane

```bash
python scripts/check-training-metrics-suspicion.py --artifacts-root artifacts/training
python scripts/validate-ai-contracts.py
bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/249-training-metrics-suspicion-guard/spec.md
```

## In-scope Files

- `scripts/check-training-metrics-suspicion.py` (NEW)
- `scripts/validate-ai-contracts.py` (if wired)
- `.github/workflows/**/*.yml` (if wired in CI lane)
- `.specify/specs/249-training-metrics-suspicion-guard/**`

## Assumptions

- Training artifacts remain emitted under `artifacts/training/<lane>/local/metrics.json`.
- Lane thresholds can be configured without changing model code paths.
- CI jobs invoking the checker already have training artifacts available.
