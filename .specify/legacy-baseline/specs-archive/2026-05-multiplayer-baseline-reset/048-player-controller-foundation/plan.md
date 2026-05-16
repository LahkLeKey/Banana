# Implementation Plan: Player Controller Foundation

**Branch**: `048-player-controller-foundation` | **Date**: 2026-05-10 | **Spec**: `.specify/specs/048-player-controller-foundation/spec.md`
**Input**: Feature specification from `.specify/specs/048-player-controller-foundation/spec.md`

## Summary

Deliver a stable player controller baseline by enforcing keyboard-only movement and introducing a custom right-click context menu surface that does not influence movement state. This slice prioritizes predictable input behavior and UI action-surface scaffolding so future gameplay features (interact, harvest, inspect, etc.) can attach to the menu without reopening controller regressions.

## Technical Context

**Language/Version**: C11 (engine), TypeScript/React (web client)
**Primary Dependencies**: Emscripten WASM runtime, React hooks, existing engine world/render/physics modules
**Storage**: N/A (runtime in-memory controller/menu state)
**Testing**: Targeted runtime validation in VS Code browser + existing repo validation patterns
**Target Platform**: Browser runtime via WASM engine and React host page
**Project Type**: Monorepo web runtime slice (native engine + frontend page)
**Performance Goals**: Maintain existing 60 Hz frame tick and responsive key movement behavior
**Constraints**: Mouse movement remains out of scope; right-click is menu-only; no cross-layer contract expansion beyond controller/menu surface
**Scale/Scope**: Single gameplay viewport page and engine controller path with menu scaffolding for future actions

## Constitution Check

*GATE: Must pass before research. Re-check after design.*

- Pass: Domain-core-first respected (movement policy remains in native engine runtime loop).
- Pass: Layered contract preserved (React container forwards movement input; no duplicated domain rules).
- Pass: One-window validation compatible (VS Code browser checks remain primary).
- Pass: Cross-surface constraints respected (Bun + VITE contract unchanged, no ASP.NET/Fastify drift introduced).
- Pass: Confidence gate policy acknowledged and tracked in orchestration preflight.

- If confidence in the next code-improving step is below 80%, stop and resolve the uncertainty with targeted Q/A before continuing. Record the resulting constraint, assumption, or decision in this plan.

## Orchestration Preflight

- Run extension health preflight before implementation orchestration:
  - `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- Run confidence gate before major execution slices:
  - `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- Append heartbeat evidence per major step in `.specify/specs/048-player-controller-foundation/heartbeat-log.md`.
- If confidence drops below 80, pause and request human input before continuing.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/048-player-controller-foundation/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── player-controller-context-menu-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/engine/
├── engine.c
├── engine.h
└── CMakeLists.txt

src/typescript/react/src/pages/
└── GameEnginePage.tsx

scripts/
└── build-engine-wasm-emsdk.sh
```

**Structure Decision**: Use existing native-engine + React page surfaces and avoid introducing new top-level modules; keep scope limited to controller behavior and menu UI contract.

## DDD/SOLID Decomposition

- **Domain Policy**: Movement authority belongs to keyboard input path; menu state must not mutate movement intent.
- **Application Flow**: React layer owns key capture + menu rendering; engine loop owns movement application and camera-follow updates.
- **Infrastructure Adapters**: WASM export list and build script remain explicit adapters for browser-executable runtime artifacts.
- **Single Responsibility**: Menu UI handles visibility/actions; movement loop handles directional motion; build scripts only package/export runtime.

## Domain-Contract Test Decomposition

- **Bounded Test Suites**: Controller movement behavior and context-menu behavior are validated as separate suites/slices.
- **Suite Ownership**: Keyboard movement checks remain in controller behavior scope; menu open/close/placement checks remain in UI contract scope.
- **Testable Boundaries**: Boundary between menu and movement is explicit: right-click changes menu state only, movement keys change player position only.
- **Coverage Contract**: Preserve existing engine/runtime validation path and add regression checks that prove no mouse movement updates are emitted.

## Implementation Sequencing Notes

- Sequence 1: Preserve engine movement authority first (`engine_set_move_input`, tick loop behavior) before UI refinements.
- Sequence 2: Update React input capture to feed keyboard axes only and keep right-click isolated to menu state.
- Sequence 3: Verify exported WASM symbols remain aligned in both CMake and direct emsdk script surfaces before runtime validation.
- Sequence 4: Validate arbitration behavior under mixed-input edge cases (rapid alternation, viewport edge menu placement, focus loss).
- Sequence 5: Capture heartbeat evidence after each completed story checkpoint with confidence >= 80.

## Complexity Tracking

No constitution violations requiring exception handling for this slice.
