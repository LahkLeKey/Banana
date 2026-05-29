# Implementation Plan: Coherent World Slice Convergence

**Branch**: `017-coherent-world-unification` | **Date**: 2026-05-26 | **Spec**: `.specify/specs/017-coherent-world-unification/spec.md`

**Input**: Feature specification from `.specify/specs/017-coherent-world-unification/spec.md`

## Summary

Converge remaining gameplay/runtime/API slices into one coherent-world flow by enforcing authoritative slice identity, deterministic continuity checkpoint lineage, and strict drift diagnostics across native and API boundaries.

## Technical Context

**Language/Version**: C11 native runtime + TypeScript API services

**Primary Dependencies**:
- Coherent-world runtime baseline from `015-coherent-game-world`
- Continuity persistence baseline from `016-gameplay-api-continuity`
- Native convergence tests in `tests/native/runtime/engine/`
- API contract + integration suites in `src/typescript/api/src/`

**Storage**: Existing authoritative baseline/delta replay services

**Testing**:
- Native focused continuity + drift suites (serial target builds)
- API continuity contract + integration harnesses
- Drift and failure-path diagnostics validation

**Target Platform**: Windows native runtime + Bun/Fastify API validation paths

**Project Type**: Cross-domain runtime/API convergence

**Performance Goals**:
- Deterministic continuity signatures over repeated round trips
- Zero regressions in continuity reject-path diagnostics determinism

**Constraints**:
- Preserve route -> service -> persistence flow in API
- Keep native convergence checks in existing scene catalog contract surfaces
- Avoid unrelated storefront/frontend scope

**Scale/Scope**:
- One convergence spec over runtime/API continuity slices
- One additional convergence checkpoint field path as mutation-safe pattern

## Constitution Check

- [x] No storefront/disclosure updates required.
- [x] Runtime/API contract convergence explicitly identified.
- [x] Deterministic success/failure quality gates defined.
- [x] Delivery evidence paths defined.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/017-coherent-world-unification/
├── spec.md
├── plan.md
└── tasks.md
```

### Source Code (expected touchpoints)

```text
src/native/engine/win32_dx12_poc/scene/
src/typescript/api/src/
  domains/persistent-world-orchestration/
  routes/
  integration/

tests/native/runtime/engine/

artifacts/native/017-coherent-world-unification/
artifacts/api/017-coherent-world-unification/
```

## Implementation Phases

### Phase 0: Setup + Inventory

- Create evidence directories for runtime/API convergence outputs.
- Inventory remaining slice contract seams and baseline validation commands.

### Phase 1: Convergence Contract Integrity

- Confirm one authoritative slice identity and continuity lineage constraints.
- Add/expand drift diagnostics and deterministic reject-path validation.

### Phase 2: Cross-Slice Round-Trip Convergence

- Validate converged slice launch/transition/replay behavior.
- Record deterministic round-trip evidence.

### Phase 3: Growth Safety + Closeout

- Validate add-a-field mutation-safe pattern remains stable.
- Finalize evidence, dashboard status, and closeout command set.

## Validation Strategy

- Native focused suites:
  - `coherent_profile_test`
  - `transition_continuity_test`
  - `transition_drift_test`
  - `continuity_roundtrip_test`
- API focused suites:
  - `test:continuity`
  - `test:integration:continuity`
  - replay/contract/revisit targeted suites

## Delivery Evidence

- `artifacts/native/017-coherent-world-unification/continuity-runtime-evidence.md`
- `artifacts/api/017-coherent-world-unification/continuity-contract-evidence.md`

## Final Command Set and Status

- Native configure + focused serial builds:
  - `cmake -S src/native -B out/v3-native`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_launch_signature_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_failure_path_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_transition_continuity_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_transition_drift_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_continuity_roundtrip_test -- -m:1`
- Native focused ctest runs:
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|coherent_launch_signature_test|coherent_failure_path_test|transition_continuity_test)"`
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`
- API continuity suites:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun run test:integration:continuity`
- Status: Complete. All targeted native and API continuity validation suites passed.
