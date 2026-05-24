# Data Model: API MMO Recenter

## Overview

This feature centers on authoritative orchestration and shared persistent truth across DX12 gameplay and website channels. The entities below represent domain records and derived views required to satisfy the feature scope.

## Entity: GameplaySession

| Field | Type | Description |
|-------|------|-------------|
| `session_id` | uuid | Stable session identity |
| `session_state` | enum | `pending`, `active`, `recovery-window`, `completed`, `aborted` |
| `admission_policy_version` | string | Ruleset version used for session admission |
| `authoritative_tick` | bigint | Last committed authoritative update index |
| `continuity_window_expires_at` | timestamp | Rejoin window boundary |
| `completion_outcome` | jsonb | Summary of termination outcomes |
| `created_at` | timestamp | Creation time |
| `updated_at` | timestamp | Last mutation time |

**Validation rules**:
- Transition ordering must be monotonic (`pending -> active -> completed|aborted`, with optional `recovery-window` before terminal state).
- Mutating session updates require authenticated actor scope and idempotency key.
- Recovery rejoin only allowed before `continuity_window_expires_at`.

## Entity: PlayerAccount

| Field | Type | Description |
|-------|------|-------------|
| `player_id` | uuid | Internal canonical player identifier |
| `external_identity` | jsonb | Provider identity linkage (for example Steam subject) |
| `account_status` | enum | `active`, `suspended`, `restricted`, `deleted` |
| `profile` | jsonb | Player-visible profile metadata |
| `policy_flags` | jsonb | Security/compliance flags |
| `version` | bigint | Optimistic concurrency version |
| `created_at` | timestamp | Creation time |
| `updated_at` | timestamp | Last mutation time |

**Validation rules**:
- One canonical `player_id` per external identity.
- Authorization checks must enforce self-access boundaries.
- Updates require version match or deterministic conflict outcome.

## Entity: ProgressionLedger

| Field | Type | Description |
|-------|------|-------------|
| `ledger_entry_id` | uuid | Event identity |
| `player_id` | uuid | Player reference |
| `source_domain` | enum | `gameplay`, `website`, `operations` |
| `progression_event_type` | string | Domain event classification |
| `delta_payload` | jsonb | Structured progression change |
| `idempotency_key` | string | Command dedupe key |
| `conflict_resolution_code` | string | Deterministic resolution marker |
| `recorded_at` | timestamp | Event commit time |

**Validation rules**:
- `idempotency_key` + `player_id` must be unique for accepted mutations.
- Ledger append is immutable after commit.
- Conflicts must resolve to explicit deterministic code path.

## Entity: InventoryLedger

| Field | Type | Description |
|-------|------|-------------|
| `inventory_entry_id` | uuid | Event identity |
| `player_id` | uuid | Player reference |
| `item_id` | string | Canonical item identifier |
| `quantity_delta` | integer | Positive or negative change |
| `source_event_ref` | uuid | Link to originating command/event |
| `settlement_status` | enum | `committed`, `reverted`, `pending-reconciliation` |
| `idempotency_key` | string | Command dedupe key |
| `recorded_at` | timestamp | Event commit time |

**Validation rules**:
- No duplicate accepted grant/consume for the same idempotency scope.
- Negative final inventory is invalid unless policy permits debt semantics.
- Reconciliation events must preserve causal linkage (`source_event_ref`).

## Entity: DomainChangeRecord

| Field | Type | Description |
|-------|------|-------------|
| `change_id` | uuid | Audit record identity |
| `bounded_context` | enum | `gameplay-session-orchestration`, `player-identity-account`, `progression-inventory`, `player-insights-web` |
| `action_type` | string | Command/event label |
| `actor_scope` | jsonb | Subject and authorization context |
| `before_state_digest` | string | Compact prior-state hash/summary |
| `after_state_digest` | string | Compact post-state hash/summary |
| `correlation_id` | string | Cross-domain trace correlation |
| `recorded_at` | timestamp | Audit capture time |

**Validation rules**:
- Every accepted mutating command writes one or more change records.
- Correlation IDs must be present for cross-domain workflow steps.
- Records are append-only for auditability.

## Entity: PlayerInsightView

| Field | Type | Description |
|-------|------|-------------|
| `player_id` | uuid | Player reference |
| `latest_session_summary` | jsonb | Last completed or active session summary |
| `progression_summary` | jsonb | Milestones and trend counters |
| `inventory_trend_summary` | jsonb | Gain/consume trend indicators |
| `no_data` | boolean | Explicit no-data semantic flag |
| `freshness_timestamp` | timestamp | Last materialization/update time |

**Validation rules**:
- Must be derivable from persisted authoritative records only.
- `no_data=true` responses remain valid typed payloads.
- Freshness metadata required for client/web UX trust.

## Relationships

- `PlayerAccount` 1..* `GameplaySession` (participant/owner relationships).
- `PlayerAccount` 1..* `ProgressionLedger`.
- `PlayerAccount` 1..* `InventoryLedger`.
- `DomainChangeRecord` references mutations spanning session/account/ledger entities.
- `PlayerInsightView` is a derived read model from `GameplaySession`, `ProgressionLedger`, `InventoryLedger`, and `DomainChangeRecord`.

## State Transition Notes

- Gameplay continuity path: `active -> recovery-window -> active|completed|aborted`.
- Account status transitions are policy-controlled and must never bypass authorization boundaries.
- Ledger entries are append-only; compensation uses explicit reversal events, not in-place mutation.
