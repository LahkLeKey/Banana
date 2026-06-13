# Native K3H4 Layer

This folder provides the top-level native `k3h4` orchestration layer used by API-facing exports.

- `k3h4_native_orchestrator.h/.c` defines the native bridge surface for k3h4 model build calls.
- Scaffold exports in `scaffold/native_entry.c` should call this layer instead of directly calling runtime netcode ABI builders.
- The k3h4 contract is metrics-centric: vector and cluster outputs flow through the k3h4 orchestration path that emits metrics metrics and ABI envelope metadata.
- Endianness is treated as a representation-layer concern (decode/encode correctness), not a geometric-layer concern (cluster geometry is invariant after correct decode).
- Harmonic interpretation is represented through spectral proxy outputs that are derived deterministically from the same metrics radius model.

This keeps the API/native orchestration boundary explicit under `src/native/k3h4`.
