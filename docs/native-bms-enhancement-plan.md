# Native BMS Enhancement Plan

## Scope

This plan covers the Banana native layer under `src/native` and the immediate ASP.NET interop contract surface required to scaffold more complex Banana Management System (BMS) behavior.

## Native Architecture Map

### Domains Touched

- Native C core and wrapper: `src/native/core`, `src/native/wrapper`
- Native DAL and DB plumbing: `src/native/dal`
- Native tests: `tests/native`
- Managed interop boundary: `src/c-sharp/asp.net/NativeInterop`

### Runtime and Contract Boundaries

- Managed runtime loading contract: `BANANA_NATIVE_PATH`
- PostgreSQL-backed native/integration flows: `BANANA_PG_CONNECTION`
- React API contract (downstream consumer): `VITE_BANANA_API_BASE_URL`

### Core Request Flow

1. API controller receives request.
2. Service executes ordered pipeline.
3. Native interop calls wrapper exports.
4. Wrapper maps C interop structs and statuses to native core.
5. Native core performs calculation/lifecycle logic.
6. DAL is consulted where configured and available.

## Current Baseline (Verified)

- Native build path works through `Build Native Library`.
- API build path works through `Build Banana API`.
- Native tests pass through CTest against `build/native`.
- Managed unit tests pass (`tests/unit/Banana.UnitTests.csproj`).
- Managed integration tests pass (`tests/integration/Banana.IntegrationTests.csproj`).
- Local native build falls back to non-PostgreSQL mode when PostgreSQL client libraries are missing.

## Enhancement Roadmap

### Phase 1: Native Domain Hardening

- Keep wrapper ABI additive-only and backward compatible.
- Continue centralizing validation and execution context in core.
- Expand batch lifecycle states beyond packed to shipped, arrived, and sold transitions.
- Add deterministic status transition validation in native core.

### Phase 2: Batch Persistence and Recovery

- Replace process-memory-only batch registry with persistence-backed storage path.
- Define serialization format for startup recovery and crash resilience.
- Add explicit native errors for persistence faults and conflict cases.

### Phase 3: Plant and Bunch Layer

- Add native entities and wrappers for plants and bunches.
- Model harvest operations that generate traceable batch records.
- Preserve additive wrapper exports so existing clients remain stable.

### Phase 4: Supply Chain and Risk Signals

- Add shipment nodes and cold-chain telemetry normalization.
- Expand ripeness prediction inputs with transport-stage telemetry.
- Introduce confidence metadata and uncertainty bounds in native outputs.

### Phase 5: Observability and Diagnostics

- Standardize structured diagnostic metadata from native wrapper outputs.
- Expand status-to-error mapping consistency across wrapper and managed interop.
- Add native trace hooks for pipeline stage profiling in non-production builds.

## Validation Matrix

### Native-only Changes

- Build: `Build Native Library`
- Test: `ctest --test-dir build/native -C Release --output-on-failure`
- Optional coverage gate: `bash scripts/run-native-c-tests-with-coverage.sh`

### ASP.NET Interop or Pipeline Changes

- Build: `Build Banana API`
- Unit tests: `dotnet test tests/unit/Banana.UnitTests.csproj -c Debug`
- Integration tests: `dotnet test tests/integration/Banana.IntegrationTests.csproj -c Debug`

### Cross-layer Runtime Changes

- App stack start: `bash scripts/compose-apps.sh`
- App stack stop: `bash scripts/compose-apps-down.sh`
- Aggregate coverage: `bash scripts/run-tests-with-coverage.sh`

## Release-Readiness Exit Criteria

- Domain boundaries preserved (native changes remain isolated unless contract updates are intentional).
- Wrapper API changes are additive and validated by native plus managed tests.
- Runtime contracts are explicit and unchanged unless documented.
- Nearest docs are updated when behavior or validation flow changes.

## Risks and Follow-Ups

- PostgreSQL-enabled native path was not exercised in this local environment due missing client libraries.
- CMake Tools extension could not configure this workspace; native tests were validated via direct CTest fallback.
- If persistence is added to native batch storage, integration coverage should include restart and recovery scenarios.
