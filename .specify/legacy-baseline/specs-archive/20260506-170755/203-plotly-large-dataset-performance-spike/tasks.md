# Tasks: Plotly Large Dataset Performance Spike (203)

**Input**: Spec at `.specify/specs/203-plotly-large-dataset-performance-spike/spec.md`

## Phase 1: Discovery

- [x] T001 Inventory large-dataset Plotly scenarios that matter for Banana notebook outputs on the Data Science page.
- [x] T002 Evaluate DS-page fit for full-fidelity, sampled, aggregated, and refused rendering paths.

## Phase 2: Recommendation

- [x] T003 Capture the recommended Plotly performance boundary in `analysis/recommendation.md`.
- [x] T004 Document user-experience, fallback, and dataset-governance implications in `analysis/audit.md`.

## Phase 3: Readiness

- [x] T005 Produce `analysis/followup-readiness-packet.md` naming the next implementation slice and bounded scope.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/203-plotly-large-dataset-performance-spike` and confirm `OK`.