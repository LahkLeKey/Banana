# Implementation Plan: Gameplay/API Persistence Continuity

**Branch**: `016-gameplay-api-continuity` | **Date**: 2026-05-26 | **Spec**: `.specify/specs/016-gameplay-api-continuity/spec.md`

**Input**: Feature specification from `.specify/specs/016-gameplay-api-continuity/spec.md`

## Summary

Bridge coherent-world runtime transitions with API-backed persistence so progression checkpoints survive cross-slice round trips with deterministic signatures and explicit diagnostics on reject paths.

## Technical Context

**Language/Version**: C11 native runtime + TypeScript API services

**Primary Dependencies**:
- Coherent-world runtime contract from spec 015 in `src/native/engine/win32_dx12_poc/scene/`
- API orchestration and persistence contracts in `src/typescript/api/src/`
- Existing persistence/orchestration baselines from specs 007 and 009
- Native/API focused tests under `tests/native/` and `tests/integration/`

**Storage**: Existing API persistence store and runtime continuity payload handoff (no new database engine)

**Testing**:
- Native deterministic continuity tests
- API contract and integration persistence tests
- Focused cross-layer validation commands + evidence captures

**Target Platform**: Windows runtime + API domain test/runtime environments

**Project Type**: Cross-domain runtime/API integration

**Performance Goals**:
- Deterministic continuity signatures across repeated transition runs
- No material regression in current startup/transition validation loops

**Constraints**:
- Preserve native controller/service/pipeline boundaries
- Preserve existing API contract governance and route validation patterns
- Avoid introducing unrelated frontend/storefront changes

**Scale/Scope**:
- One canonical continuity payload contract
- Connected transition persistence for at least one representative pair
- Mutation-safe growth path for one new persisted field

## Constitution Check

- [x] Player trust/disclosure alignment verified (no public store copy changes).
- [x] Storefront governance impact: not applicable in this slice.
- [x] Cross-domain contract mapping includes native coherent profile + API persistence schema.
- [x] Quality gates include deterministic pass and failure diagnostics.
- [x] Delivery evidence path defined for both native and API artifacts.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/016-gameplay-api-continuity/
├── spec.md
├── plan.md
└── tasks.md
```

### Source Code (expected touchpoints)

```text
src/native/engine/win32_dx12_poc/
└── scene/

src/typescript/api/src/
├── routes/
├── services/
└── middleware/

tests/native/runtime/engine/
tests/integration/

artifacts/native/016-gameplay-api-continuity/
artifacts/api/016-gameplay-api-continuity/
```

**Structure Decision**: Keep runtime continuity composition in native owning paths and persistence validation in API owning paths, joined by one shared continuity payload contract.

## Implementation Phases

### Phase 0: Contract Inventory

- Map coherent runtime transition data to current API persistence schema.
- Identify mismatch/drift points and canonical ownership.

### Phase 1: Canonical Continuity Contract

- Define shared continuity payload fields and validation invariants.
- Add strict reject-path diagnostics for invalid payloads.

### Phase 2: Persistence Round-Trip

- Persist connected transition checkpoint state.
- Rehydrate state on return transition and verify deterministic signatures.

### Phase 3: Growth Safety + Evidence

- Add one new persisted field through owning contract path.
- Capture focused native/API test outputs and runtime evidence.

## Validation Strategy

- Deterministic connected transition round-trip tests (>=10 iterations).
- Contract drift tests between runtime payload and API validators.
- Reject-path tests for invalid route/variant/signature combinations.

## Delivery Evidence

- `artifacts/native/016-gameplay-api-continuity/continuity-runtime-evidence.md`
- `artifacts/api/016-gameplay-api-continuity/continuity-contract-evidence.md`

## Final Command Set

Native focused continuity closeout:

- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_coherent_profile_test -- -m:1`
- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_transition_continuity_test -- -m:1`
- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_transition_drift_test -- -m:1`
- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_continuity_roundtrip_test -- -m:1`
- `ctest -C Debug --test-dir out/v3-native -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)" --output-on-failure`

API focused continuity closeout:

- `bun run test:continuity`
- `bun run test:integration:continuity`
- `bun test src/domains/persistent-world-orchestration/services/replayMergeService.test.ts src/lib/contracts/v1/contracts.test.ts src/routes/world.revisit.contract.test.ts`

Final status:

- Native focused suite: `4/4 passed`
- API continuity harness: `14/14 + 5/5 passed`
- API adjacent contract/replay/revisit suite: `11/11 passed`

## Complexity Tracking

No constitution exceptions expected.
