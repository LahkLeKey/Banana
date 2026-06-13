# Contract: Native Hypersphere K-Means ABI Envelope

## Scope
Versioned binary envelope for hypersphere K-means outputs crossing native C runtime, TypeScript FFI service, and API orchestration.

## Canonical Encoding
- Byte order: little-endian canonical on wire.
- Decoder behavior:
  - If host is little-endian, decode directly.
  - If host is big-endian fixture, byte-swap numeric primitives before semantic validation.
- Numeric encoding:
  - Fixed-point metrics use signed `Q16.16` stored as int32.
  - Counts/ids use unsigned integer widths shown below.

## Envelope Layout
1. `magic` (`uint32`) fixed value `0x4B4D4842` (`BHMK`).
2. `version` (`uint16`) initial value `1`.
3. `flags` (`uint16`) bitmask (`bit0: radius_clamp_applied`, `bit1: score_invalid_present`).
4. `payloadLength` (`uint32`) bytes after header.
5. `payloadCrc32` (`uint32`) CRC over payload bytes.
6. `payload` (variable) structured sections below.

## Payload Sections
1. `meta`
   - `clusterCount` (`uint16`)
   - `vectorCount` (`uint32`)
   - `dimensionCount` (`uint16`)
   - `iterationCount` (`uint16`)
   - `convergenceStatus` (`uint8` enum)
2. `centers`
   - repeated `clusterCount` entries
   - each entry: `clusterId` (`uint16`), `memberCount` (`uint32`), `centerQ16[dimensionCount]` (`int32[]`)
3. `radii`
   - repeated `clusterCount` entries
   - each entry: `clusterId` (`uint16`), `nearestNeighborDistanceQ16` (`int32`), `inscribedRadiusQ16` (`int32`), `radiusState` (`uint8` enum)
4. `scores`
   - repeated `vectorCount * clusterCount` entries
   - each entry: `vectorId` (`uint32`), `clusterId` (`uint16`), `distanceToCenterQ16` (`int32`), `weightedScoreQ16` (`int32`), `scoreValidity` (`uint8` enum)
5. `spectral`
   - repeated `clusterCount` entries
   - each entry: `clusterId` (`uint16`), `frequencyProxyQ16` (`int32`), `amplitudeProxyQ16` (`int32`), `spectralState` (`uint8` enum)

## Error Contract
- `ERR_UNSUPPORTED_VERSION`: version unknown, do not publish partial payload.
- `ERR_BAD_MAGIC`: envelope header mismatch.
- `ERR_BAD_CRC`: checksum mismatch.
- `ERR_PAYLOAD_TRUNCATED`: payload length underflow.
- `ERR_NON_FINITE_VALUE`: decoded value invalid for semantic domain.
- `ERR_CONVERGENCE_FAILURE`: max iterations exceeded; payload may include metadata only with no promotable metrics.

## Determinism Rules
- Cluster and metric arrays are sorted by deterministic `clusterId`.
- Score rows are sorted by `vectorId` then `clusterId`.
- Tie breaks in assignment select lowest `clusterId`.
- All fixed-point divisions use deterministic rounding mode (half-away-from-zero).
