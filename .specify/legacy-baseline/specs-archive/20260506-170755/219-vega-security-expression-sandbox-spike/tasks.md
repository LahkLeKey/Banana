# Tasks: Vega Security Expression Sandbox Spike (219)

**Input**: Spec at `.specify/specs/219-vega-security-expression-sandbox-spike/spec.md`

## Phase 1: Discovery

- [x] T001 Inventory the Vega expression and config risks relevant to Banana notebook outputs on the Data Science page.
- [x] T002 Evaluate DS-page fit for trust, sanitization, rejection, and degraded-state handling.

## Phase 2: Recommendation

- [x] T003 Capture the recommended Vega safety boundary in `analysis/recommendation.md`.
- [x] T004 Document trust, fallback, and operational implications in `analysis/audit.md`.

## Phase 3: Readiness

- [x] T005 Produce `analysis/followup-readiness-packet.md` naming the next implementation slice and bounded scope.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/219-vega-security-expression-sandbox-spike` and confirm `OK`.