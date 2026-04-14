# Banana Flavor (High Level Language) ↔ C Native Interop Example

> Previously CInteropSharp (CoI Refactor)

Modernization baseline for a legacy C system without rewriting core logic.

Using a banana as the single source of truth.

https://learn.microsoft.com/en-us/azure/architecture/patterns/strangler-fig

## Architecture

```mermaid
flowchart LR
  Legacy[Legacy C logic] --> Wrapper[C wrapper ABI boundary]
  Wrapper --> Api[C# .NET 8 Web API via P/Invoke]
  Wrapper --> Node[Optional Node/Electron ffi-napi bridge]
  Api --> Client[REST clients]
  Api --> ManagedDb[Managed PostgreSQL via Npgsql]
  Wrapper --> NativeDb[Legacy native PostgreSQL path]
  HookDefault[Native test hooks default no-op] -.linked in runtime build.-> Wrapper
  HookEnv[Native test hooks env forced-paths] -.linked in native test build.-> Wrapper
```

## Repository Structure

```text
src/
  native/
    legacy/
    wrapper/
    testing/
  api/
    Controllers/
    DataAccess/
    Pipeline/
    Services/
    NativeInterop/
    Middleware/
  electron/
tests/
  native/
  unit/
  integration/
scripts/
docs/
```

## New Developer Setup

- Start with `docs/developer-onboarding.md` for a guided first pass through the codebase.
- Includes a visual request-flow diagram for the controller → pipeline → native interop path.

## What Is an ABI?

An ABI (Application Binary Interface) is the low-level contract that lets separately compiled code talk to each other at runtime.

It defines details like:

- Exported function names and calling conventions.
- Primitive type sizes and struct memory layout.
- Ownership rules for allocated memory across module boundaries.

In this repository, the ABI is the stable C wrapper boundary in `src/native/wrapper` that both .NET (P/Invoke) and optional Node/Electron consumers call into. Keeping this ABI stable lets us modernize managed orchestration without rewriting or tightly coupling to the legacy C internals.

## Interop Boundary

- Legacy logic remains isolated in `src/native/legacy`.
- Wrapper in `src/native/wrapper` is the only exported native API.
- Wrapper exports primitive signatures and fixed-layout structs.
- Native memory created by wrapper must be released with `cinterop_free`.
- Managed code does not call legacy functions directly.

## Pipeline Pattern (API Orchestration)

The `/banana` flow now runs through a composable pipeline inside the API service layer:

```mermaid
flowchart TD
  A[BananaController] --> B[BananaService]
  B --> C[PipelineExecutor PipelineContext]
  C --> D[ValidationStep]
  D --> E[DatabaseAccessStep]
  E --> F[NativeCalculationStep]
  F --> G[PostProcessingStep]
  G --> H[AuditStep]

  E --> I[IDataAccessPipelineClient]
  I --> J[LegacyNativeDbDataAccessClient]
  I --> K[ManagedNpgsqlDataAccessClient]
  J --> L[Native wrapper DB export]
  K --> M[Npgsql]
```

Current step responsibilities:

- `ValidationStep`: validates request inputs and blocks invalid execution paths early.
- `DatabaseAccessStep`: executes one atomic data-access operation and stores raw DB result + DB metadata in the pipeline context.
- `NativeCalculationStep`: executes native banana calculation through the stable wrapper ABI.
- `PostProcessingStep`: applies managed enrichment that does not alter native calculation semantics.
- `AuditStep`: emits structured pipeline telemetry for diagnostics and observability.

Why this was introduced:

- Keeps the legacy/native boundary intact while modernizing orchestration.
- Decouples validation, data access, native execution, enrichment, and audit concerns.
- Enables additive extension by registering new pipeline steps without changing executor logic.
- Preserves backward compatibility because wrapper ABI and native calculation contract remain stable.

DB mode options:

- Legacy native mode (default): `DatabaseAccessStep` resolves `LegacyNativeDbDataAccessClient`, which calls the native DB wrapper path.
- Managed mode (optional): `DatabaseAccessStep` resolves `ManagedNpgsqlDataAccessClient`, which executes managed PostgreSQL queries via Npgsql.

Extension hooks:

- Implement `IPipelineStep<PipelineContext>` in `src/api/Pipeline/Steps`.
- Set `Order` to control deterministic execution.
- Register the step in DI (`Program.cs`) as `IPipelineStep<PipelineContext>`.
- Keep DB concerns inside `DatabaseAccessStep` and its data-access clients to preserve stage boundaries.

## Developer Interop Middleware / DI Walkthrough

If you are new to middleware/pipeline patterns, read these files in order:

1. `src/api/Controllers/BananaController.cs`
  - Accepts HTTP input and calls the service.
2. `src/api/Services/BananaService.cs`
  - Builds `PipelineContext` and runs `PipelineExecutor<PipelineContext>`.
