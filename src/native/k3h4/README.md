# Native K3H4 Layer

This folder provides the top-level native `k3h4` orchestration layer used by API-facing exports.

- `k3h4_native_orchestrator.h/.c` defines the native bridge surface for k3h4 model build calls.
- Scaffold exports in `scaffold/native_entry.c` should call this layer instead of directly calling runtime netcode ABI builders.

This keeps the API/native orchestration boundary explicit under `src/native/k3h4`.
