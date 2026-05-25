# Research: Deterministic Persistent World Orchestration

## Decision 1: Use one root world seed with canonical derivation for every area

**Decision**: Derive area-level generation inputs from a single immutable root seed plus normalized area identity fields (`world_id`, `lane_id`, `chunk_x`, `chunk_y`, `partition_epoch`).

**Rationale**: This satisfies FR-001 and prevents hidden nondeterminism from ad-hoc per-area seed generation while keeping replayability and auditability intact.

**Alternatives considered**: Independent random seeds per area. Rejected because it breaks reproducibility and complicates revisit parity debugging.

## Decision 2: Freeze stable area identity and chunk partition contracts

**Decision**: Treat area identity and chunk partition as versioned contracts with immutable identity fields and explicit boundary metadata (`boundary_hash`, adjacency set, seam policy version).

**Rationale**: FR-002 and FR-004 require stable partition membership and seam-safe continuity across sessions and players.

**Alternatives considered**: Dynamic partition resizing without compatibility metadata. Rejected because partition drift causes cross-session and cross-player divergence.

## Decision 3: Make deterministic mesh vectors part of generation signatures

**Decision**: Persist a canonical deterministic signature for each generated baseline containing terrain signature + mesh-vector signature + contract version.

**Rationale**: SC-001 and SC-002 depend on objective parity checks that include mesh vectors, not just terrain topology.

**Alternatives considered**: Visual-only screenshot comparisons. Rejected because rendering differences can hide vector-level nondeterminism.

## Decision 4: Enforce server-authoritative baseline plus ordered delta log

**Decision**: Separate persistence into (1) deterministic baseline artifacts and (2) ordered interaction deltas, with canonical replay order defined by `(area_state_version, delta_sequence, idempotency_key)`.

**Rationale**: FR-005, FR-006, FR-013, and FR-014 require authoritative reconstruction using baseline + replay without client-trust ambiguity.

**Alternatives considered**: Persist only final snapshots after each change. Rejected because deterministic replay auditability and merge transparency are lost.

## Decision 5: Define deterministic merge policy for overlapping changes

**Decision**: Use a canonical merge policy based on authoritative version checkpoints plus deterministic conflict resolution precedence (`policy_version`, operation class precedence, stable actor tie-break).

**Rationale**: FR-007 and FR-008 require equivalent input delta sets to produce identical canonical output regardless of player arrival order.

**Alternatives considered**: Last-write-wins by arrival timestamp only. Rejected because network timing jitter produces nondeterministic outcomes.

## Decision 6: Version authoritative area state transitions monotonically

**Decision**: Every canonical state transition increments a monotonic `area_state_version`; stale or duplicate deltas are rejected or no-op applied using idempotency checks.

**Rationale**: FR-010 and SC-003/SC-004 require stale update protection and replay consistency.

**Alternatives considered**: Timestamp-only freshness checks. Rejected because clock skew and race conditions weaken correctness.

## Decision 7: Classify failure modes and retry deterministically with same contract inputs

**Decision**: Partition failures into recoverable (`GENERATION_TIMEOUT`, `PERSISTENCE_TIMEOUT`, `DEPENDENCY_UNAVAILABLE`) and non-recoverable (`INVALID_IDENTITY`, `CONTRACT_MISMATCH`, `CORRUPT_DELTA_LOG`). Recoverable retries must reuse identical generation/replay contract inputs and preserve idempotency keys.

**Rationale**: FR-011 and FR-012 require deterministic retries that avoid area identity duplication and divergence.

**Alternatives considered**: Best-effort retries with regenerated context. Rejected because regenerated context can produce noncanonical branches.

## Decision 8: Revisit parity must include cross-player modified-area scenarios

**Decision**: Treat revisit validation as baseline + delta replay parity across session boundaries and across players when earlier players modified the target area.

**Rationale**: Directly addresses User Story 1 and User Story 2 plus SC-002/SC-003.

**Alternatives considered**: Validate only same-player revisit. Rejected because multiplayer persistence correctness remains unproven.
