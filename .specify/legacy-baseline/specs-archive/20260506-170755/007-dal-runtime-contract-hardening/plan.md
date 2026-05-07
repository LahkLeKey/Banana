# Implementation Plan: DAL Runtime Contract Hardening

**Branch**: `feature/006-c-upstream-value-spike` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/007-dal-runtime-contract-hardening/spec.md`

## Summary

Harden DAL runtime contract behavior so profile-query paths return explicit, deterministic non-success outcomes for unconfigured, dependency-unavailable, and query-failure states while preserving successful path compatibility and cross-layer status mapping consistency.

## Technical Context

**Language/Version**: C11 native code, C#/.NET 8 ASP.NET Core, Markdown planning artifacts
**Primary Dependencies**: Native DAL (`src/native/core/dal/banana_dal.c`), wrapper status contract (`src/native/wrapper/banana_wrapper.h`), ASP.NET status mapping (`src/c-sharp/asp.net/Pipeline/StatusMapping.cs`)
**Storage**: PostgreSQL (runtime-gated via `BANANA_PG_CONNECTION`)
**Testing**: Native C tests (`tests/native`), .NET unit tests (`tests/unit`), contract-lane tests (`tests/e2e/Contracts`)
**Target Platform**: Windows development environment with native library integration and .NET runtime
**Project Type**: Monorepo native + managed API contract hardening slice
**Performance Goals**: No measurable regression in existing DAL success-path latency while improving failure determinism
**Constraints**: Preserve controller -> service -> pipeline -> native interop contract, keep `BANANA_PG_CONNECTION` and `BANANA_NATIVE_PATH` semantics explicit, avoid unrelated status taxonomy expansion
**Scale/Scope**: One bounded DAL hardening story affecting native DAL outcomes and managed status mapping

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Domain Core First | PASS | DAL behavior changes are anchored in native C domain ownership |
| II. Layered Interop Contract | PASS | Plan preserves native-to-managed contract synchronization and pipeline mapping |
| III. Spec First Delivery | PASS | 007 spec created before plan and tasks |
| IV. Verifiable Quality Gates | PASS | Native, unit, and contract lane validation are explicitly required |
| V. Documentation and Wiki Parity | PASS | This slice is documented under `.specify/specs/007-*`; no workflow/wiki contract change introduced |

**Post-design check (Phase 1)**: PASS. No constitution violations introduced by design artifacts.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/007-dal-runtime-contract-hardening/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── dal-runtime-contracts.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/core/
├── dal/
│   └── banana_dal.c
├── banana_status.c
└── banana_status.h

src/native/wrapper/
└── banana_wrapper.h

src/c-sharp/asp.net/
├── Pipeline/
│   └── StatusMapping.cs
└── NativeInterop/
    └── NativeStatusCode.cs

tests/
├── native/
├── unit/
└── e2e/Contracts/
```

**Structure Decision**: Keep implementation scoped to DAL runtime outcome semantics and mapping surfaces under existing native and ASP.NET layers; validate with smallest sufficient native/unit/contract lanes.

## Complexity Tracking

No constitution violations requiring justification.
