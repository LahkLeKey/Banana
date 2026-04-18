# Banana Architecture Map

## Domains

- Native C: `src/native`, `tests/native`, `CMakeLists.txt`
- ASP.NET: `src/c-sharp/asp.net`, `tests/unit`, `tests/integration`
- Frontend/Electron: `src/typescript/react`, `src/typescript/electron`
- Delivery/runtime: `docker`, `scripts`, `docker-compose.yml`, `.github/workflows`

## Core Flow

- HTTP enters `Controllers/BananaController.cs`
- Orchestration lives in `Services/BananaService.cs`
- Ordered work runs through `Pipeline/PipelineExecutor.cs` and `Pipeline/Steps/*`
- Native interop crosses through `NativeInterop/NativeBananaClient.cs` and `NativeMethods.cs`
- Native implementation lives in `src/native/core`, `src/native/dal`, and `src/native/wrapper`

## Helper Routing

- `native-core-agent`: `src/native/core` and focused native tests
- `native-dal-agent`: `src/native/dal` and PostgreSQL-gated native behavior
- `native-wrapper-agent`: `src/native/wrapper` and downstream ABI coordination
- `api-pipeline-agent`: controllers, services, middleware, pipeline, and `Program.cs`
- `api-interop-agent`: `NativeInterop`, `DataAccess`, and managed/native contract mapping
- `react-ui-agent`: `src/typescript/react/src`
- `electron-agent`: `src/typescript/electron`
- `compose-runtime-agent`: `docker-compose.yml`, `docker`, and local runtime scripts
- `workflow-agent`: `.github/workflows` and coverage automation
- `test-triage-agent`: failing tests, harness issues, and validation ownership mapping

## Key Contracts

- `BANANA_PG_CONNECTION` for PostgreSQL-backed native and integration flows
- `BANANA_NATIVE_PATH` for managed/native runtime loading
- `VITE_BANANA_API_BASE_URL` for React runtime API calls

## Existing Entry Points

- Native build task: `Build Native Library`
- API build task: `Build Banana API`
- Compose stack task: `Start Compose Apps`
- Aggregate tests: `scripts/run-tests-with-coverage.sh`
- Native coverage gate: `scripts/run-native-c-tests-with-coverage.sh`
- CI workflow: `.github/workflows/compose-ci.yml`
