# Contract: API Netcode K3H4 Response

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
  "interactionSignal": 73,
  "k3h4Mode": "power",
  "spectralMode": "affinity-graph"
}
```

### Request Notes
- `k3h4Mode` optional values: `multiplicative | power`
- `spectralMode` optional values: `disabled | affinity-graph`
- If omitted, the API orchestration defaults to:
  - `k3h4Mode = multiplicative`
  - `spectralMode = disabled`

## Successful Response
```json
{
  "contractVersion": 1,
  "lspRepresentation": {
    "language": "netcode.analytics.v1",
    "boundedContext": "netcode",
    "aggregate": "k3h4",
    "authority": "server-native",
    "contractVersion": 1,
    "deterministicHash": 123456,
    "rollout": {
      "enabled": true,
      "cohort": "all"
    }
  },
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
  "k3h4Projection": {
    "dimensions": 8,
    "nodes": [
      {
        "x": 0.5,
        "y": 0.3,
        "z": 0.2,
        "coherence": 91,
        "inradius": 0.34,
        "nearestNeighborDistance": 0.7
      }
    ],
    "alignment": 42,
    "radialStability": 73
  },
  "k3h4": {
    "centers": [
      {
        "clusterId": 0,
        "centerQ16": [65536, 32768],
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
      "convergenceStatus": 0,
      "iterationCount": 7,
      "assignmentChangesLastIteration": 0,
      "deterministicHash": 123456,
      "endiannessDecodePath": "little-endian"
    }
  },
  "k3h4Runtime": {
    "mode": "power",
    "spectralActivation": "affinity-graph"
  },
  "abiLayers": [
    {
      "layer": "k3h4",
      "contractVersion": 1,
      "status": "ok",
      "payloadBytes": 892,
      "byteOrderTag": 16909060,
      "deterministicHash": 123456
    }
  ],
  "abiLayerCoverage": {
    "expectedLayers": ["learning", "reward", "link", "vector", "k3h4"],
    "presentLayers": ["learning", "reward", "link", "vector", "k3h4"],
    "missingLayers": [],
    "completeness": 1,
    "complete": true
  },
  "rollout": {
    "enabled": true,
    "cohort": "all"
  }
}
```

## Error Response
```json
{
  "errorCode": "ERR_UNSUPPORTED_VERSION",
  "message": "Unsupported native k3h4 contract version",
  "contractVersion": 1,
  "retryable": false,
  "rollout": {
    "enabled": true,
    "cohort": "all"
  }
}
```

## Behavioral Rules
- Production compute always occurs in API/native path.
- React consumers render returned values and never run production fallback compute.
- Route rejects invalid `k3h4Mode` or `spectralMode` values with HTTP `400`.
- Route returns deterministic ordering for layer metadata arrays.
- Runtime mode/spectral selection is surfaced via `k3h4Runtime` and never inferred client-side.

## Rollout Controls
- `BANANA_NETCODE_K3H4_ENABLED=true|false`
- `BANANA_NETCODE_K3H4_COHORT` for staged activation.
- Disabled flag returns HTTP `503` with rollout metadata and no analytics payload.
