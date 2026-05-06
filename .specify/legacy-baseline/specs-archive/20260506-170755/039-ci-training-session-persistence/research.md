# Research: CI Training Session Persistence

## Decision 1: Canonical session record shape

- Decision: Use one normalized session record shape for all model lanes with lane-specific metrics nested under a metrics object.
- Rationale: Enables shared validators and replay tooling while retaining lane-specific signals.
- Alternatives considered:
  - Separate schema per lane: rejected due to duplicated validator logic and migration drift.

## Decision 2: Persistence location

- Decision: Store persisted sessions under data/<lane>/sessions/ with append-only history files and stable naming.
- Rationale: Keeps training lineage close to lane assets and supports cross-environment portability.
- Alternatives considered:
  - Store in artifacts/: rejected because artifacts are transient and not source-of-truth.

## Decision 3: Replay contract

- Decision: Persist run fingerprint, corpus identity, profile, thresholds, and selected session details.
- Rationale: Guarantees deterministic replay diagnostics across environments.
- Alternatives considered:
  - Persist only summary metrics: rejected due to weak reproducibility.

## Decision 4: Validation gate

- Decision: Validate persisted sessions in CI before native rebuild and promotion steps.
- Rationale: Fails fast when persistence contract regresses.
- Alternatives considered:
  - Validate only on promotion: rejected because breakage would surface late.
