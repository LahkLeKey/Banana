# Research: Backlog Risk-Value Prioritization

## Decision 1: Use weighted value-risk scoring with deterministic tie-breaks

- Decision: Apply an explicit weighted model combining value gain, risk reduction, dependency unlock, urgency, effort penalty, and delivery-risk penalty.
- Rationale: Transparent scoring avoids ad hoc prioritization and keeps ranking explainable.
- Alternatives considered:
  - Priority-label-only sorting. Rejected because it ignores dependency unlock and execution risk.
  - Manual subjective ranking per cycle. Rejected because it is not reproducible.

## Decision 2: Enforce dependency-safe immediate queue

- Decision: Exclude blocked issues from immediate recommendations unless their blockers are selected first.
- Rationale: Prevents scheduling work that cannot start and reduces wasted context switching.
- Alternatives considered:
  - Score blocked items equally. Rejected because it produces non-actionable top picks.

## Decision 3: Keep dual outputs for human and AI consumers

- Decision: Publish a concise human summary and a verbose machine-readable audit snapshot for each prioritization run.
- Rationale: Maintainers need fast decisions while automation and audit paths need full score detail.
- Alternatives considered:
  - Human-only markdown output. Rejected because automation cannot reliably consume it.
  - JSON-only output. Rejected because humans lose quick readability.

## Decision 4: Use existing issue label taxonomy as primary signals

- Decision: Derive priority, phase, ownership, and readiness from existing labels (`priority:*`, `phase:*`, `status:ready`, `agent:*`).
- Rationale: Reuses current governance and avoids new metadata systems.
- Alternatives considered:
  - Introduce a new custom metadata store. Rejected because of migration overhead and drift risk.

## Decision 5: Anchor near-term high-value sequencing on coverage epic #305

- Decision: Treat #305 chain as the immediate high-value target and prioritize foundation blockers (#306, #307) before integration and E2E stories.
- Rationale: Foundation contracts and gates unlock downstream work and reduce large-scale rework risk.
- Alternatives considered:
  - Start directly with integration or E2E tranches. Rejected because denominator and gate drift would invalidate later progress.
