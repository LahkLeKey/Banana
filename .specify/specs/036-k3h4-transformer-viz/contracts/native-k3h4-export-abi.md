# Contract: Native K3H4 Training Artifact ABI

## Scope

Versioned binary envelope for k3h4 training artifact files written by `banana_k3h4_export_training_artifact()` and consumed by the API training service. Extends the existing 035 ABI envelope with a `TRAINING_ARTIFACT` record type.

## Canonical Encoding

- Byte order: little-endian (enforced by hardware preflight — same as 035 ABI contract).
- Floating-point fields: IEEE 754 `float32`, little-endian.
- Fixed-point score fields: signed `Q16.16` as `int32`, little-endian.
- String fields: null-padded UTF-8.

## Hardware Preflight (Inherited from 035)

Must pass before `banana_k3h4_export_training_artifact()` executes:
- `hardware_byte_order_tag == 0x01020304`
- `hardware_dtype_tag == RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED (1)`
- `hardware_alignment_bytes == RUNTIME_K3H4_ALIGNMENT_BYTES_4 (4)`

Preflight failure → function returns explicit error code, no artifact file is created.

## Binary Layout

```
Offset   Size   Field
0        1      artifact_type          = 0x01 (TRAINING_ARTIFACT)
1        2      contract_version       uint16 LE
3        4      byte_order_tag         uint32 LE = 0x01020304
7        16     session_id             char[16], null-padded UTF-8
23       4      epoch_index            int32 LE
27       4      mode_flag              int32 LE (0=multiplicative, 1=power)
31       4      spectral_active        int32 LE (0=disabled, 1=affinity-graph)
35       4      cluster_count          int32 LE
39       4      dimension              int32 LE
43       ...    clusters[]             cluster_count × ClusterRecord
-8       4      integrity_marker       uint32 LE = 0xDEADBEEF
-4       4      crc32                  uint32 LE (CRC32 of bytes [0..-9])
```

### ClusterRecord Layout (per cluster, length = 4*d + 12 bytes)

```
Offset   Size   Field
0        4*d    center[d]              float32[d] LE
4*d      4      inscribed_radius       float32 LE
4*d+4    4      multiplicative_score   int32 LE (Q16.16)
4*d+8    4      power_score            int32 LE (Q16.16)
4*d+12   4      spectral_proxy         int32 LE (Q16.16, 0 if spectral disabled)
```

## Write Contract

- File is written atomically: written to `<path>.tmp`, then renamed to `<path>` on success.
- On interrupted write, no partial file is exposed to readers.
- `integrity_marker` (0xDEADBEEF) is always the last non-CRC field.
- CRC32 covers all bytes from offset 0 through `integrity_marker` (inclusive).

## Read Contract

Reader must:
1. Verify `byte_order_tag == 0x01020304`.
2. Verify `contract_version` is in the supported set; unknown → return `RUNTIME_K3H4_CONTRACT_UNSUPPORTED_VERSION`.
3. Verify `integrity_marker == 0xDEADBEEF`; absent/wrong → return `artifact_incomplete`.
4. Verify CRC32; mismatch → return `RUNTIME_K3H4_CONTRACT_CRC_MISMATCH`.

## Export Function Signature

```c
// src/native/engine/runtime/netcode/k3h4/netcode_k3h4_export.h
typedef enum {
    BANANA_K3H4_EXPORT_OK                  = 0,
    BANANA_K3H4_EXPORT_PREFLIGHT_FAILED    = 1,
    BANANA_K3H4_EXPORT_WRITE_FAILED        = 2,
    BANANA_K3H4_EXPORT_INVALID_SESSION     = 3,
    BANANA_K3H4_EXPORT_VERSION_UNSUPPORTED = 4,
} BananaK3h4ExportStatus;

BananaK3h4ExportStatus banana_k3h4_export_training_artifact(
    const char *session_id,   // max 15 chars + null terminator
    int32_t epoch_index,
    const RuntimeNetcodeK3h4Output *state,
    const char *out_dir       // base output directory
);
```

## Error Status Codes

| Code | Meaning |
|---|---|
| `BANANA_K3H4_EXPORT_OK` | Artifact written and CRC verified |
| `BANANA_K3H4_EXPORT_PREFLIGHT_FAILED` | Hardware preflight failed; no file written |
| `BANANA_K3H4_EXPORT_WRITE_FAILED` | I/O failure; no partial file exposed |
| `BANANA_K3H4_EXPORT_INVALID_SESSION` | session_id too long or invalid chars |
| `BANANA_K3H4_EXPORT_VERSION_UNSUPPORTED` | Requested ABI version not supported |

## Scaling Benchmark Output Format

The CTest benchmark writes to `artifacts/native/k3h4-scaling-benchmark.json`:

```json
{
  "schema_version": 1,
  "generated_at_utc": "2026-06-14T00:00:00Z",
  "platform": "<cpu>/<os>",
  "series": [
    { "n": 64,    "k3h4_ns": 12345,    "attention_ns": 11111 },
    { "n": 256,   "k3h4_ns": 49000,    "attention_ns": 180000 },
    { "n": 1024,  "k3h4_ns": 196000,   "attention_ns": 2900000 },
    { "n": 4096,  "k3h4_ns": 786000,   "attention_ns": 46000000 },
    { "n": 16384, "k3h4_ns": 3145000,  "attention_ns": 737000000 }
  ]
}
```

## Determinism Rules

- Cluster ordering: ascending `center[0]`, ties broken by `center[1]`, …
- Score fields: deterministic Q16.16 fixed-point; no floating-point accumulation in scores.
- Spectral proxy: `0` for disabled mode; deterministic for affinity-graph mode.
- CRC32 is canonical (ISO 3309 polynomial `0xEDB88320`).
