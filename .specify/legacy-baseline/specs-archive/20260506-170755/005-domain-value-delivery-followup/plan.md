# Implementation Plan: Domain Value Delivery Follow-Up (Phase 2)

**Branch**: `005-domain-value-delivery-followup` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/005-domain-value-delivery-followup/spec.md`

## Summary

Close the remaining production contract gaps from 004 by (1) introducing a dedicated native-runtime-unavailable API status contract, (2) moving ML and not-banana payload shaping out of controllers into pipeline/service mappers, (3) adding Fastify ripeness route parity with schema-backed tests, (4) exposing harvest/truck HTTP endpoints in ASP.NET for existing interop methods, and (5) codifying the integration-equivalent validation lane so contract checks are auditable and reproducible.

## Technical Context

**Language/Version**: C# (.NET 8 ASP.NET Core), TypeScript (Node/Fastify), C (ABI unchanged)
**Primary Dependencies**: `System.Text.Json`, ASP.NET DI + pipeline components, Fastify route schema validation, existing native interop P/Invoke bridge
**Storage**: N/A for new persistence; existing native/PostgreSQL behavior unchanged in this slice
**Testing**: `dotnet test tests/unit`, `dotnet test tests/e2e`, `bun run --cwd src/typescript/api test`, contract-level route tests for TS API ripeness
**Target Platform**: Windows dev host, Linux container/runtime parity via existing compose contracts
**Project Type**: Monorepo web-service + API gateway + native interop
**Performance Goals**: No new performance SLOs; preserve current endpoint latency envelopes while hardening correctness
**Constraints**: Preserve controller -> service -> pipeline -> native interop layering; avoid ABI break in native wrapper; keep `BANANA_NATIVE_PATH` remediation explicit
**Scale/Scope**: ASP.NET controller/pipeline/interop + TS API route/test + docs/spec artifacts in one feature slice

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Domain Core First | PASS | No native ML algorithm changes; only managed contract shaping and endpoint exposure |
| II. Layered Interop Contract | PASS | Mapping is shifted into service/pipeline abstractions, not duplicated in controllers |
| III. Spec First Delivery | PASS | 005 spec exists before planning; plan artifacts generated before tasks |
| IV. Verifiable Quality Gates | PASS | Unit + e2e + TS API route tests included as explicit validation gates |
| V. Documentation and Wiki Parity | PASS | Integration-lane contract and endpoint surfaces captured in plan/contracts/quickstart artifacts |

**Post-design check (Phase 1)**: PASS. No constitution violations introduced by selected design or artifact outputs.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/005-domain-value-delivery-followup/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── api-contracts.md
└── tasks.md
```

### Source Code (repository root)

```text
src/c-sharp/asp.net/
├── Controllers/
│   ├── BananaMlController.cs
│   ├── NotBananaController.cs
│   ├── HarvestController.cs              # new
│   └── TruckController.cs                # new
├── NativeInterop/
│   ├── NativeBananaClient.cs
│   └── NativeStatusCode.cs
└── Pipeline/
    ├── StatusMapping.cs
    ├── Results/                          # new typed result records
    └── Mapping/                          # new mapping services/pipeline steps

src/typescript/api/src/
├── index.ts
└── routes/
    ├── ripeness.ts                       # new
    └── ripeness.test.ts                  # new

tests/
├── unit/
│   ├── BananaMlControllerTests.cs
│   ├── NotBananaControllerTests.cs
│   ├── HarvestControllerTests.cs         # new
│   └── TruckControllerTests.cs           # new
└── e2e/
    ├── Contracts/
    │   └── ApiContractLaneTests.cs       # new or expanded
    └── E2eRunnerContractTests.cs
```

**Structure Decision**: Multi-domain monorepo slice. Keep native ABI stable and implement delivery changes in ASP.NET + TS API + test lanes, with controller responsibilities narrowed to HTTP orchestration only.

## Complexity Tracking

No constitution violations requiring justification.
