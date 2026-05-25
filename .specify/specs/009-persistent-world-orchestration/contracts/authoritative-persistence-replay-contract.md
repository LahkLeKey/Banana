# Contract: Authoritative Persistence and Replay

## Purpose

Define server-authoritative area persistence boundaries, delta application, and deterministic replay semantics.

## Persistence Boundary

1. Deterministic baseline artifacts are persisted separately from interaction deltas.
2. Authoritative area state is reconstructed as:

`canonicalState = Replay(MergePolicy, baseline, orderedDeltas)`

## Baseline Record

| Field | Type | Required |
|-------|------|----------|
| `baselineId` | uuid | yes |
| `areaId` | string | yes |
| `generationContractVersion` | string | yes |
| `terrainSignature` | string | yes |
| `meshVectorSignature` | string | yes |

## Delta Record

| Field | Type | Required |
|-------|------|----------|
| `deltaId` | uuid | yes |
| `areaId` | string | yes |
| `baseAreaStateVersion` | bigint | yes |
| `deltaSequence` | bigint | yes |
| `idempotencyKey` | string | yes |
| `deltaPayload` | json | yes |

## Authoritative State Record

| Field | Type | Required |
|-------|------|----------|
| `areaId` | string | yes |
| `areaStateVersion` | bigint | yes |
| `appliedDeltaThroughSequence` | bigint | yes |
| `canonicalStateSignature` | string | yes |
| `mergePolicyVersion` | string | yes |

## Replay and Merge Rules

- Replay order is deterministic by (`areaStateVersion`, `deltaSequence`, `deltaId`).
- Equivalent baseline + equivalent ordered delta set MUST produce identical `canonicalStateSignature`.
- Stale deltas (`baseAreaStateVersion` older than authoritative state) are reconciled or rejected deterministically by merge policy.
- Duplicate idempotency keys for same area must no-op.

## Versioning Rules

- `areaStateVersion` increments monotonically for every accepted canonical transition.
- Any merge-rule change requires new `mergePolicyVersion` and compatibility note.

## Error Codes

- `STALE_DELTA_VERSION`
- `DUPLICATE_DELTA_IDEMPOTENCY_KEY`
- `DELTA_PAYLOAD_INVALID`
- `REPLAY_INTEGRITY_FAILURE`
