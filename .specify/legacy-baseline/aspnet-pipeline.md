# Legacy Baseline — ASP.NET Pipeline & Native Interop

**Paths**: `src/c-sharp/asp.net/`, project file `Banana.Api.csproj`

## What exists

- **Controllers** (`Controllers/`): `BananaController`, `BatchController`, `NotBananaController`, `RipenessController`.
- **Services** (`Services/`): orchestrate pipeline execution and domain calls.
- **Pipeline** (`Pipeline/`): `IPipelineStep`, `PipelineContext`, `PipelineExecutor`, ordered `Steps/` for request shaping.
- **Native interop** (`NativeInterop/`): `INativeBananaClient` + `NativeBananaClient`, `NativeMethods` (P/Invoke), `NativeLibraryResolver`, `NativeStatusCode`, plus DTOs (`BananaBatchRecord`, `BananaBreakdownNative`, `BananaResult`, ML/ripeness records).
- **Middleware** (`Middleware/`): cross-cutting handlers (logging/error translation).
- **DataAccess** (`DataAccess/`): client selection layer.
- **Models** (`Models/`): API DTOs.
- `Program.cs` wires DI + middleware + controllers.

## Hard contracts

- `INativeBananaClient` is the single seam for native calls — every test double in `tests/unit` and `tests/integration` implements it.
- `NativeStatusCode` numeric values mirror C wrapper exactly.
- `BANANA_NATIVE_PATH` env var controls native lib resolution; no machine-specific hardcoding.
- HTTP routes consumed by React/Electron via `VITE_BANANA_API_BASE_URL` contract.
- `coverage-denominator.json` checked in to stabilize coverage thresholds.

## What works

- Pipeline pattern cleanly orders pre/native/post steps.
- DI wiring is straightforward.
- Status-code → HTTP translation is consistent.

## Vibe drift / pain points

- Multiple controllers duplicate request-shape concerns that the pipeline already owns.
- `DataAccess` overlaps with `NativeInterop` responsibilities — unclear which owns DB-vs-native dispatch.
- DTO sprawl: `*Native` shadow types per record introduce manual mapping that drifts.
- Some pipeline steps depend on others implicitly via `PipelineContext` keys (string-typed bag).
- Adding a method to `INativeBananaClient` requires updating ~10+ test fakes (CS0535 cascade).
- Middleware order is correct but undocumented.

## Cross-domain dependencies

- Consumes: `006-native-core` ABI.
- Consumed by: `009-react-shared-ui` (HTTP), `010-electron-desktop` (HTTP via shared API client), `011-react-native-mobile` (HTTP).
- Tested by: `014-test-coverage` (unit + integration).

## v1 entry points to preserve in v2

- `INativeBananaClient` interface seam (or its v2 equivalent)
- Public HTTP routes / response shapes (frontends depend on them)
- `coverage-denominator.json` as the coverage anchor
