# Implementation Plan: Coherent World Cohesion Continuation

**Branch**: `021-coherent-world-cohesion` | **Date**: 2026-05-26 | **Spec**: `.specify/specs/021-coherent-world-cohesion/spec.md`

**Input**: Feature specification from `.specify/specs/021-coherent-world-cohesion/spec.md`

## Summary

Continue coherent-world cohesion by enforcing canonical checkpoint sequence identity, validating replay determinism under sequence-drift retries, and adding one safe additive continuity checkpoint attribute through the owning API contract path.

## Technical Context

**Language/Version**: C11 native runtime + TypeScript API (Bun/Fastify)

**Primary Dependencies**:
- 020 coherent-world convergence baseline
- Native demo scene coherence/transition suite
- API continuity payload and replay services

**Storage**: Existing baseline/delta replay repositories

**Testing**:
- Focused native coherence/transition suites
- API continuity contract and integration suites
- Deterministic diagnostics/replay lineage assertions

**Target Platform**: Windows native + Bun API runtime

**Constraints**:
- Preserve route -> service -> persistence boundaries
- Keep native changes scoped to continuity/coherence seams only when needed
- Keep storefront/public claims out of scope

## Constitution Check

- [x] No disclosure/storefront changes
- [x] Cross-domain contracts identified
- [x] Quality gates defined
- [x] Evidence outputs defined

## Project Structure

```text
.specify/specs/021-coherent-world-cohesion/
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
artifacts/native/021-coherent-world-cohesion/
artifacts/api/021-coherent-world-cohesion/
```

## Implementation Phases

### Phase 0: Setup + Inventory
- Seed evidence docs and baseline validation command inventory.
- Inventory checkpoint-sequence seams from 020 outputs.

### Phase 1: Canonical Sequence Inputs (US1)
- Tighten checkpoint-sequence canonicalization / regression reject diagnostics.
- Add API contract/integration reject-path coverage.

### Phase 2: Replay Determinism (US2)
- Validate replay lineage remains authoritative after sequence-drift retries.
- Expand deterministic diagnostics for sequence-drift retry family.

### Phase 3: Growth Safety (US3)
- Add one additive continuity checkpoint field through owning contract path.
- Validate deterministic mutation and replay persistence behavior.

### Phase 4: Hardening + Closeout
- Run focused native/API suites and capture outputs.
- Finalize spec/plan/tasks/dashboard and handoff summary.

## Validation Strategy

- Native focused: coherent profile + transition continuity/drift + continuity roundtrip
- API focused: `test:continuity` + `test:integration:continuity`

## Delivery Evidence

- `artifacts/native/021-coherent-world-cohesion/continuity-runtime-evidence.md`
- `artifacts/api/021-coherent-world-cohesion/continuity-contract-evidence.md`

## Final Command Set

- `cd src/typescript/api && bun run test:continuity`
- `cd src/typescript/api && bun run test:integration:continuity`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test banana_runtime_demo_scene_catalog_transition_continuity_test banana_runtime_demo_scene_catalog_transition_drift_test banana_runtime_demo_scene_catalog_continuity_roundtrip_test -- -m:1`
- `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`

## Status

Complete (24/24).
