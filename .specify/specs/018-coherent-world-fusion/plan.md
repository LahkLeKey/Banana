# Implementation Plan: Coherent World Fusion Continuation

**Branch**: `018-coherent-world-fusion` | **Date**: 2026-05-26 | **Spec**: `.specify/specs/018-coherent-world-fusion/spec.md`

**Input**: Feature specification from `.specify/specs/018-coherent-world-fusion/spec.md`

## Summary

Continue collapsing remaining gameplay/runtime/API slice seams into one coherent-world contract surface by tightening launch identity convergence, drift rejection, and deterministic evolution paths.

## Technical Context

**Language/Version**: C11 native runtime + TypeScript API (Bun/Fastify)

**Primary Dependencies**:
- 017 coherent-world unification as baseline
- Native demo scene catalog coherence and transition suites
- API continuity payload/replay route contracts

**Storage**: Existing baseline/delta replay repositories

**Testing**:
- Focused native coherence/drift tests (serial target builds)
- API continuity contract + integration suites
- Deterministic reject-path diagnostics validation

**Target Platform**: Windows native + Bun API test runtime

**Project Type**: Cross-domain continuation slice

**Constraints**:
- Keep route -> service -> persistence API flow
- Keep native convergence in existing scene-catalog surfaces
- Avoid unrelated storefront/frontend changes

## Constitution Check

- [x] No storefront/disclosure changes
- [x] Cross-domain contract seams identified
- [x] Success/failure gates defined
- [x] Evidence targets defined

## Project Structure

```text
.specify/specs/018-coherent-world-fusion/
├── spec.md
├── plan.md
└── tasks.md
```

```text
src/native/engine/win32_dx12_poc/scene/
src/typescript/api/src/
  domains/persistent-world-orchestration/
  routes/
  integration/

tests/native/runtime/engine/
artifacts/native/018-coherent-world-fusion/
artifacts/api/018-coherent-world-fusion/
```

## Implementation Phases

### Phase 0: Setup + Inventory
- Seed evidence artifacts and command baseline.
- Map remaining converged-vs-legacy seams from 017 outputs.

### Phase 1: Identity Surface Convergence
- Tighten canonical slice identity checks in native/API seams.
- Add deterministic reject paths for malformed identity inputs.

### Phase 2: Contract Fusion Validation
- Extend accept/reject continuity tests for legacy drift shapes.
- Validate deterministic replay lineage under mixed entry paths.

### Phase 3: Growth Safety + Closeout
- Add one additive continuity attribute in owning path.
- Validate deterministic mutation safety and finalize evidence.

## Validation Strategy

- Native: coherent profile, transition continuity/drift, continuity roundtrip
- API: `test:continuity`, `test:integration:continuity`, plus focused replay/contract suites

## Delivery Evidence

- `artifacts/native/018-coherent-world-fusion/continuity-runtime-evidence.md`
- `artifacts/api/018-coherent-world-fusion/continuity-contract-evidence.md`

## Final Command Set and Status

- Native focused hardening (serial build + ctest):
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_transition_continuity_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_transition_drift_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_continuity_roundtrip_test -- -m:1`
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`
- API continuity suites:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun run test:integration:continuity`
- Status: Complete. All targeted native/API continuity and drift-validation suites passed.
