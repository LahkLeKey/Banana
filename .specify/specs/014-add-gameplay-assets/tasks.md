# Tasks: Gameplay Asset Model Expansion

**Input**: Design documents from `.specify/specs/014-add-gameplay-assets/`

**Prerequisites**: `plan.md`, `spec.md`

**Tests**: Native deterministic asset-resolution, scene-placement, and failure-path checks are required for each user story increment.

## Phase 1: Setup (Shared Infrastructure)

Path targets:
- `src/native/engine/win32_dx12_poc/scene/`
- `src/native/engine/runtime/`
- `tests/native/`
- `artifacts/native/014-add-gameplay-assets/`

- [x] T001 Identify the current banana-only and terrain-placeholder asset registration path in `src/native/engine/win32_dx12_poc/scene/` and document the owning files in `artifacts/native/014-add-gameplay-assets/catalog-diagnostics.md`
- [x] T002 Create the feature evidence directory `artifacts/native/014-add-gameplay-assets/` and seed initial evidence headings for scene launch and diagnostics output
- [x] T003 [P] Map the cheapest existing validation scenes from the prior demo-scene slice into a short target list in `artifacts/native/014-add-gameplay-assets/scene-launch-evidence.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

Path targets:
- `docs/banana-archipelago-worldbuilding.md`
- `src/native/engine/runtime/`
- `src/native/engine/win32_dx12_poc/scene/`
- `src/native/engine/win32_dx12_poc/overlay/`
- `tests/native/`

- [x] T004 Define the gameplay asset catalog contract and stable model keys in the runtime/scene layer for non-terrain asset resolution
- [x] T005 [P] Define the first three theme groups for tropical/coastal, urban/industrial, and rugged/wild slices using `docs/banana-archipelago-worldbuilding.md` as the canonical theming source
- [x] T006 Add deterministic scene-placement records and diagnostics tags for gameplay asset model references
- [x] T007 Add missing/disabled asset fallback handling so scenes emit explicit diagnostics instead of silently collapsing to placeholder-only composition

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Richer Traversal Landmarks (Priority: P1) 🎯 MVP

**Goal**: Targeted scenes render multiple gameplay landmarks and props beyond the single banana mesh.

**Independent Test**: Launch a focused validation scene and confirm at least one landmark cluster, one traversal prop set, and the banana reference asset render together without placeholder-only fallback.

### Tests for User Story 1

- [x] T008 [P] [US1] Add a native deterministic asset-resolution test in `tests/native/` covering multiple gameplay model keys in one validation scene
- [x] T009 [P] [US1] Add a native failure-path test in `tests/native/` proving missing gameplay model keys emit diagnostics and preserve controlled fallback behavior

### Implementation for User Story 1

- [x] T010 [P] [US1] Implement the first gameplay landmark model entries in the native asset catalog under `src/native/engine/runtime/` or the owning scene catalog path
- [x] T011 [P] [US1] Implement traversal prop entries that can be placed alongside landmarks in targeted validation scenes
- [x] T012 [US1] Update the narrowest validation scene bootstrap path in `src/native/engine/win32_dx12_poc/scene/` to load multiple non-terrain gameplay models deterministically
- [x] T013 [US1] Preserve and explicitly wire the existing banana mesh as a reference/fallback asset in the same catalog path
- [x] T014 [US1] Capture runtime evidence for the MVP scene in `artifacts/native/014-add-gameplay-assets/scene-launch-evidence.md`

**Checkpoint**: User Story 1 is independently functional and demonstrates the runtime moving beyond the banana-plus-terrain baseline

---

## Phase 4: User Story 2 - Region-Themed Asset Variants (Priority: P1)

**Goal**: Representative scenes resolve distinct themed gameplay model sets for different Banana Archipelago region types.

**Independent Test**: Launch three representative scenes or variants and verify tropical/coastal, urban/industrial, and rugged/wild slices use different gameplay model groups.

### Tests for User Story 2

- [x] T015 [P] [US2] Add a theme-coverage test in `tests/native/` proving targeted scenes resolve different asset theme sets by region
- [x] T016 [P] [US2] Add a repeated-launch determinism test in `tests/native/` for theme-specific scene variants

### Implementation for User Story 2

- [x] T017 [P] [US2] Implement tropical/coastal gameplay asset entries and bind them to at least one targeted scene variant
- [x] T018 [P] [US2] Implement urban/industrial gameplay asset entries and bind them to at least one targeted scene variant
- [x] T019 [P] [US2] Implement rugged/wild gameplay asset entries and bind them to at least one targeted scene variant
- [x] T020 [US2] Update scene selection or bootstrap metadata in `src/native/engine/win32_dx12_poc/scene/` and `src/native/engine/win32_dx12_poc/overlay/` so themed variants can be launched intentionally during playtests
- [x] T021 [US2] Capture comparative themed-scene evidence in `artifacts/native/014-add-gameplay-assets/scene-launch-evidence.md`

**Checkpoint**: User Stories 1 and 2 both work independently, and region identity is visible through gameplay asset variation

---

## Phase 5: User Story 3 - Asset Catalog Growth Without Scene Rewrites (Priority: P2)

**Goal**: New gameplay models can be added through catalog registration and scene references without unrelated bootstrap rewrites.

**Independent Test**: Register one new gameplay model, reference it from a validation scene, and launch that scene without changing unrelated scene bootstrap logic.

### Tests for User Story 3

- [x] T022 [P] [US3] Add a native catalog-mutation test in `tests/native/` proving a new gameplay model entry can be added without changing unrelated scene bootstrap code
- [x] T023 [P] [US3] Add a diagnostics-path test in `tests/native/` covering disabled catalog entries referenced by a scene

### Implementation for User Story 3

- [x] T024 [P] [US3] Refine the gameplay asset catalog structure so new model entries are registered through one owning path in `src/native/engine/runtime/` or `src/native/engine/win32_dx12_poc/scene/`
- [x] T025 [US3] Update targeted scene metadata to reference gameplay asset model keys rather than one-off hardcoded mesh wiring
- [x] T026 [US3] Document the minimal add-a-model workflow and diagnostics expectations in `artifacts/native/014-add-gameplay-assets/catalog-diagnostics.md`

**Checkpoint**: All user stories are independently functional and the asset catalog can grow without scene-by-scene rewrites

---

## Phase 6: Polish and Cross-Cutting

- [x] T027 [P] Refresh `.specify/specs/execution-dashboard.md` if this feature needs explicit dashboard tracking
- [x] T028 Update `.specify/specs/014-add-gameplay-assets/plan.md` and evidence docs with any final validation command adjustments discovered during implementation
- [x] T029 [P] Capture final deterministic test output and runtime logs under `artifacts/native/014-add-gameplay-assets/`
- [x] T030 Run the focused validation path for this feature and record the results in `artifacts/native/014-add-gameplay-assets/scene-launch-evidence.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - blocks all user stories
- **User Stories (Phase 3+)**: Depend on Foundational completion
- **Polish (Phase 6)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Starts after Foundational and defines the MVP validation surface
- **User Story 2 (P1)**: Starts after Foundational and builds on the same catalog/placement contracts, but remains independently testable via themed scenes
- **User Story 3 (P2)**: Starts after Foundational and should be validated after at least one concrete gameplay asset path exists

### Parallel Opportunities

- T003, T005, T008, T009, T010, T011, T015, T016, T017, T018, T019, T022, T023, T024, T027, and T029 can run in parallel when dependencies are satisfied
- Theme-specific asset entry work for US2 can be split by region theme across contributors
- Evidence capture can proceed in parallel with test hardening once target scenes are stable

## Implementation Strategy

### MVP First

1. Complete Setup and Foundational phases
2. Implement User Story 1
3. Validate deterministic multi-model scene composition and failure-path diagnostics
4. Stop and review the first richer gameplay scene before expanding theme coverage

### Incremental Delivery

1. Deliver US1 to prove the runtime can render more than banana-plus-terrain
2. Deliver US2 to make regional theming visible in playtests
3. Deliver US3 to make future gameplay asset additions cheaper and less brittle

## Notes

- `[P]` tasks = different files or separable evidence work with no direct dependency conflicts
- Prefer the narrowest owning runtime/bootstrap file when implementing catalog or placement changes
- Keep the banana mesh available as a deliberate reference asset; do not treat removal of the banana as the feature goal
- Validation evidence is part of the definition of done for this feature