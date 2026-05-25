# Contract: Failure, Retry, and Reconciliation

## Purpose

Define deterministic handling of generation/persistence/replay faults and retries so canonical area outcomes remain consistent.

## Failure Classification

### Recoverable

- `GENERATION_TIMEOUT`
- `PERSISTENCE_TIMEOUT`
- `DEPENDENCY_UNAVAILABLE`
- `TRANSIENT_LOCK_CONTENTION`

### Non-Recoverable

- `INVALID_AREA_IDENTITY`
- `CONTRACT_MISMATCH`
- `CORRUPT_DELTA_LOG`
- `UNSUPPORTED_POLICY_VERSION`

## Retry Contract

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `failureId` | uuid | yes | Original failure record |
| `areaId` | string | yes | Target area |
| `operationType` | enum | yes | `generation`, `persistence`, `replay`, `merge` |
| `retryAttempt` | integer | yes | Incrementing retry number |
| `retryContractFingerprint` | string | yes | Canonical hash of retry-relevant inputs |
| `isRecoverable` | boolean | yes | Derived from failure class |

## Deterministic Retry Rules

- Recoverable retries MUST preserve the same canonical input fingerprint as the original attempt.
- Retries MUST maintain idempotency key lineage for delta-related operations.
- Retry success MUST not create a second area identity for the same tuple.
- Retry outputs must match canonical parity expectations for equivalent successful non-faulted runs.

## Reconciliation Behavior

- If retry succeeds, authoritative state advances exactly once.
- If retry repeats with unchanged recoverable condition, state remains unchanged and failure history appends.
- Non-recoverable failures transition to operator-required state; automated retries are blocked.

## Operator Context Requirements

For all non-recoverable failures, emit:

- Area identity tuple
- Contract version set
- Failure code and stage
- Last authoritative area state version
- Correlation/request IDs
