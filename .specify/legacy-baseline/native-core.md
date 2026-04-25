# Legacy Baseline — Native C Core / Wrapper / DAL

**Paths**: `src/native/core/`, `src/native/wrapper/`, `src/native/testing/`, `CMakeLists.txt`, `tests/native/`

## What exists

- **Core domain** (`src/native/core/domain/`): ~25 paired `.c/.h` modules covering banana lifecycle, agriculture, cultivation, processing, ripeness, retail, supply chain, logistics, inventory, ml_models, agent_pulse_model, not_banana classifier, plus `generated/` and `ml/` subtrees.
- **DAL** (`src/native/core/dal/`): `banana_db.[ch]` plus `domain/` and `internal/`. PostgreSQL-gated by `BANANA_PG_CONNECTION`.
- **Wrapper** (`src/native/wrapper/`): `banana_wrapper.[ch]` exports the ABI, plus DAL and domain shims. Status codes drive interop.
- **Build**: CMake from repo root → `build/native/` (Visual Studio 17 2022 / x64 Release on Windows; gcc on Linux containers).

## Hard contracts

- ABI export surface in `banana_wrapper.h` is consumed by `src/c-sharp/asp.net/NativeInterop/NativeMethods.cs`.
- `NativeStatusCode` enum mirrors C status codes 1:1.
- `BANANA_PG_CONNECTION` env var must be set whenever PostgreSQL paths are exercised.
- `BANANA_NATIVE_PATH` resolves built artifacts in non-default layouts.
- Test loading uses runtime path resolution captured in `/memories/repo/native-test-loading.md`.

## What works

- Deterministic domain functions (lifecycle, ripeness staging) are stable and have unit-test coverage in `tests/native/`.
- Wrapper status-code translation is consistent across managed/unmanaged boundaries.
- `not_banana` classifier path has end-to-end coverage including PostgreSQL gate.

## Vibe drift / pain points

- Domain modules proliferated ahead of demand: agriculture, cultivation, logistics, retail, supply_chain, agent_pulse_model, integration, profile, events, services, repositories, read_models, applications — many appear speculative with thin test coverage.
- `generated/` mixes hand-edited and generated code; provenance is unclear.
- `ml/` and `ml_models` overlap; responsibilities should be one module.
- Mixed memory-ownership patterns across DAL/domain — some return owned buffers, some take caller-owned.
- Error reporting fragmented: status enum + `errno`-style fields + log strings.
- Header dependencies fan out widely; recompilation cost is high.

## Cross-domain dependencies

- Consumed by: `007-aspnet-pipeline` (P/Invoke), `014-test-coverage` (native tests).
- Depends on: PostgreSQL (DAL only), CMake toolchain.

## v1 entry points to preserve in v2

- CMake target name `banana_wrapper` (DLL/SO output)
- Public symbols in `banana_wrapper.h` consumed by managed interop
- Status code numeric values (breaking these breaks every consumer)
