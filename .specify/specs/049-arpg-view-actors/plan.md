# Implementation Plan: ARPG View and Actor Scaffolding

**Branch**: `[049-arpg-view-actors]` | **Date**: 2026-05-11 | **Spec**: `.specify/specs/049-arpg-view-actors/spec.md`
**Input**: Feature specification from `.specify/specs/049-arpg-view-actors/spec.md`

## Summary

Rescaffold the base island scene to deliver stable ARPG-style centered framing for keyboard movement, complete context-menu interactions beyond placeholders, and introduce non-player world actors with target-specific interaction outcomes. The implementation will align scene anchoring and follow-camera behavior in native engine code, then complete interaction contracts in React/native interop so movement, camera, and actor interactions remain coherent in one session.

## Technical Context

**Language/Version**: C11 (native engine), TypeScript (React app), Bash (build/runtime orchestration)
**Primary Dependencies**: Emscripten WebGL2 runtime, React 19 + Vite 5 runtime shell, Bun package workflow
**Storage**: N/A (runtime-only scene state for this slice)
**Testing**: Native C test suites + React behavior tests + interactive local runtime validation in VS Code integrated browser
**Target Platform**: Web runtime (WASM in browser) and native-compatible engine codepaths
**Project Type**: Monorepo game/runtime feature slice (native core + web container)
**Performance Goals**: Maintain smooth interactive runtime at perceived 60 FPS behavior with no control deadlocks
**Constraints**: Preserve WASD/Arrow keyboard movement contract, keep right-click menu decoupled from movement, keep ARPG framing stable across viewport ratios
**Scale/Scope**: Single-playfield proof-of-concept scene with player banana + multiple non-player actors and actionable context menu outcomes

## Constitution Check

*GATE: Must pass before research. Re-check after design.*

- Pass: Domain core remains under `src/native` with React as orchestration shell (Principles I and II).
- Pass: Spec-first workflow is followed with active feature pointer and generated plan artifacts (Principle III).
- Pass: One-window interactive validation remains required for scene/camera/menu behavior (Principle VI).
- Pass: Confidence gate policy retained; no unresolved confidence blockers for planning scope (Principles XVIII and XXII).
- Pass: Domain-contract test decomposition will be applied in tasks with dedicated suites for camera, controller/menu, and actor interactions (Principle XXIV).

- If confidence in the next code-improving step is below 80%, stop and resolve the uncertainty with targeted Q/A before continuing. Record the resulting constraint, assumption, or decision in this plan.

## Orchestration Preflight

- Run extension health preflight before implementation orchestration:
  - `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- Run confidence gate before major execution slices:
  - `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- Append heartbeat evidence per major step in `.specify/specs/049-arpg-view-actors/heartbeat-log.md`.
- If confidence drops below 80, pause and request human input before continuing.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/049-arpg-view-actors/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── interaction-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/engine/
├── engine.c
└── render/
    ├── camera.c
    └── renderer.c

src/typescript/react/src/
└── pages/
    └── GameEnginePage.tsx

tests/native/
└── engine/

src/typescript/react/src/pages/__tests__/
└── GameEnginePage.context-menu.test.tsx
```

**Structure Decision**: Use existing native-engine + React container surfaces; no new top-level projects. The feature is a cross-domain gameplay slice where camera/scene authority remains native and UI interaction shell remains React.

## DDD/SOLID Decomposition

- **Domain Policy**: Player-centric ARPG framing, target-scoped interactions, and movement/menu arbitration rules live as engine/domain contracts.
- **Application Flow**: React captures user input and context intent, then dispatches to native engine contracts; native loop owns movement, camera, and actor outcomes.
- **Infrastructure Adapters**: WASM export list, runtime asset copy flow, and local containerized Emscripten build scripts provide execution adapters.
- **Single Responsibility**: Camera framing logic, terrain/world positioning, actor-interaction resolution, and menu presentation each remain in isolated modules/surfaces.

## Domain-Contract Test Decomposition

- **Bounded Test Suites**:
  - `camera-framing` suite validates centered follow region and viewport-ratio stability.
  - `controller-arbitration` suite validates WASD responsiveness and right-click/menu decoupling.
  - `actor-interaction` suite validates target resolution and action outcome specificity.
  - `context-menu` suite validates open/close/selection behavior and edge-safe placement.
- **Suite Ownership**: Native suites assert frame/update contracts; React suites assert UI/menu behavior and bridge correctness; one aggregate runtime smoke validates the integrated loop.
- **Testable Boundaries**: Each suite owns one primary behavior boundary and avoids cross-suite state coupling.
- **Coverage Contract**: Maintain existing native quality gates and React test gates; do not regress current 100% native quality expectations where applicable.

## Complexity Tracking

No constitution violations requiring justification at planning time.

## Plan to Implementation Deltas

- Camera framing contract was hardened beyond initial scope with defensive projection/view guards:
  - `camera_get_view` now protects against degenerate up-vector alignment.
  - `camera_get_projection` now guards non-positive aspect ratios.
- Runtime viewport contract was made explicit in native render flow:
  - `renderer_resize` now rejects invalid dimensions and updates `glViewport` when GL is active.
- Interaction contract was expanded from placeholder dispatch to actionable actor targeting:
  - Native engine exports `_engine_handle_right_click` and `_engine_handle_right_click_normalized` now resolve nearest eligible non-player actor.
  - React context menu actions now dispatch bridge calls and emit deterministic user-visible feedback for target and no-target outcomes.
- World-population scope was concretized with deterministic non-player actor spawn offsets to stabilize runtime and tests.
- Brand asset integration was added as an implementation-layer enhancement:
  - Splash image standardized to `splash.png` and rendered as a loading overlay while WASM status is `idle/loading`.
  - Verified in-browser on local dev runtime with loading overlay visible before gameplay loop handoff.
