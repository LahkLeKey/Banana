# Implementation Plan: ASP.NET Pipeline (v2)

**Branch**: `007-aspnet-pipeline` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: C# / .NET 8.  
**Primary Dependencies**: ASP.NET Core, P/Invoke to `006-native-core` wrapper, banana/not-banana model artifact metadata contracts, chatbot interoperability contracts with `008`.  
**Storage**: PostgreSQL (gated, primarily via native DAL).  
**Testing**: xUnit unit + integration suites under `tests/{unit,integration}`, plus cross-slice parity checks for banana/not-banana classifier semantics.  
**Target Platform**: Windows + Linux containers.  
**Project Type**: Web service.  
**Performance Goals**: No regression on request p95.  
**Constraints**: Stable HTTP surface; preserve `BANANA_NATIVE_PATH` and `coverage-denominator.json`; publish threshold/model-source provenance compatible with `008` score/chat semantics.

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
- **Phase 6**: Align managed banana/not-banana model metadata + threshold provenance contract with `008`.
- **Phase 7**: Define chatbot interoperability/fallback semantics for banana-vs-not-banana classification across `007` and `008`.
- **Phase 8**: Add drift/parity CI gates for corpus, training scripts, native runtime outputs, and managed API contracts.