3. `src/api/Pipeline/PipelineExecutor.cs`
  - Composes steps into one chain and executes in deterministic order.
4. `src/api/Program.cs`
  - Registers pipeline steps and selects DB access implementation (`LegacyNative` vs `ManagedNpgsql`) via `DbAccessOptions`.
5. `src/api/Pipeline/Steps/*`
  - `ValidationStep` → validates input
  - `DatabaseAccessStep` → executes atomic DB stage
  - `NativeCalculationStep` → calls native interop
  - `PostProcessingStep` → enrichment logic
  - `AuditStep` → final structured logging
6. `src/api/DataAccess/*`
  - Defines the DB stage contract and concrete clients (`LegacyNativeDbDataAccessClient`, `ManagedNpgsqlDataAccessClient`).
7. `src/api/NativeInterop/*`
  - Contains the P/Invoke boundary and native error mapping.
8. `src/native/wrapper/*` and `src/native/legacy/*`
  - Wrapper owns exported ABI, validation, and status mapping.
  - Legacy layer owns calculation and SQL behavior.
9. `src/native/testing/native_test_hooks_default.c` and `tests/native/native_test_hooks_env.c`
  - Shows how test-only native branch forcing is isolated from production runtime code.

## Build Native (Windows)

```bat
scripts\build-native.bat
```

Expected output directory:

- `build/native/bin/Release/cinterop_native.dll`

## Build Everything (bash)

```bash
./scripts/build-all.sh
```

## Docker Compose Orchestration

Compose profiles orchestrate runtime and test flows:

- `runtime`: builds native artifact, then starts API
- `tests`: runs unit and integration test containers
- `electron`: runs optional ffi-napi example against native artifact

Run API stack:

```bash
docker compose --profile runtime up --build api
```

Run test flows:

```bash
docker compose --profile tests up --build --abort-on-container-exit --exit-code-from test-all test-all
```

This test flow now includes:

- PostgreSQL service (`postgres:16`) used by the native C DB path.
- Native C tests via CTest.
- .NET unit + integration tests with coverage collectors.
- Coverage artifacts copied to `./artifacts` on the host.

Run optional Electron flow:

```bash
docker compose --profile electron up --build --abort-on-container-exit --exit-code-from electron-example electron-example
```

Convenience wrappers:

```bash
./scripts/compose-runtime.sh
./scripts/compose-tests.sh
./scripts/compose-electron.sh
```

## CI (GitHub Actions)

On every push, CI runs the same compose profiles used locally:

- `tests` profile via `test-all`
- `runtime` profile with `/banana` endpoint smoke test
- `electron` profile optional bridge smoke test

Workflow file:

- `.github/workflows/compose-ci.yml`

CI notes:

- Native C coverage job provisions PostgreSQL and runs native tests with `CINTEROP_PG_CONNECTION` configured.
- .NET coverage job also provisions PostgreSQL and runs with explicit native DB connection settings.
- CI prints only DB target host/port (no credentials) before test execution for visibility.

## Native Test Hook Isolation

Test-only native behavior is isolated from legacy runtime files:

- Production native builds use `src/native/testing/native_test_hooks_default.c` (no-op hooks).
- Native test builds use `tests/native/native_test_hooks_env.c` (env-driven forced paths).
- Legacy/runtime files (`src/native/legacy/*`, `src/native/wrapper/*`) now call hook functions instead of parsing test env vars directly.

## Run API

```bash
export CINTEROP_NATIVE_PATH="$(pwd)/build/native/bin/Release"
./scripts/run-api.sh
```

Open Swagger:

- `http://localhost:5000/swagger` or `https://localhost:5001/swagger`

Example endpoint:

- `GET /banana?purchases=10&multiplier=2`

## Optional Node/Electron Bridge

For `ffi-napi` compatibility, use Node 14/16 for local non-container runs.

```bash
cd src/electron
npm install
CINTEROP_NATIVE_PATH="$(pwd)/../../build/native/bin/Release" npm run example
```

## Tests

```bash
dotnet test tests/unit/CInteropSharp.UnitTests.csproj
dotnet test tests/integration/CInteropSharp.IntegrationTests.csproj
```

Integration tests call the real API and native wrapper. Build native artifacts first.

## Coverage

Run coverage locally with the built-in `XPlat Code Coverage` collector:

```bash
dotnet test tests/unit/CInteropSharp.UnitTests.csproj -c Release --collect:"XPlat Code Coverage" --results-directory .artifacts/test-results/unit
dotnet test tests/integration/CInteropSharp.IntegrationTests.csproj -c Release --collect:"XPlat Code Coverage" --results-directory .artifacts/test-results/integration
```

CI/CD (`.github/workflows/compose-ci.yml`) now runs unit and integration tests with coverage, then publishes:

- `test-results` artifact (`.trx` + raw coverage files)
- `coverage-report` artifact (Cobertura + HTML + text summary)
