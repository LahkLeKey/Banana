# Data Model: Native Hypersphere K-Means Analytics

## Entity: FeatureVectorBatch
- Description: One analytics compute request containing vectors and runtime parameters.
- Fields:
  - `batchId` (string, deterministic request id)
  - `contractVersion` (uint16)
  - `dimensionCount` (uint16, `1..16`)
  - `clusterCount` (uint16, `1..N`, bounded by request size)
  - `maxIterations` (uint16)
  - `convergenceThresholdQ16` (int32 fixed-point)
  - `vectors` (array of `FeatureVector`)
  - `weights` (optional array of fixed-point weights)
- Validation rules:
  - Reject empty vectors, mixed dimensions, non-finite values, unsupported version.
  - Require deterministic input ordering token.

## Entity: FeatureVector
- Description: A single normalized input feature vector.
- Fields:
  - `vectorId` (uint32 stable id)
  - `componentsQ16` (array<int32> length `dimensionCount`)
- Validation rules:
  - All components must be finite after decode.
  - Length must equal batch `dimensionCount`.

## Entity: ClusterCenter
- Description: Final center for one cluster after fixed-point K-means convergence.
- Fields:
  - `clusterId` (uint16 stable, deterministic order)
  - `centerQ16` (array<int32> length `dimensionCount`)
  - `memberVectorIds` (array<uint32>)
  - `memberCount` (uint32)
- Validation rules:
  - `memberCount >= 0` and equals `memberVectorIds.length`.

## Entity: InscribedHypersphereMetric
- Description: Radius metadata derived from nearest-neighbor inter-center distance.
- Fields:
  - `clusterId` (uint16)
  - `nearestNeighborDistanceQ16` (int32)
  - `inscribedRadiusQ16` (int32)
  - `radiusState` (enum: `ok | single_cluster | near_zero_clamped`)
- Validation rules:
  - Radius is `nearestNeighborDistance / 2` when at least two clusters exist.
  - `inscribedRadiusQ16 >= radiusFloorQ16` if clamped.

## Entity: WeightedVoronoiScore
- Description: Normalized cluster-distance metric per input vector.
- Fields:
  - `vectorId` (uint32)
  - `clusterId` (uint16)
  - `distanceToCenterQ16` (int32)
  - `weightedScoreQ16` (int32)
  - `scoreValidity` (enum: `valid | invalid_radius`)
- Validation rules:
  - `weightedScoreQ16 = distanceToCenterQ16 / max(inscribedRadiusQ16, radiusFloorQ16)`.

## Entity: SpectralProxyMetric
- Description: Harmonic proxy derived from inverse cluster radius.
- Fields:
  - `clusterId` (uint16)
  - `frequencyProxyQ16` (int32)
  - `amplitudeProxyQ16` (int32)
  - `spectralState` (enum: `ok | radius_floor_applied`)
- Validation rules:
  - Frequency proxy uses deterministic scale factor from config.

## Entity: ComputeObservability
- Description: Deterministic execution metadata for diagnostics and rollout.
- Fields:
  - `convergenceStatus` (enum: `converged | max_iterations_exceeded | invalid_input`)
  - `iterationCount` (uint16)
  - `assignmentChangesLastIteration` (uint32)
  - `deterministicHash` (hex string of canonical payload)
  - `endiannessDecodePath` (enum: `little_endian | byte_swapped`)

## Entity: ClusterComputeContractEnvelope
- Description: Versioned payload crossing native ABI and API boundaries.
- Fields:
  - `magic` (uint32)
  - `version` (uint16)
  - `flags` (uint16)
  - `payloadLength` (uint32)
  - `payloadCrc32` (uint32)
  - `result` (aggregate of centers, radii, scores, spectral proxies, observability)
- Validation rules:
  - Unsupported versions return deterministic error code without partial result.
  - CRC and payload length must match before decode.

## Relationships
- `FeatureVectorBatch` 1..* `FeatureVector`
- `FeatureVectorBatch` 1..* `ClusterCenter`
- `ClusterCenter` 1..1 `InscribedHypersphereMetric`
- `FeatureVector` *..* `ClusterCenter` via `WeightedVoronoiScore`
- `ClusterCenter` 1..1 `SpectralProxyMetric`
- `FeatureVectorBatch` 1..1 `ClusterComputeContractEnvelope`
- `ClusterComputeContractEnvelope` 1..1 `ComputeObservability`

## State Transitions
1. `received` -> `validated`: request decoded, schema/version checks pass.
2. `validated` -> `iterating`: fixed-point K-means loop active.
3. `iterating` -> `converged`: no assignment change or threshold reached.
4. `iterating` -> `max_iterations_exceeded`: deterministic failure state, no production promotion.
5. `converged` -> `scored`: radius, weighted Voronoi, and spectral proxies computed.
6. `scored` -> `serialized`: ABI envelope encoded with canonical endianness.
7. `serialized` -> `published`: API response emitted to consumers.
