# Tasks: Vega-Lite React Renderer Spike (212)

**Input**: Spec at `.specify/specs/212-vega-lite-react-renderer-spike/spec.md`

## Phase 1: Discovery

- [ ] T001 Inventory candidate Vega-Lite host patterns for React notebook rendering on the Banana Data Science page.
- [ ] T002 Evaluate lifecycle, resize, update, and cleanup behavior for each DS-page embedding pattern.

## Phase 2: Recommendation

- [ ] T003 Capture the recommended Vega-Lite React host pattern in `analysis/recommendation.md`.
- [ ] T004 Document maintainability, rendering, and fallback implications in `analysis/audit.md`.

## Phase 3: Readiness

- [ ] T005 Produce `analysis/followup-readiness-packet.md` naming the next implementation slice and bounded scope.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/212-vega-lite-react-renderer-spike` and confirm `OK`.