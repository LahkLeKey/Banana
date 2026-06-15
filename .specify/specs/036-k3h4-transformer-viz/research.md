# Research: K3H4 Transformer-Comparative Visualizations

**Feature**: `036-k3h4-transformer-viz`
**Phase**: 0 — Research (Completed, findings carried forward from spec motivation + 035 implementation)

---

## Asymptotic Scaling Analysis

**Decision**: Use wall-clock measured benchmark (CTest) as the authoritative scaling source; annotate UI slopes from measured data, not theoretical constants.

**Rationale**: The spec explicitly requires "measured slope within ±5% of 2.0 / 1.0 derived from native CTest benchmark outputs rather than synthetic data" (SC-002). Theoretical $O(n^2)$ vs $O(n \cdot k)$ expressions are used only as the expected-slope targets against which measured values are validated.

**Scaling model confirmed**:
- Transformer self-attention: $O(n^2 d)$ — quadratic in sequence length.
- K3H4 power-mode clustering: $O(n \cdot k \cdot d \cdot i)$ — linear in $n$ because $k$, $d$, $i$ are runtime constants.
- Nyström/randomized-SVD spectral approximation: $O(n \cdot r)$ where $r \ll n$.

**Benchmark input sizes**: $n \in \{64, 256, 1024, 4096, 16384\}$ — five points covering three decades, sufficient to distinguish quadratic from linear on a log-log axis.

**Alternatives considered**:
- Synthetic constants only (rejected: violates SC-002, NFR-005).
- FLOP-count model (rejected: hardware variability makes FLOP counts non-representative without wall-clock grounding).

---

## Training Artifact Binary Format

**Decision**: Extend the existing versioned little-endian ABI envelope from 035 with a new `TRAINING_ARTIFACT` record type. All existing contract_version, byte_order_tag, crc32, and contract_status semantics are preserved.

**New fields added to artifact envelope**:
- `artifact_type` discriminant byte: `0x01 = TRAINING_ARTIFACT`
- `session_id` (16 bytes, null-padded UTF-8)
- `epoch_index` (int32, little-endian)
- `mode_flag` (int32): `0 = multiplicative`, `1 = power`
- `spectral_active` (int32): `0 = disabled`, `1 = affinity-graph`
- Per-cluster: center vector (float32 × d), inscribed radius (float32), multiplicative score (Q16.16 int32), power score (Q16.16 int32), spectral proxy value (Q16.16 int32)
- Trailing integrity marker: `uint32 0xDEADBEEF` + CRC32 of all preceding bytes.

**Determinism**: Cluster ordering is deterministic (ascending center-x tie-break, as established in 035). Scores are fixed-point Q16.16 with deterministic rounding. Platform byte order is enforced by existing hardware preflight.

**Partial-write detection**: Trailing `0xDEADBEEF` integrity marker is absent in truncated files; reader returns `artifact_incomplete` status before exposing any geometry.

**Alternatives considered**:
- JSON artifact files (rejected: non-deterministic floating-point serialization across platforms, too large for epoch-by-epoch writes in hot training loop).
- Separate schema for 036 (rejected: re-using the existing ABI envelope keeps the versioning and endianness contracts in one place).

---

## API Pattern for New Endpoints

**Decision**: Follow existing controller → service → native interop flow in `src/typescript/api/src/routes/netcode.ts` and `src/typescript/api/src/services/nativeNetcode.ts`. New endpoints are registered in the existing `netcode.ts` route file. New service functions are added to a new `k3h4TrainingService.ts` — avoiding direct mutation of the existing `nativeNetcode.ts` service.

**Rationale**: The existing `netcode.ts` file already handles k3h4 orchestration; adding new routes there keeps the surface discoverable. A separate training service keeps concerns narrow (training artifact I/O vs live clustering compute).

**Endpoint defaults**: `mode` query param defaults to `power` (aligns with FR-012 and spec default-mode decision).

---

## React Visualization Library

**Decision**: Use the chart library already present in `src/typescript/react`. No new charting dependency is introduced. Log-log scaling is achieved with log-scaled axes on the existing library. SVG-based Voronoi overlay is drawn with a lightweight path builder, not a separate Voronoi library.

**Rationale**: Introducing a new charting or geometry library would increase bundle size, create a new dependency risk, and require a separate approval step. The existing chart library supports log axes and custom SVG overlays sufficient for the four visualization components.

**Presentation-only constraint**: All data originates from API calls. No clustering math, Voronoi scoring, or artifact serialization may run client-side in production builds (FR-018).

---

## Polling vs Subscription for Confidence Dashboard

**Decision**: Use poll-based refresh (5-second interval, with exponential backoff on consecutive empty epochs) rather than WebSocket or SSE. Stop polling once the session transitions to a completed state.

**Rationale**: The existing API infrastructure does not have a push channel. Adding SSE or WebSocket to satisfy a single dashboard component would introduce infrastructure debt disproportionate to the use-case. 5-second polling satisfies NFR-003.

**Alternatives considered**:
- SSE (deferred: requires new server infrastructure; acceptable future upgrade path).
- WebSocket (deferred: same reason).
