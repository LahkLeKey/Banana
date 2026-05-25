# Contract: Deterministic Generation

## Purpose

Define canonical inputs and outputs for deterministic area baseline generation across terrain and mesh vectors.

## Inputs

| Field | Type | Required | Notes |
|-------|------|----------|-------|
| `worldId` | uuid | yes | World identity |
| `rootSeed` | string | yes | Immutable root seed material |
| `derivationPolicyVersion` | string | yes | Seed derivation algorithm version |
| `areaIdentity` | object | yes | `laneId`, `chunkX`, `chunkY`, `partitionEpoch`, `identityHash` |
| `partitionContractVersion` | string | yes | Chunk partition compatibility version |
| `generationContractVersion` | string | yes | Generation pipeline version |
| `requestId` | uuid | yes | Traceability and idempotency correlation |

## Canonical Input Normalization

1. Normalize field names and ordering exactly as documented in this contract.
2. Normalize numeric coordinates to signed integer canonical form.
3. Normalize strings using UTF-8 with no locale-dependent transforms.
4. Compute `generationInputFingerprint = hash(normalizedInputPayload)`.

Equivalent normalized inputs MUST produce equivalent baseline signatures.

## Outputs

| Field | Type | Required | Notes |
|-------|------|----------|-------|
| `baselineId` | uuid | yes | Generated baseline artifact ID |
| `areaId` | string | yes | Stable area identity |
| `terrainSignature` | string | yes | Deterministic terrain signature |
| `meshVectorSignature` | string | yes | Deterministic mesh vector signature |
| `boundaryHash` | string | yes | Deterministic chunk boundary signature |
| `contractVersion` | string | yes | Returned generation contract version |
| `generationInputFingerprint` | string | yes | Echo of canonical input fingerprint |

## Determinism Rules

- Same canonical input fingerprint MUST produce same `terrainSignature`, `meshVectorSignature`, and `boundaryHash`.
- Any contract-affecting algorithm change MUST increment `generationContractVersion`.
- Mixed-version generation output comparison is invalid unless an explicit compatibility adapter exists.

## Error Codes

- `GENERATION_TIMEOUT` (recoverable)
- `DEPENDENCY_UNAVAILABLE` (recoverable)
- `INVALID_AREA_IDENTITY` (non-recoverable)
- `CONTRACT_VERSION_UNSUPPORTED` (non-recoverable)

## Retry Rules

- Recoverable retries MUST reuse identical canonical inputs and preserve `requestId` lineage.
- Retry outputs are valid only if deterministic signatures match expected parity behavior.
