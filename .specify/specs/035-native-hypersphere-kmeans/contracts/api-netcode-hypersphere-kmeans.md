# Contract: API Netcode Hypersphere K-Means Response

## Endpoint
- Method: `POST`
- Path: `/api/netcode/analytics`
- Ownership: `src/typescript/api/src/routes/netcode.ts`

## Request Body
```json
{
  "callDensity": 10,
  "questPercent": 20,
  "playerLevel": 30,
  "comboStreak": 40,
  "branchPressure": 50,
  "dependencyPulse": 60,
  "workflowDepth": 2,
  "networkDimensions": 8,
  "modelConfidence": 77,
  "policyMomentum": 66,
  "kmeans": {
    "clusterCount": 4,
    "maxIterations": 32,
    "convergenceThresholdQ16": 8
  }
}
```

## Successful Response
```json
{
  "contractVersion": 1,
  "reward": {
    "neuralRelevanceScore": 88,
    "projectedRewardXp": 144,
    "rewardTier": 1
  },
  "link": {
    "intel": 9,
    "objectives": 8,
    "player": 7,
    "ops": 6
  },
  "vector": {
    "dimensions": 8,
    "nodeVectors": [[0.1, 0.2], [0.3, 0.4], [0.5, 0.6], [0.7, 0.8]],
    "contractStrength": [11, 22, 33, 44]
  },
  "hypersphereKmeans": {
    "centers": [
      {
        "clusterId": 0,
        "centerQ16": [65536, 32768],
        "memberVectorIds": [1, 4, 9],
        "memberCount": 3
      }
    ],
    "radii": [
      {
        "clusterId": 0,
        "nearestNeighborDistanceQ16": 49152,
        "inscribedRadiusQ16": 24576,
        "radiusState": "ok"
      }
    ],
    "weightedVoronoiScores": [
      {
        "vectorId": 1,
        "clusterId": 0,
        "distanceToCenterQ16": 12288,
        "weightedScoreQ16": 32768,
        "scoreValidity": "valid"
      }
    ],
    "spectralProxy": [
      {
        "clusterId": 0,
        "frequencyProxyQ16": 131072,
        "amplitudeProxyQ16": 65536,
        "spectralState": "ok"
      }
    ],
    "observability": {
      "convergenceStatus": "converged",
      "iterationCount": 7,
      "assignmentChangesLastIteration": 0,
      "scoringValidity": "valid",
      "deterministicHash": "a8b1f0f3c9..."
    }
  }
}
```

## Error Response
```json
{
  "errorCode": "ERR_UNSUPPORTED_VERSION",
  "message": "Unsupported native hypersphere contract version",
  "contractVersion": 1,
  "retryable": false
}
```

## Behavioral Rules
- Production compute always occurs in API/native path.
- React consumers render returned values and never run production fallback compute.
- Route returns deterministic ordering for all arrays (`clusterId`, then `vectorId`).
- When convergence fails, response includes observability metadata and excludes promotable score payloads.

## Rollout Controls
- `BANANA_NETCODE_HYPERSPHERE_KMEANS_ENABLED=true|false`
- `BANANA_NETCODE_HYPERSPHERE_KMEANS_COHORT` for staged activation.
- Disabled flag returns existing `reward/link/vector/hypersphere` payload without `hypersphereKmeans`.
