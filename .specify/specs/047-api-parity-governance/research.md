# Research: API Parity Governance

## Decision 1: Govern parity at overlapping capability level, not full API surface level

- Decision: Build parity inventory only for overlapping capability areas between ASP.NET (`:8080`) and Fastify (`:8081`), while allowing intentional single-surface routes.
- Rationale: The feature scope and constitution require parity where consumers depend on overlap, without forcing total API convergence.
- Alternatives considered:
  - Enforce 1:1 parity across all routes in both APIs.
  - Track parity informally in PR descriptions only.
  - Rejected because these options either over-constrain delivery or under-enforce stability.

## Decision 2: Define parity contract dimensions as method + path + status semantics + response shape

- Decision: Treat a route pair as parity-compliant only when identity (`method`, `path`) and behavior (`status outcomes`, required response fields) align or are covered by approved exception.
- Rationale: Constitution Principle VII and FR-002 through FR-005 require both existence and contract behavior parity.
- Alternatives considered:
  - Compare only method/path presence.
  - Compare only response schema and ignore status semantics.
  - Rejected because either leaves critical drift classes undetected.

## Decision 3: Make parity gate fail-closed on unresolved drift

- Decision: Delivery gate fails when any unresolved finding exists in categories `missing_route`, `status_mismatch`, or `shape_mismatch` without active approved exception.
- Rationale: FR-007 and SC-004 require prevention, not advisory-only reporting.
- Alternatives considered:
  - Warning-only drift reporting.
  - Failing only for `missing_route` but not behavior mismatches.
  - Rejected because these approaches permit consumer-visible regressions.

## Decision 4: Model exceptions as explicit, time-bounded governance artifacts

- Decision: Require each exception to include owner, rationale, approval timestamp, expiration timestamp, and bounded scope (`method + path + mismatch_type`).
- Rationale: FR-008 and parity-governance accountability require controlled temporary divergence.
- Alternatives considered:
  - Open-ended exception notes in comments.
  - Team-level blanket waivers.
  - Rejected because they cannot be validated deterministically and can hide unrelated drift.

## Decision 5: Anchor parity workflow to existing Banana runtime and CI entry points

- Decision: Reuse existing compose profiles (`runtime`, `api-fastify`) and workflow checks rather than introducing parallel orchestration.
- Rationale: Aligns with monorepo guidance to prefer existing scripts/tasks and lowers adoption risk.
- Alternatives considered:
  - New standalone parity runner outside current validation pipeline.
  - Manual periodic parity review.
  - Rejected because they fragment enforcement and reduce reliability.

## NEEDS CLARIFICATION Resolution

All Technical Context unknowns are resolved for feature 047.
No unresolved `NEEDS CLARIFICATION` markers remain.
