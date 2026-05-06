# Implementation Plan: 000-native-rehydration

**Branch**: `000-native-rehydration` | **Date**: 2026-05-03 | **Spec**: spec.md
**Input**: Feature specification from `.specify/specs/000-native-rehydration/spec.md`

**Note**: This template is filled in by the `specify plan` command. See `.specify/templates/plan-template.md` for the execution workflow.

## Summary

[Extract from feature spec: primary requirement + technical approach from research]

## Technical Context

<!--
-->

**Language/Version**: C 17 standard
**Primary Dependencies**: CMake 3.20+, C standard library
**Storage**: PostgreSQL for model data; memory-resident during inference
**Testing**: CMake/Ctest with native C tests
**Target Platform**: Linux (glibc), macOS, Windows
**Project Type**: Native C library with optimized inference
**Performance Goals**: <50ms inference per image on CPU; <10MB memory footprint
**Constraints**: Cross-platform ABI stable; no external deps; thread-safe
**Scale/Scope**: 3 model families; 10k+ inference calls/day

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

[Gates determined based on constitution file]

## Project Structure

### Documentation (this feature)

```text
.specify/specs/000-native-rehydration/
├── plan.md              # This file (specify plan command output)
├── research.md          # Phase 0 output (specify plan command)
├── data-model.md        # Phase 1 output (specify plan command)
├── quickstart.md        # Phase 1 output (specify plan command)
├── contracts/           # Phase 1 output (specify plan command)
└── tasks.md             # Phase 2 output (specify tasks command - NOT created by specify plan)
```

### Source Code (repository root)

**Structure**: Native C layer integration and interop boundary hardening

```text
src/native/
├── core/
│   ├── banana_core.h
│   ├── banana_batch.c
│   ├── banana_calc.c
│   ├── banana_classify.c
│   ├── banana_ripeness.c
│   ├── banana_status.c
│   ├── banana_truck.c
│   └── banana_status.h
├── wrapper/
│   ├── libbanana.c
│   └── libbanana.h
└── tests/
    ├── CMakeLists.txt
    └── unit/

CMakeLists.txt
CMakePresets.json
```

**Structure Decision**: Native rehydration focuses on C layer boundary validation and wrapper interface stability. DAL contracts and error handling paths established.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |
