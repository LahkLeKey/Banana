# Contract: Bounded Context Ownership

## Purpose

Define explicit domain ownership boundaries and allowed cross-domain interactions for the API MMO recenter feature.

## Bounded Contexts

## 1) gameplay-session-orchestration

Owns:
- Session admission and membership lifecycle.
- Authoritative in-session state transitions.
- Continuity-window rejoin semantics.
- Session completion outcomes and transition events.

Publishes:
- `session.admitted`
- `session.state-updated`
- `session.rejoin-restored`
- `session.terminated`

Consumes:
- Account authorization status from `player-identity-account`.
- Progression/inventory settlement acknowledgements from `progression-inventory`.

Cannot own:
- Account profile mutation.
- Long-term progression or inventory truth persistence policy.

## 2) player-identity-account

Owns:
- Canonical player identity and ownership linkage.
- Account status policy and profile envelope.
- Authorization boundary checks for self-scope access.

Publishes:
- `account.created`
- `account.status-changed`
- `account.profile-updated`

Consumes:
- Auth/session validation inputs from existing auth flows.

Cannot own:
- Session state progression logic.
- Inventory/progression reward calculations.

## 3) progression-inventory

Owns:
- Exactly-once progression and inventory ledger writes.
- Idempotency key settlement and conflict resolution policy.
- Durable item/progression source-of-record updates.

Publishes:
- `progression.recorded`
- `inventory.recorded`
- `ledger.conflict-resolved`

Consumes:
- Validated gameplay or web commands with actor scope from other domains.

Cannot own:
- Session orchestration state machine.
- Website insight aggregation formatting.

## 4) player-insights-web

Owns:
- Player-facing insight queries and summaries.
- No-data semantics and freshness metadata.
- Read-optimized projection shape for website consumption.

Publishes:
- `insight.view-materialized`

Consumes:
- Authoritative history from gameplay/account/ledger change records.

Cannot own:
- Source-of-record mutations for progression/inventory/session state.

## Cross-Domain Interaction Rules

1. Cross-domain communication must use explicit contracts (typed service interfaces and/or event envelopes), never shared mutable models.
2. Mutations crossing domains must carry correlation ID, actor scope, and idempotency key (when state-changing).
3. Website insight reads must not issue write-path operations.
4. Any boundary changes must update this contract and API surface contract in the same change.

## Drift Control

- Keep ownership matrix versioned with API contract revisions.
- Validate route-to-bounded-context mapping in parity governance checks.
- Reject new endpoints without declared bounded-context owner.
