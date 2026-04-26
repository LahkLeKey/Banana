# Implementation Plan: CMake Preset Activation Fix

**Branch**: `008-cmake-preset-activation-fix` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/008-cmake-preset-activation-fix/spec.md`

## Summary

Stabilize CMake preset activation for Banana's multi-root workspace so CMake Tools configures deterministically, exposes native build/test discovery, and emits actionable diagnostics when activation is invalid.

## Technical Context

**Language/Version**: C11 (native targets), JSON workspace/settings contracts, CMake Presets v6  
**Primary Dependencies**: CMake Tools extension behavior, `CMakePresets.json`, `.vscode/settings.json`, `tests/.vscode/settings.json`  
**Storage**: N/A  
**Testing**: CMake Tools build/test discovery, `ctest` native lane (`tests/native`)  
**Target Platform**: Windows developer environment with VS Code multi-root workspace  
**Project Type**: Infrastructure/runtime contract hardening for native toolchain activation  
**Performance Goals**: Zero additional configure latency beyond current baseline for successful preset activation  
**Constraints**: Preserve existing native target contracts, avoid behavior changes in native feature code, preserve multi-root workflow  
**Scale/Scope**: Single bounded bug-fix slice scoped to preset activation/discovery and diagnostics

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Domain Core First | PASS | No native business logic changes required. |
| II. Layered Interop Contract | PASS | No controller/service/pipeline/native interop behavior change. |
| III. Spec First Delivery | PASS | Spec created before plan for feature 008. |
| IV. Verifiable Quality Gates | PASS | Native lane validation remains explicit and required. |
| V. Documentation and Wiki Parity | PASS | Planning artifacts updated in feature scope; no workflow runbook behavior change beyond this slice. |

**Post-design check (Phase 1)**: PASS. No constitution violations introduced by design artifacts.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/008-cmake-preset-activation-fix/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── cmake-preset-activation-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
.vscode/
└── settings.json

tests/.vscode/
└── settings.json

CMakePresets.json
Banana.code-workspace

tests/native/
└── CMakeLists.txt
```

**Structure Decision**: Keep implementation strictly on workspace/preset activation surfaces and validation entry points; do not modify native model/business logic files.

## Complexity Tracking

No constitution violations requiring justification.
