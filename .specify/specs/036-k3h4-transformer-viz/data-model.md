# Data Model: K3H4 Transformer-Comparative Visualizations

**Feature**: `036-k3h4-transformer-viz`
**Phase**: 1 — Design

---

## Core Entities

### TrainingArtifact

An immutable, versioned, little-endian binary file written by the native export hook for a specific session + epoch.

| Field | Type | Notes |
|---|---|---|
| `artifact_type` | `uint8` | `0x01 = TRAINING_ARTIFACT` |
| `contract_version` | `uint16` | ABI envelope version (incremented if format changes) |
| `byte_order_tag` | `uint32` | `0x01020304` — endianness sentinel |
| `session_id` | `char[16]` | Null-padded UTF-8 session identifier |
| `epoch_index` | `int32` | 0-based epoch number |
| `mode_flag` | `int32` | `0 = multiplicative`, `1 = power` |
| `spectral_active` | `int32` | `0 = disabled`, `1 = affinity-graph` |
| `cluster_count` | `int32` | Number of clusters serialized |
| `dimension` | `int32` | Feature dimension `d` |
| `clusters[]` | `ClusterRecord[cluster_count]` | Per-cluster geometry (see below) |
| `integrity_marker` | `uint32` | `0xDEADBEEF` |
| `crc32` | `uint32` | CRC32 of all preceding bytes |
| `contract_status` | `int32` | `RUNTIME_K3H4_CONTRACT_OK` on write |

**Storage path**: `artifacts/native/k3h4-training/<session_id>/epoch-<N>.bin`

**Invariants**:
- File is written atomically via temp-file + rename.
- Trailing `0xDEADBEEF` + CRC32 must be present and valid; absent/mismatched → `artifact_incomplete`.
- ABI version is incremented if any field layout changes; API rejects unknown versions with `version_unsupported`.

---

### ClusterRecord (embedded in TrainingArtifact)

| Field | Type | Notes |
|---|---|---|
| `center[d]` | `float32[d]` | Cluster center in feature space |
| `inscribed_radius` | `float32` | Inscribed hypersphere radius |
| `multiplicative_score` | `int32` | Weighted Voronoi score, Q16.16 fixed-point |
| `power_score` | `int32` | Power-mode Voronoi score, Q16.16 fixed-point |
| `spectral_proxy` | `int32` | Spectral proxy value, Q16.16 (0 if spectral disabled) |

**Ordering**: ascending by `center[0]` (x-component), ties broken by `center[1]`, etc. — deterministic across platforms.

---

### ScalingBenchmarkResult

Machine-readable benchmark output, written to `artifacts/native/k3h4-scaling-benchmark.json` by the CTest benchmark.

```json
{
  "schema_version": 1,
  "generated_at_utc": "2026-06-14T00:00:00Z",
  "platform": "...",
  "series": [
    {
      "n": 64,
      "k3h4_ns": 12345,
      "attention_ns": 11111
    }
  ]
}
```

| Field | Type | Notes |
|---|---|---|
| `schema_version` | `int` | `1` — versioned for future extension |
| `generated_at_utc` | `string` | ISO-8601 UTC timestamp |
| `platform` | `string` | CPU model + OS string for context |
| `series[].n` | `int` | Input size |
| `series[].k3h4_ns` | `int` | Wall-clock nanoseconds for k3h4 power-mode clustering |
| `series[].attention_ns` | `int` | Wall-clock nanoseconds for transformer attention cost model |

**Invariants**:
- `n` values must include all of `{64, 256, 1024, 4096, 16384}` (FR-006).
- JSON is written atomically; partial files are discarded on re-run.
- Re-running on the same hardware must produce `k3h4_ns` and `attention_ns` within ±10% (FR-008).

---

### ConfidenceTimeSeries

Per-session ordered sequence of per-epoch ML confidence scalars.

| Field | Type | Notes |
|---|---|---|
| `session_id` | `string` | Session identifier |
| `status` | `"active" \| "completed" \| "pending"` | Session lifecycle state |
| `epochs[]` | `EpochConfidence[]` | Ordered by `epoch_index` ascending |

### EpochConfidence

| Field | Type | Notes |
|---|---|---|
| `epoch_index` | `int` | 0-based epoch number |
| `confidence` | `number` | ML confidence scalar derived from weighted Voronoi scores |
| `mode` | `"multiplicative" \| "power"` | Mode used for this epoch |

---

### TrainingSession

| Field | Type | Notes |
|---|---|---|
| `session_id` | `string` | Caller-provided or API-generated unique identifier |
| `created_at` | `string` | ISO-8601 UTC timestamp |
| `mode` | `"multiplicative" \| "power"` | Default mode for session |

**Notes**: Session lifecycle is managed by the caller. The API stores session metadata in-memory (or file-backed under `artifacts/native/k3h4-training/<session_id>/`). Session objects are not persisted to PostgreSQL in this feature.

---

## State Transitions

### TrainingArtifact write lifecycle

```
EXPORT_REQUESTED
    → PREFLIGHT_OK / PREFLIGHT_FAILED
PREFLIGHT_OK
    → WRITING (atomic temp file)
WRITING
    → COMMITTED (temp rename succeeds)
    → ABANDONED (interrupted — no partial file exposed)
COMMITTED
    → READABLE (integrity marker + CRC validated by reader)
    → ARTIFACT_INCOMPLETE (marker or CRC absent/mismatched)
```

### ConfidenceTimeSeries read lifecycle

```
SESSION_ID_UNKNOWN   → 404 / artifact_not_found
SESSION_PENDING      → 200 { status: "pending", epochs: [] }
SESSION_ACTIVE       → 200 { status: "active", epochs: [...] }
SESSION_COMPLETED    → 200 { status: "completed", epochs: [...] }
ARTIFACT_INCOMPLETE  → 200 (epoch omitted from series, not 500)
```

---

## Validation Rules

- `session_id`: non-empty, max 15 characters (15 + null-terminator = 16 bytes), alphanumeric + hyphen only.
- `epoch_index`: non-negative integer.
- `mode` query param: `"multiplicative"` or `"power"`; default `"power"` if omitted.
- ABI `contract_version`: must match supported versions list; unknown → `version_unsupported` error.
- `inscribed_radius`: non-negative; `0.0` permitted for degenerate single-token cluster.
- `cluster_count`: `>= 1`; artifact with `cluster_count = 0` is `artifact_incomplete`.
