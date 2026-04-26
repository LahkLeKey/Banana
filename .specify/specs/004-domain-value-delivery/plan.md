# Implementation Plan: Domain Value Delivery — End-to-End Ripeness MVP

**Branch**: `004-domain-value-delivery` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/004-domain-value-delivery/spec.md`

## Summary

Fix the broken classify→ripeness→display pipeline across all four domain layers so a user can submit a banana description and see a typed `RipenessLabel` in the React UI. This requires: (1) upgrading the native C ripeness function to use the ML regression model instead of keyword counting, (2) deserializing the double-encoded JSON string in the ASP.NET controller, (3) fixing the shared UI `Ripeness` type contract, (4) wiring `predictRipeness()` and `<RipenessLabel>` in React, and (5) implementing the ASP.NET pipeline step infrastructure.

## Technical Context

**Language/Version**: C (C99), C# (.NET 8 / ASP.NET Core), TypeScript (strict, Bun runtime), React 18
**Primary Dependencies**: `banana_ml_models.h` (native ML domain), `System.Text.Json` (ASP.NET), `@banana/ui` (shared component library), React + Tailwind (UI)
**Storage**: N/A for this slice (no Prisma / PostgreSQL changes)
**Testing**: `ctest` (native), `dotnet test` (ASP.NET unit + integration), `bun test` (TypeScript), `tsc --noEmit` (type correctness)
**Target Platform**: Linux container (compose `api` + `react` services)
**Project Type**: Monorepo — touches native library, web-service, frontend app, shared component library
**Performance Goals**: No new performance requirements — correctness fix only
**Constraints**: Must not break existing `ctest` or `dotnet test` suites; type change in `@banana/ui` must be non-breaking for Electron and React Native (no current usages of old Ripeness type in those packages)
**Scale/Scope**: 5 files changed across 4 layers; 2 new pipeline interface files; 1 new result record

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Domain Core First | ✅ PASS | Native C ripeness logic upgraded to call ML layer; upper layers only orchestrate |
| II. Layered Interop Contract | ✅ PASS | `controller → pipeline → native interop` flow preserved; deserialization stays in controller for this slice, not pushed into interop layer |
| III. Spec First Delivery | ✅ PASS | spec.md written and reviewed before implementation; plan generated before tasks |
| IV. Verifiable Quality Gates | ✅ PASS | `ctest`, `dotnet test`, `bun test`, `tsc --noEmit` all identified as required gates |
| V. Documentation and Wiki Parity | ✅ PASS | No workflow/automation contracts change in this slice; wiki sync not required |

**Constitution Check post-design (Phase 1)**: All gates still pass. Research confirmed no ABI changes required, type fix has no downstream breakage risk, pipeline step interface is additive.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/004-domain-value-delivery/
├── plan.md                          # This file
├── research.md                      # Phase 0 output (complete)
├── data-model.md                    # Phase 1 output (complete)
├── contracts/
│   └── api-contracts.md             # Phase 1 output (complete)
├── checklists/
│   └── implementation-readiness.md  # Pre-existing
└── tasks.md                         # Phase 2 output (specify tasks - not yet created)
```

### Source Code (affected files)

```text
src/native/core/
└── banana_ripeness.c                # MODIFY: replace keyword loop with banana_ml_predict_regression_score

src/c-sharp/asp.net/
├── Controllers/
│   └── RipenessController.cs        # MODIFY: deserialize native JSON → RipenessResult
├── Pipeline/
│   ├── PipelineContext.cs           # MODIFY: add InputJson property
│   ├── RipenessResult.cs            # NEW: sealed record
│   ├── PipelineStepResult.cs        # NEW: step result record
│   ├── IPipelineStep.cs             # NEW: step interface
│   ├── PipelineRunner.cs            # NEW: ordered step executor
│   └── Steps/
│       └── InputValidationStep.cs   # NEW: Order=10 validation step

src/typescript/shared/ui/src/
├── types.ts                         # MODIFY: Ripeness type
└── components/
    └── RipenessLabel.tsx            # MODIFY: COLORS map

src/typescript/react/src/
├── lib/
│   └── api.ts                       # MODIFY: add predictRipeness()
└── App.tsx                          # MODIFY: add ripeness form + RipenessLabel render

tests/
├── native/
│   └── test_ripeness.c              # NEW or MODIFY: label-domain ctest
└── unit/
    └── PipelineRunnerTests.cs       # NEW: pipeline step ordering + halt tests
```

**Structure Decision**: Monorepo multi-layer. Each file change is scoped to its owning domain layer per Constitution Principle I and II.

## Complexity Tracking

*No Constitution violations. No justification table required.*
