# Contract: API K3H4 Training Visualization Endpoints

## Scope

New API endpoints for k3h4 transformer-comparative visualizations, registered in `src/typescript/api/src/routes/netcode.ts`. All endpoints follow the existing controller → service → native interop pattern.

---

## Endpoint 1: GET /api/netcode/k3h4/scaling-benchmark

Returns the measured scaling series from the latest native CTest benchmark artifact.

### Request

```
GET /api/netcode/k3h4/scaling-benchmark
```

No query parameters required.

### Success Response (200)

```json
{
  "contractVersion": 1,
  "schemaVersion": 1,
  "generatedAtUtc": "2026-06-14T00:00:00Z",
  "platform": "<cpu>/<os>",
  "series": [
    { "n": 64,    "k3h4Ns": 12345,    "attentionNs": 11111 },
    { "n": 256,   "k3h4Ns": 49000,    "attentionNs": 180000 },
    { "n": 1024,  "k3h4Ns": 196000,   "attentionNs": 2900000 },
    { "n": 4096,  "k3h4Ns": 786000,   "attentionNs": 46000000 },
    { "n": 16384, "k3h4Ns": 3145000,  "attentionNs": 737000000 }
  ],
  "metadata": {
    "calibratedSizes": [64, 256, 1024, 4096, 16384],
    "extrapolatedAbove": null
  }
}
```

### Error Responses

| Status | Code | Condition |
|---|---|---|
| 404 | `benchmark_not_found` | Artifact file missing (CTest not yet run) |
| 503 | `benchmark_unavailable` | File I/O error reading artifact |

---

## Endpoint 2: POST /api/netcode/k3h4/training-session

Creates a training session and returns a session identifier.

### Request

```json
{
  "mode": "power"
}
```

| Field | Type | Required | Default |
|---|---|---|---|
| `mode` | `"multiplicative" \| "power"` | No | `"power"` |

### Success Response (201)

```json
{
  "sessionId": "sess-abc123",
  "mode": "power",
  "createdAtUtc": "2026-06-14T00:00:00Z"
}
```

---

## Endpoint 3: GET /api/netcode/k3h4/training-session/:id/confidence

Returns the per-epoch confidence time-series for a session.

### Request

```
GET /api/netcode/k3h4/training-session/:id/confidence?mode=power
```

| Query Param | Type | Required | Default |
|---|---|---|---|
| `mode` | `"multiplicative" \| "power"` | No | `"power"` |

### Success Response (200)

```json
{
  "contractVersion": 1,
  "sessionId": "sess-abc123",
  "status": "active",
  "mode": "power",
  "epochs": [
    { "epochIndex": 0, "confidence": 0.41, "mode": "power" },
    { "epochIndex": 1, "confidence": 0.57, "mode": "power" }
  ],
  "metadata": {
    "peakEpoch": 1,
    "rollingAverage3": 0.49
  }
}
```

**Pending session** (no epochs yet):

```json
{
  "contractVersion": 1,
  "sessionId": "sess-abc123",
  "status": "pending",
  "mode": "power",
  "epochs": [],
  "metadata": {
    "peakEpoch": null,
    "rollingAverage3": null
  }
}
```

### Error Responses

| Status | Code | Condition |
|---|---|---|
| 404 | `session_not_found` | Session ID unknown |
| 400 | `invalid_mode` | mode param not `multiplicative` or `power` |

---

## Endpoint 4: GET /api/netcode/k3h4/training-session/:id/epoch/:n/geometry

Returns cluster geometry state for a specific epoch.

### Request

```
GET /api/netcode/k3h4/training-session/:id/epoch/:n/geometry?mode=power
```

| Query Param | Type | Required | Default |
|---|---|---|---|
| `mode` | `"multiplicative" \| "power"` | No | `"power"` |

### Success Response (200)

```json
{
  "contractVersion": 1,
  "sessionId": "sess-abc123",
  "epochIndex": 2,
  "mode": "power",
  "spectralActive": true,
  "dimension": 8,
  "projectionMetadata": {
    "method": "pca",
    "components": 2,
    "explainedVariance": [0.72, 0.18]
  },
  "clusters": [
    {
      "clusterId": 0,
      "center": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8],
      "center2d": [0.31, -0.12],
      "inscribedRadius": 0.34,
      "multiplicativeScore": 0.88,
      "powerScore": 0.71,
      "spectralProxy": 0.55
    }
  ]
}
```

### Error Responses

| Status | Code | Condition |
|---|---|---|
| 404 | `session_not_found` | Session ID unknown |
| 404 | `artifact_not_found` | Epoch artifact file missing |
| 422 | `artifact_incomplete` | Artifact exists but integrity/CRC check failed |
| 422 | `version_unsupported` | Artifact ABI version not supported |
| 400 | `invalid_mode` | mode param invalid |

---

## Cross-Endpoint Determinism Contract

- Identical `(session_id, epoch_index, mode)` inputs MUST produce byte-identical JSON response bodies (FR-013).
- Confidence scalars are computed from fixed-point scores in the artifact; same artifact → same confidence.
- Response JSON is serialized with sorted keys and no trailing whitespace to ensure byte identity.

## Route Registration Pattern

All four endpoints follow the existing Fastify pattern in `netcode.ts`:
- Route registered via `fastify.get(...)` / `fastify.post(...)` with explicit schema.
- Handler calls a service function; service owns artifact I/O and type mapping.
- No business logic in route handler.
- Validation errors → 400/422 with structured `{ error: { code, message } }` body.
- Service errors → 500 (unexpected) or 4xx (contract-defined codes above).
