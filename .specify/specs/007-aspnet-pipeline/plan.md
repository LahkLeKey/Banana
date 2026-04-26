# Implementation Plan: ASP.NET Pipeline (v2)

**Branch**: `007-aspnet-pipeline` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: C# / .NET 8.  
**Primary Dependencies**: ASP.NET Core, P/Invoke to `006-native-core` wrapper.  
**Storage**: PostgreSQL (gated, primarily via native DAL).  
**Testing**: xUnit unit + integration suites under `tests/{unit,integration}`.  
**Target Platform**: Windows + Linux containers.  
**Project Type**: Web service.  
**Performance Goals**: No regression on request p95.  
**Constraints**: Stable HTTP surface; preserve `BANANA_NATIVE_PATH` and `coverage-denominator.json`.

## Constitution Check

- Hard contracts preserved: PASS.
- Helper-agent decomposition: PASS (`api-pipeline-agent`, `api-interop-agent`).

## Project Structure

```text
src/c-sharp/asp.net/
├── Controllers/        # thin: route + bind + delegate to pipeline
├── Pipeline/
│   ├── PipelineContext.cs   # typed
│   ├── PipelineExecutor.cs
│   └── Steps/
├── NativeInterop/
│   ├── INativeBananaClient.cs   # successor seam
│   ├── NativeBananaClient.cs
│   ├── NativeMethods.cs
│   ├── NativeLibraryResolver.cs
│   └── NativeStatusCode.cs
├── Services/
├── Middleware/
├── Models/             # one DTO per record
├── DataAccess/         # narrow client-selection only (or fold into NativeInterop)
├── Program.cs
└── coverage-denominator.json   # stays
```

## Phasing

- **Phase 0**: Inventory routes, response shapes, and current `*Native` DTOs.
- **Phase 1**: Introduce typed `PipelineContext`; migrate steps incrementally with adapters.
- **Phase 2**: Collapse `*Native` DTOs; mapping via source generators or single mapper.
- **Phase 3**: Reduce interop fake fan-out (default abstract base or generated fake).
- **Phase 4**: Decide `DataAccess` fate; fold into NativeInterop or formalize role.
- **Phase 5**: Cutover to v2 native (`006`); legacy v1 native paths removed.
