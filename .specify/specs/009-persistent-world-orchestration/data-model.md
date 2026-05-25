# Data Model: Deterministic Persistent World Orchestration

## Overview

This model defines deterministic area generation inputs, stable identity/partition contracts, server-authoritative baseline plus delta persistence, deterministic merge/replay, and fault/retry semantics.

## Entity: WorldSeedContract

| Field | Type | Description |
|-------|------|-------------|
| `world_id` | uuid | Canonical world identity |
| `root_seed` | string | Immutable root deterministic seed material |
| `derivation_policy_version` | string | Seed derivation algorithm contract version |
| `normalization_schema_version` | string | Canonical input normalization version |
| `created_at` | timestamp | Contract creation time |
| `updated_at` | timestamp | Last contract metadata update |

**Validation rules**:
- `root_seed` is immutable after world creation.
- Derivation must be deterministic for equivalent normalized inputs.
- Unknown derivation policy version is invalid for generation requests.

## Entity: AreaIdentity

| Field | Type | Description |
|-------|------|-------------|
| `area_id` | string | Stable unique area identifier |
| `world_id` | uuid | Owning world |
| `lane_id` | string | Logical shard/lane identity |
| `chunk_x` | integer | Chunk X coordinate |
| `chunk_y` | integer | Chunk Y coordinate |
| `partition_epoch` | integer | Partition epoch for compatibility migration |
| `identity_hash` | string | Canonical hash of identity tuple |

**Validation rules**:
- `identity_hash` must equal canonical hash of (`world_id`, `lane_id`, `chunk_x`, `chunk_y`, `partition_epoch`).
- Identity tuple is immutable once first baseline is persisted.
- Duplicate `area_id` with conflicting tuple is rejected.

## Entity: ChunkPartitionContract

| Field | Type | Description |
|-------|------|-------------|
| `area_id` | string | Area reference |
| `boundary_hash` | string | Deterministic boundary signature |
| `adjacent_area_ids` | string[] | Ordered adjacency set |
| `seam_policy_version` | string | Seam continuity contract version |
| `mesh_vector_frame` | string | Canonical vector frame ID |
| `created_at` | timestamp | Contract persisted time |

**Validation rules**:
- `adjacent_area_ids` ordering is canonical and deterministic.
- Equivalent area identity must always produce same `boundary_hash` and adjacency set.
- Seam policy changes require explicit version bump and compatibility migration path.

## Entity: GenerationBaseline

| Field | Type | Description |
|-------|------|-------------|
| `baseline_id` | uuid | Baseline artifact identity |
| `area_id` | string | Area reference |
| `seed_contract_version` | string | Seed contract version used |
| `generation_contract_version` | string | Baseline generation contract version |
| `terrain_signature` | string | Deterministic terrain signature |
| `mesh_vector_signature` | string | Deterministic mesh-vector signature |
| `baseline_payload_uri` | string | Pointer to stored baseline artifact |
| `generated_at` | timestamp | Generation completion time |

**Validation rules**:
- Equivalent normalized inputs must produce equal `terrain_signature` and `mesh_vector_signature`.
- New baseline for existing area is only allowed by explicit contract migration workflow.

## Entity: InteractionDelta

| Field | Type | Description |
|-------|------|-------------|
| `delta_id` | uuid | Delta identity |
| `area_id` | string | Area reference |
| `submitted_by_actor` | string | Player/system actor reference |
| `base_area_state_version` | bigint | Version client believed authoritative |
| `delta_sequence` | bigint | Canonical server sequence within area |
| `idempotency_key` | string | Duplicate-submission guard |
| `delta_payload` | jsonb | Operation payload |
| `submitted_at` | timestamp | Server receipt time |

**Validation rules**:
- (`area_id`, `idempotency_key`) uniqueness is enforced.
- Delta replay order is deterministic by (`area_state_version`, `delta_sequence`, `delta_id`).
- Deltas with irreconcilable payload schema are rejected as non-recoverable faults.

## Entity: MergePolicyContract

| Field | Type | Description |
|-------|------|-------------|
| `policy_version` | string | Merge policy identity |
| `operation_precedence` | jsonb | Deterministic operation-class precedence |
| `actor_tiebreak_rule` | string | Stable actor-order tiebreak policy |
| `replay_window_size` | integer | Window for conflict analysis |
| `created_at` | timestamp | Policy creation time |

**Validation rules**:
- Policy output is deterministic for equivalent ordered input sets.
- Policy changes require version bump and replay compatibility note.

## Entity: AuthoritativeAreaState

| Field | Type | Description |
|-------|------|-------------|
| `area_id` | string | Area reference |
| `baseline_id` | uuid | Active baseline reference |
| `area_state_version` | bigint | Monotonic canonical state version |
| `applied_delta_through_sequence` | bigint | Last included delta sequence |
| `canonical_state_signature` | string | Deterministic signature after replay/merge |
| `updated_at` | timestamp | Last canonical update |

**Validation rules**:
- `area_state_version` increases strictly monotonically.
- Recomputed canonical signature from baseline + deltas must match stored signature.
- Stale updates cannot overwrite newer `area_state_version`.

## Entity: OrchestrationFailureRecord

| Field | Type | Description |
|-------|------|-------------|
| `failure_id` | uuid | Failure event identity |
| `area_id` | string | Area reference |
| `operation_type` | enum | `generation`, `persistence`, `replay`, `merge` |
| `failure_code` | enum | Recoverable/non-recoverable code |
| `is_recoverable` | boolean | Retry eligibility |
| `retry_contract_fingerprint` | string | Canonical fingerprint of retry inputs |
| `attempt_count` | integer | Number of attempts |
| `recorded_at` | timestamp | Failure timestamp |

**Validation rules**:
- Recoverable retries must keep identical `retry_contract_fingerprint` unless a new canonical attempt is created.
- Non-recoverable failures require operator action and cannot auto-retry.

## Relationships

- `WorldSeedContract` 1..* `AreaIdentity`
- `AreaIdentity` 1..1 `ChunkPartitionContract`
- `AreaIdentity` 1..* `GenerationBaseline`
- `AreaIdentity` 1..* `InteractionDelta`
- `MergePolicyContract` 1..* `AuthoritativeAreaState`
- `GenerationBaseline` 1..* `AuthoritativeAreaState`
- `AuthoritativeAreaState` 1..* `OrchestrationFailureRecord`

## State Transition Notes

- Baseline lifecycle: `not-generated -> generated -> active` (or `superseded` under explicit migration).
- Area state lifecycle: `initialized -> replaying -> canonical -> replaying` as new deltas arrive.
- Failure lifecycle: `recorded -> retrying -> recovered` for recoverable paths; `recorded -> blocked` for non-recoverable faults.
