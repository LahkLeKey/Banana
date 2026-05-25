# Contract: Area Identity and Chunk Partition

## Purpose

Define stable area identity and chunk partition boundaries so area membership and adjacency remain deterministic across sessions and players.

## Area Identity Tuple

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `worldId` | uuid | yes | World scope |
| `laneId` | string | yes | Logical lane/shard |
| `chunkX` | integer | yes | Chunk X coordinate |
| `chunkY` | integer | yes | Chunk Y coordinate |
| `partitionEpoch` | integer | yes | Partition compatibility epoch |
| `identityHash` | string | yes | Canonical hash of tuple above |

## Partition Metadata

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `areaId` | string | yes | Stable area identifier |
| `boundaryHash` | string | yes | Deterministic boundary descriptor |
| `adjacentAreaIds` | array<string> | yes | Canonical ordered adjacency list |
| `seamPolicyVersion` | string | yes | Boundary continuity policy version |
| `meshVectorFrame` | string | yes | Canonical frame for mesh vectors |

## Invariants

- `identityHash` MUST be reproducible from canonical tuple normalization.
- `areaId` MUST map 1:1 to identity tuple in a given `partitionEpoch`.
- Equivalent identity tuples MUST always resolve to the same partition metadata.
- Chunk adjacency ordering MUST be deterministic.

## Compatibility and Migration

- Partition model changes require incremented `partitionEpoch` and migration notes.
- Legacy epoch replay requires an explicit adapter path to avoid identity collisions.

## Validation Failures

- `IDENTITY_HASH_MISMATCH`
- `PARTITION_BOUNDARY_DRIFT`
- `ADJACENCY_SET_INCONSISTENT`
- `UNSUPPORTED_PARTITION_EPOCH`
