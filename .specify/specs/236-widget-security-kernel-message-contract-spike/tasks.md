# Tasks: Widget Security and Kernel Message Contract Spike (236)

**Input**: Spec at `.specify/specs/236-widget-security-kernel-message-contract-spike/spec.md`

## Phase 1: Discovery

- [ ] T001 Inventory the widget message, callback, and control risks relevant to Banana notebook behavior on the Data Science page.
- [ ] T002 Evaluate DS-page fit for trust, sanitization, rejection, and degraded-state handling.

## Phase 2: Recommendation

- [ ] T003 Capture the recommended widget safety boundary in `analysis/recommendation.md`.
- [ ] T004 Document trust, fallback, and operational implications in `analysis/audit.md`.

## Phase 3: Readiness

- [ ] T005 Produce `analysis/followup-readiness-packet.md` naming the next implementation slice and bounded scope.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/236-widget-security-kernel-message-contract-spike` and confirm `OK`.