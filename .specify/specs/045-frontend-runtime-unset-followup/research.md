# Research: Frontend Runtime Unset Follow-up

## Decision 1: Treat rendered runtime state as source of truth for API-base readiness

- Decision: Use rendered behavior in Banana v2 as the closure signal for API-base resolution, with container/env checks as supporting diagnostics only.
- Rationale: The defect presents as mismatch between environment wiring and rendered state; env-only checks cannot guarantee user-visible correctness.
- Alternatives considered:
  - Continue relying primarily on `printenv` in containers.
  - Depend on compose interpolation checks only.
  - Rejected because both can pass while UI still shows unresolved state.

## Decision 2: Keep configuration error handling explicit but state-specific

- Decision: Show "Frontend runtime configuration error" only when effective API-base resolution is invalid; distinguish this from downstream API/session bootstrap failures.
- Rationale: Current user experience conflates resolution failures and runtime bootstrap issues, reducing trust in remediation guidance.
- Alternatives considered:
  - Use one generic error for all bootstrap failures.
  - Hide resolution errors and rely on silent retry.
  - Rejected because both reduce diagnosability.

## Decision 3: Preserve canonical compose startup/down as remediation baseline

- Decision: Keep compose runtime/apps tasks as canonical remediation path and verify outcomes with rendered checks after restart.
- Rationale: This aligns with established repo runtime contracts and one-window validation principles.
- Alternatives considered:
  - Add ad-hoc local-only commands for remediation.
  - Require manual source edits or env exports per incident.
  - Rejected because they increase drift and onboarding friction.

## Decision 4: Add regression checks around session/bootstrap recovery

- Decision: Include test/validation cases that assert session/chat readiness returns once API-base state is valid.
- Rationale: User-reported failure includes `session: missing` and disabled chat controls after remediation attempts.
- Alternatives considered:
  - Validate only API-base text rendering.
  - Rejected because it misses functional recovery of interactive features.
