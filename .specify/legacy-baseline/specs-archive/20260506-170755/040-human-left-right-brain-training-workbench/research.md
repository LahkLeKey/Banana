# Research: Human Left/Right Brain Training Workbench

## Decision 1: Human workflow shape

- Decision: Use a wizard-like standard web flow: configure run, execute, inspect results, persist/promote.
- Rationale: Familiar UX for average users and easy operational onboarding.
- Alternatives considered:
  - Single large form with all controls and logs: rejected due to cognitive overload.

## Decision 2: Lane abstraction

- Decision: Treat left/right lanes as first-class selectable lanes backed by existing trainer commands and shared persistence format.
- Rationale: Preserves domain language and aligns with current model ownership.
- Alternatives considered:
  - Hardcode separate pages per lane: rejected due to duplicated behavior and drift.

## Decision 3: Auditability

- Decision: Persist operator intent and action metadata alongside run outcomes.
- Rationale: Supports human-governed model promotion and post-incident review.
- Alternatives considered:
  - Rely only on CI logs: rejected because local/human workflows would lose parity.

## Decision 4: Failure guidance

- Decision: Convert threshold and validation failures into user-readable remediation steps.
- Rationale: Reduces manual triage and repeated failed runs.
- Alternatives considered:
  - Show raw script stderr only: rejected as too technical for general operators.
