# Research: API MMO Recenter

## Decision 1: Define four bounded subdomains with explicit API ownership

**Decision**: Use these bounded subdomains as the primary API ownership model:
1. `gameplay-session-orchestration`
2. `player-identity-account`
3. `progression-inventory`
4. `player-insights-web`

**Rationale**: This satisfies FR-001 and FR-006 while preserving the existing controller -> service -> pipeline -> native interop direction from repo guidance. It also reduces drift by assigning one source of truth per behavior family.

**Alternatives considered**: Keep route-level ownership without bounded subdomain policy. Rejected because route-only ownership does not prevent cross-domain drift and makes contract governance brittle.

## Decision 2: Split API surfaces by channel and intent, while preserving one player truth model

**Decision**: Publish versioned contracts under `v1/gameplay/*` (DX12 runtime orchestration) and `v1/player/*` (website-focused account/progression/insights), with shared identity and ledger-backed persistence semantics.

**Rationale**: This keeps gameplay orchestration and web APIs separately governed while guaranteeing shared account/progression/inventory truth for FR-003, FR-005, and FR-010.

**Alternatives considered**: Use one mixed endpoint namespace for all channels. Rejected because it encourages accidental coupling and weakens bounded-context integrity.

## Decision 3: Enforce exactly-once persistence with idempotency keys + conflict versioning

**Decision**: Require mutating commands to include an idempotency key and player/account scope, and settle writes through Neon-backed ledgers using optimistic version checks.

**Rationale**: This is the smallest enforceable pattern that addresses FR-004 and SC-004 in concurrent DX12 + web mutation scenarios.

**Alternatives considered**: Database-only dedupe based on payload hashes. Rejected because hash-only dedupe does not encode command intent and is fragile under semantically equivalent payload variants.

## Decision 4: Keep Neon serverless Postgres as sole system-of-record with explicit runtime aliasing

**Decision**: Continue the repository runtime contract where `NEON_DATABASE_URL`, `DATABASE_URL`, and `BANANA_PG_CONNECTION` are synchronized at startup and treated as one authoritative persistence endpoint.

**Rationale**: This aligns with existing API runtime behavior and satisfies FR-007 without introducing dual-write risk.

**Alternatives considered**: Add a separate analytics persistence store for insights during this feature. Rejected because it violates single system-of-record intent and increases consistency drift risk.

## Decision 5: Model player insights as derived read models fed from authoritative change records

**Decision**: Build website insights from persisted domain change records and ledger/session history snapshots, with explicit freshness metadata.

**Rationale**: This provides auditable visibility for FR-009 and clean no-data semantics for new players (User Story 3 acceptance scenario 2).

**Alternatives considered**: Compute insights directly from live gameplay session state. Rejected because transient runtime state is not a durable source of record and cannot satisfy replay/audit expectations.

## Decision 6: Fly.io deployment readiness uses health, rollback, and connection resilience gates

**Decision**: Keep Fly deployment constraints explicit: startup health checks, bounded concurrency, Neon connectivity validation, and rollback-ready release procedure.

**Rationale**: This fulfills FR-008 and SC-005 with measurable deployment evidence that matches current repository deployment intent.

**Alternatives considered**: Treat deployment as out-of-scope for planning and defer gates to implementation. Rejected because Fly target readiness is an explicit feature requirement.

## Decision 7: Read/write isolation at API boundary to protect gameplay orchestration under insight load

**Decision**: Apply domain-level QoS and route policy separation so website insight reads cannot starve gameplay orchestration writes/admission paths.

**Rationale**: This directly addresses the edge case around insight traffic degrading gameplay quality and supports SC-001.

**Alternatives considered**: Shared middleware policy across all routes. Rejected because global limits can create cross-channel contention and violate bounded operational intent.
