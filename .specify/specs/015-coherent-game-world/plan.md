# Implementation Plan: Coherent Game World Integration

**Branch**: `015-coherent-game-world` | **Date**: 2026-05-26 | **Spec**: `.specify/specs/015-coherent-game-world/spec.md`

**Input**: Feature specification from `.specify/specs/015-coherent-game-world/spec.md`

## Summary

Unify existing native runtime scene, route, objective, and gameplay-model slices into one coherent world profile with deterministic cross-slice continuity and a maintainable catalog growth path. Start in the DX12 POC validation surface, then use focused evidence and deterministic tests to prove integrated behavior.

## Technical Context

**Language/Version**: C11 native runtime and Win32 DX12 POC shell

**Primary Dependencies**:
- Existing scene catalog + continent asset registry (`src/native/engine/win32_dx12_poc/scene`)
- Scene browser and HUD overlays (`src/native/engine/win32_dx12_poc/overlay`)
- Objective policy + runtime launch flow (`src/native/engine/win32_dx12_poc/app`, `src/native/engine/win32_dx12_poc/objective`)
- Canonical route/county/region contracts (`src/native/engine/runtime/terrain/static_mesh`)
- Prior persistent-world and slice specs (`.specify/specs/009-*`, `010-*`, `011-*`, `014-*`)

**Storage**: In-memory runtime metadata + existing persistence/orchestration contracts (no new DB introduced)

**Testing**:
- Focused native runtime tests in `tests/native/runtime/engine`
- Deterministic ctest suite scoped to coherent-world scene/catalog behavior
- Runtime launch evidence via `BANANA_DX12_POC_AUTOTEST`

**Target Platform**: Windows 10+ DX12 prototype runtime

**Project Type**: Native runtime integration and orchestration slice

**Performance Goals**:
- Deterministic repeated coherent-world launches without signature drift
- Maintain current startup smoke loop usability for targeted variants

**Constraints**:
- Keep controller -> service -> pipeline -> native interop boundaries intact
- Avoid introducing broad API/frontend rewrites in this slice
- Preserve existing banana fallback reference behavior

**Scale/Scope**:
- One coherent-world launch profile
- At least two connected regional transitions with continuity checks
- Catalog mutation/disabled diagnostics safety path validated

## Constitution Check

- [x] Player trust and disclosure alignment verified: no public storefront claim changes in scope.
- [x] Storefront governance artifacts required: not applicable for this slice.
- [x] Cross-domain contracts mapped for native/runtime + orchestration continuity paths.
- [x] Quality gates defined with deterministic launch/continuity/mutation/diagnostics checks.
- [x] Reproducible delivery path identified with focused ctest + runtime evidence artifacts.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/015-coherent-game-world/
├── spec.md
├── plan.md
└── tasks.md
```

### Source Code (expected touchpoints)

```text
src/native/engine/win32_dx12_poc/
├── app/
├── objective/
├── overlay/
└── scene/

src/native/engine/runtime/
└── terrain/static_mesh/

tests/native/runtime/engine/

artifacts/native/015-coherent-game-world/
```

**Structure Decision**: Deliver coherent-world integration through existing native scene/overlay/runtime contracts and focused tests; no new top-level architecture or cross-domain package split.

## Implementation Phases

### Phase 0: Coherent World Discovery Baseline

- Map cross-slice contracts from specs 009/010/011/014 into one coherent-world profile target.
- Identify one authoritative launch profile and transition pair for continuity validation.
- Capture baseline diagnostics signature before integration changes.

### Phase 1: Coherent Launch Profile Integration

- Add coherent-world metadata composition path (scene + region + route + theme + placements).
- Ensure scene browser/HUD and launch diagnostics expose coherent-world identity explicitly.
- Enforce validation checks for route adjacency, county anchors, and placement integrity in one path.

### Phase 2: Continuity and Growth Safety

- Add deterministic cross-slice continuity test path for connected variants.
- Add mutation and disabled-entry diagnostics tests for catalog growth safety.
- Keep model catalog ownership centralized to avoid scene-by-scene rewrites.

### Phase 3: Evidence and Closure

- Run focused coherent-world ctest suite.
- Capture runtime launch logs for coherent-world profile and transition checkpoints.
- Update feature artifacts and execution dashboard readiness status.

## Validation Strategy

- Coherent launch test: one variant must pass full validation with stable metadata signature.
- Continuity test: repeated connected-variant transitions maintain deterministic checkpoints.
- Mutation safety test: introducing a new model binding does not change unrelated scene metadata.
- Disabled diagnostics test: disabled references return explicit diagnostics and controlled fallback semantics.

## Delivery Evidence

- `artifacts/native/015-coherent-game-world/coherent-world-diagnostics.md`
- `artifacts/native/015-coherent-game-world/continuity-evidence.md`
- Focused ctest outputs for coherent-world suite

## Final Validation Commands

- `cmake --build out/v3-native --target banana_engine_win32_dx12_poc`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test banana_runtime_demo_scene_catalog_coherent_launch_signature_test banana_runtime_demo_scene_catalog_coherent_failure_path_test banana_runtime_demo_scene_catalog_transition_continuity_test banana_runtime_demo_scene_catalog_transition_drift_test banana_runtime_demo_scene_catalog_coherent_mutation_safety_test banana_runtime_demo_scene_catalog_coherent_disabled_profile_test`
- `ctest -C Debug -R "runtime_demo_scene_catalog_(bootstrap|coherent_profile|coherent_launch_signature|coherent_failure_path|transition_continuity|transition_drift|coherent_mutation_safety|coherent_disabled_profile)_test" --test-dir out/v3-native --output-on-failure`
- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=2 ./out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`

## Complexity Tracking

No constitution exceptions are expected for this feature.
