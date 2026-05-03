# Tasks: Feedback Loop Promotion Spike (165)

**Input**: Spec at `.specify/specs/165-feedback-loop-promotion-spike/spec.md`

## Phase 1: Discovery

- [ ] T001 Inventory existing feedback signals from spec-drain, deployment, and runtime diagnostics.
- [ ] T002 Define promotion scoring dimensions (value gain, risk reduction, dependency unlock).

## Phase 2: Design

- [ ] T003 Define promotion artifact schema and storage path.
- [ ] T004 Define handoff path from promoted artifacts to new Spec Kit slices.

## Phase 3: Validation

- [ ] T005 Validate compatibility with existing drain/accounted summary artifacts.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/165-feedback-loop-promotion-spike` and confirm `OK`.
