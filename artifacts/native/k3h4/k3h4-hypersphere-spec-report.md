# K3H4 Hypersphere Architecture Report

## Scope
This report ties together the k3h4 model intent across native, API, and frontend layers, with focus on:
- hypersphere-first orchestration
- endianness-safe contract transport
- harmonic/spectral interpretation

## Executive Summary
- Native now has a top-level k3h4 layer under `src/native/k3h4` used by API-facing scaffold exports.
- Runtime netcode orchestration uses k3h4 pipeline paths that produce deterministic hypersphere outputs and contract metadata.
- API and frontend surfaces were renamed from hypersphere-kmeans terminology to k3h4 for contract alignment.
- Endianness is handled as serialization/transport correctness, not as geometry semantics.
- Harmonic interpretation is represented by spectral proxy outputs derived from hypersphere radius behavior.

## Architecture Flow
1. Native API export (`src/native/scaffold/native_entry.c`) receives request input.
2. Export calls top-level k3h4 bridge (`src/native/k3h4/k3h4_native_orchestrator.c`).
3. Bridge delegates into runtime netcode ABI/orchestration (`src/native/engine/runtime/abi/netcode/netcode_abi.c`).
4. Runtime k3h4 orchestration computes vector + hypersphere + observability and envelope metadata.
5. API consumes native outputs and exposes k3h4 contract payloads.
6. Frontend renders orchestrated values only (no production recompute fallback).

## Endianness Interpretation
### Mathematical intent
- K3h4 geometry is defined over interpreted numeric values.
- Endianness does not alter geometry if bytes are decoded with the correct byte order contract.

### Engineering contract
- Envelope/version/CRC validation protects interpretation correctness.
- Mixed-endianness handling is explicit via decode-path metadata and validation behavior.
- Invalid version, malformed payload, and CRC mismatch paths fail deterministically.

## Harmonic Interpretation
- Spectral proxy fields provide harmonic interpretation of cluster/hypersphere state.
- Radius-derived spectral values are deterministic and ordered by stable cluster identity.
- This supports a resonance-style interpretation without coupling harmonic semantics to byte order.

## DDD and SOLID Alignment
### Domain boundaries
- Native domain: deterministic k3h4 compute and contract envelope formation.
- API application boundary: orchestration and transport mapping to HTTP contract.
- Frontend presentation boundary: render-only consumption of authoritative outputs.

### SOLID mapping
- Single Responsibility: k3h4 native layer isolates API-facing orchestration entrypoints.
- Open/Closed: runtime k3h4 internals can evolve while preserving stable bridge contracts.
- Liskov Substitution: deterministic contract behavior is preserved across callers.
- Interface Segregation: scaffold uses focused build functions rather than cross-domain dependencies.
- Dependency Inversion: higher layers depend on k3h4 abstraction, not raw runtime internals.

## Validation Signals
- Native build and focused k3h4/netcode test suites pass for determinism, edge cases, orchestrator parity, and ABI envelope checks.
- Contract naming parity was migrated to k3h4 across native/API/frontend/spec artifacts.

## Operational Guidance
- New native model work should enter via `src/native/k3h4` first.
- API-facing scaffold exports should continue using the k3h4 bridge.
- ABI envelope and endianness tests remain mandatory gates for transport integrity.
- Frontend should treat k3h4 values as authoritative render inputs.

## Note on Term Naming
- This repository uses `k3h4` as the canonical model identifier.
- If `k34h` is preferred as an alias in external docs, it should be documented as an alias while keeping code symbols stable under `k3h4`.
