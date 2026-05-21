# Implementation Plan: C Runtime Parallel Foundation

**Branch**: `[055-c-runtime-parallel-foundation]` | **Date**: 2026-05-19 | **Spec**: .specify/specs/055-c-runtime-parallel-foundation/spec.md
**Jurisdiction**: package | **Agent of Record**: native core + API interop + React runtime bridge
**Input**: Feature specification from `.specify/specs/055-c-runtime-parallel-foundation/spec.md`

## Summary

Decompose the native runtime into DDD/SOLID-aligned C modules, split monolithic `engine_tick` into explicit phase boundaries, migrate multiplayer reconciliation ownership into C APIs, and enforce stronger native test coverage gates so renderer-facing bridges remain thin and replaceable.

## Authority Chain

- **Governing Spec**: `.specify/specs/055-c-runtime-parallel-foundation/spec.md`
- **Bounded Context**: `src/native/engine` runtime loop, sync ownership, and renderer-adapter boundaries
- **Executing Authority**: Native engine owners with coordinated TypeScript bridge updates
- **Superseded Inputs**: none
- **Archive Action**: none

## Technical Context

**Language/Version**: C11 (native engine), TypeScript (interop bridge)
**Primary Dependencies**: CMake native targets, existing engine/world/AI modules, Bun runtime bridge
**Storage**: N/A
**Testing**: native CMake tests (`ctest`), route/runtime tests where contract shapes change
**Target Platform**: WASM + native desktop build targets
**Project Type**: native engine library + runtime bridge
**Performance Goals**: preserve realtime baseline and enable phased parallelization strategy without behavioral regressions
**Constraints**: keep renderer contracts replaceable; avoid gameplay logic ownership in TypeScript
**Scale/Scope**: engine tick decomposition, module extraction, sync API ownership migration

## Constitution Check

*GATE: Must pass before research. Re-check after design.*

- Confidence gate threshold is 80% for autonomous continuation.
- Parallel-runtime decisions must preserve explicit contracts and deterministic fallback mode.
- Native decomposition must maintain bounded responsibilities and testability.

## Orchestration Preflight

- Run extension health preflight before implementation orchestration:
  - `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- Run confidence gate before major execution slices:
  - `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- Append heartbeat evidence per major step in `.specify/specs/055-c-runtime-parallel-foundation/heartbeat-log.md`.
- If confidence drops below 80, pause and request human input before continuing.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/055-c-runtime-parallel-foundation/
├── plan.md
├── spec.md
├── tasks.md
├── heartbeat-log.md
└── checklists/
    └── c-runtime-parallel-goals.md
```

### Source Code (repository root)

```text
src/native/engine/
├── engine.c
├── engine.h
├── engine_serialize.c
├── physics/
├── ai/
├── world/
└── render/

src/typescript/api/src/
└── services/nativeEngine.ts

src/typescript/react/src/
└── pages/GameEnginePage.tsx

tests/native/
└── *.c
```

**Structure Decision**: Keep the existing engine tree but extract bloated runtime responsibilities from `engine.c` into bounded, domain-named native modules to enforce DDD/SOLID seams.

## DDD/SOLID Decomposition

- **Domain Policy**: Tick phase contracts, player reconciliation rules, and deterministic fallback behavior are domain policy and remain renderer-agnostic.
- **Application Flow**: `engine_tick` becomes an orchestrator calling phase modules (input/sim/sync/camera/render submit) without embedding all logic inline.
- **Infrastructure Adapters**: WASM bridge and TypeScript bridge invoke explicit C APIs; adapters do not own simulation policies.
- **Single Responsibility**: Each extracted C module owns one bounded concern (for example, tick phases, player sync, camera tracking, terrain budget).

## Domain-Contract Test Decomposition

- **Bounded Test Suites**: Organize/expand native tests by domain responsibility (physics, AI/controller, world/sync, render/tick contracts).
- **Suite Ownership**: Keep aggregate orchestrator tests thin; domain suites verify local behavior directly.
- **Testable Boundaries**: Each exported C API introduced for migration has direct contract assertions.
- **Coverage Contract**: Run native coverage flow and enforce at least 80% line coverage gate while tracking incremental increases for decomposed modules.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| Multi-module extraction from monolithic engine runtime | Needed to restore SRP and testability | Keeping logic in one file increases coupling and blocks parallelization |
