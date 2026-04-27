# Research: Chat Bootstrap Fetch Failure Follow-up

## Decision 1: Classify bootstrap failures by transport vs configuration source

- Decision: Treat `TypeError`/network-level fetch bootstrap failures as `bootstrap_transport` and keep configuration resolution failures as `config_resolution`.
- Rationale: The reported `Failed to fetch` symptom indicates endpoint reachability/transport failure, not necessarily missing or invalid API-base configuration.
- Alternatives considered:
  - Keep a single generic bootstrap error.
  - Map all bootstrap failures to configuration errors.
  - Rejected because both approaches blur remediation and prolong incident triage.

## Decision 2: Keep canonical runtime/apps startup order as remediation contract

- Decision: Recovery guidance references canonical compose flow: runtime up+ready first, then apps up+ready, followed by page reload/retry.
- Rationale: This is Banana's established local runtime contract and directly addresses dependency readiness timing behind fetch failures.
- Alternatives considered:
  - Suggest ad-hoc direct service restarts per incident.
  - Add source-level manual configuration edits as first-line remediation.
  - Rejected because both increase drift and reduce deterministic troubleshooting.

## Decision 3: Model bootstrap lifecycle with explicit recovery transition

- Decision: Define bootstrap state transitions that allow `failed_transport -> bootstrapping -> ready` once runtime availability returns.
- Rationale: Requirement FR-004 needs recovery without manual code edits and avoids stale failed state after dependency restoration.
- Alternatives considered:
  - Keep terminal failed state until hard refresh only.
  - Auto-hide error without explicit state transition tracking.
  - Rejected because either can hide failures or block deterministic recovery validation.

## Decision 4: Validate in one-window integrated browser flow plus readiness checks

- Decision: Closure evidence combines compose readiness checks and rendered-state verification in VS Code integrated browser.
- Rationale: Constitution Principle VI requires one-window interactive validation; transport failures must be proven fixed where users experience them.
- Alternatives considered:
  - CLI-only validation from container logs.
  - Unit-test-only validation without runtime/browser checks.
  - Rejected because neither alone proves rendered bootstrap behavior.

## NEEDS CLARIFICATION Resolution

All Technical Context placeholders and clarifications are resolved for planning.
No unresolved `NEEDS CLARIFICATION` items remain.
