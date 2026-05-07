# Tasks: Plotly Figure Schema Normalization Spike (201)

**Input**: Spec at `.specify/specs/201-plotly-figure-schema-normalization-spike/spec.md`

## Phase 1: Discovery

- [x] T001 Inventory Plotly figure payload shapes relevant to Banana notebook outputs on the Data Science page.
- [x] T002 Evaluate normalization needs across rendering, persistence, migration, and degraded-state behavior.

## Phase 2: Recommendation

- [x] T003 Capture the recommended Plotly figure contract in `analysis/recommendation.md`.
- [x] T004 Document DS-page rendering, compatibility, and fallback implications in `analysis/audit.md`.

## Phase 3: Readiness

- [x] T005 Produce `analysis/followup-readiness-packet.md` naming the next implementation slice and bounded scope.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/201-plotly-figure-schema-normalization-spike` and confirm `OK`.