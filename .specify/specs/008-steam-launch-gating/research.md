# Research: Steam Launch Gating

## Decision 1: Enforce hard production launch gating at native launch boundary

**Decision**: The native engine launch path is blocked unless Steam identity assertion is valid and the linked production account verification status is `verified-good-standing`.

**Rationale**: This directly satisfies FR-001, FR-002, FR-007, and SC-001/SC-004 by preventing gameplay admission before account trust is established.

**Alternatives considered**: Delay verification until after runtime boot and gate only selected gameplay systems. Rejected because partial runtime access creates bypass risk and weakens production safety.

## Decision 2: Define explicit environment modes with one-way strictness

**Decision**: Use `development`, `staging-uat`, and `production-steam-package` modes with strict precedence rules. Policy strictness can only increase along the path `development -> staging-uat -> production-steam-package`.

**Rationale**: Teams keep practical local workflows while production cannot be weakened by override flags or fallback behavior.

**Alternatives considered**: Single mode with optional bypass flag. Rejected because it is easy to misconfigure and can accidentally ship weakened gating.

## Decision 3: Implement safe fallback and override policy with deny-by-default for production

**Decision**: In `production-steam-package`, missing Steam/API/session evidence always resolves to deny. In non-production modes, fallback behavior may allow limited startup only when explicitly enabled and clearly audited.

**Rationale**: Preserves development ergonomics while enforcing a non-bypassable production contract.

**Alternatives considered**: Permit cached-success fallback in production outage windows. Rejected because stale evidence can allow suspended/unlinked users through.

## Decision 4: Publish dedicated API verification and status contracts

**Decision**: Provide launch-gating endpoints for verification decisioning and status retrieval with stable reason codes and remediation metadata.

**Rationale**: Native runtime, API, and web flows need consistent machine-readable outcomes for UX and telemetry.

**Alternatives considered**: Reuse generic auth endpoints and infer reasons client-side. Rejected due to inconsistent semantics and weak observability.

## Decision 5: Add explicit web remediation contract for registration/linking

**Decision**: When launch is denied for remediable reasons, API returns an actionable remediation payload including web path, required actions, and retry preconditions.

**Rationale**: Reduces support burden and aligns to User Story 2 and FR-004/FR-005.

**Alternatives considered**: Static help URL only. Rejected because it cannot represent per-reason requirements and completion state.

## Decision 6: Standardize blocked-launch UX state model and reason taxonomy

**Decision**: Define canonical blocked states for `UNLINKED_ACCOUNT`, `SUSPENDED_OR_RESTRICTED`, `STALE_SESSION`, `STEAM_UNAVAILABLE`, `API_UNAVAILABLE`, and `OFFLINE_UNVERIFIABLE` with required user actions.

**Rationale**: Supports FR-003, FR-011, and SC-002 through clear reason/action coupling.

**Alternatives considered**: Single generic error message. Rejected because users cannot self-remediate effectively.

## Decision 7: Treat session/verification freshness as first-class policy input

**Decision**: Verification evidence includes TTL and issuance metadata; stale sessions always deny in production and trigger re-auth/re-link flows.

**Rationale**: Handles stale-token and recently-suspended edge cases while keeping decisions auditable.

**Alternatives considered**: Long-lived session acceptance without freshness checks. Rejected because it can authorize outdated account states.

## Decision 8: Failure-mode behavior favors deterministic deny with guided retry

**Decision**: For Steam/API outages, return deterministic deny outcomes with retry/backoff guidance and reason-specific messaging; successful retry must obtain fresh verification evidence.

**Rationale**: Maintains security posture and predictable UX during incidents.

**Alternatives considered**: Auto-retry loops hidden from user until timeout. Rejected because it obscures state and increases perceived launch instability.
