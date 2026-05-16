# Tasks: ARPG View and Actor Scaffolding

**Input**: Design documents from `.specify/specs/049-arpg-view-actors/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Include test tasks because the spec defines explicit test organization and measurable acceptance scenarios.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2, US3)
- Every task includes an exact file path

## Setup (Shared Infrastructure)

**Purpose**: Prepare orchestration preflight and feature scaffolding checks.

- [x] T001 Run extension-health preflight and capture output in `.specify/specs/049-arpg-view-actors/heartbeat-log.md`
- [x] T002 Run confidence gate startup check and append result to `.specify/specs/049-arpg-view-actors/heartbeat-log.md`
- [x] T003 [P] Add a feature runtime verification note section in `.specify/specs/049-arpg-view-actors/quickstart.md`

---

## Foundational (Blocking Prerequisites)

**Purpose**: Core runtime contracts required before user-story work.

**⚠️ CRITICAL**: User-story tasks begin only after this phase.

- [x] T004 Ensure wasm export contract includes movement/input and interaction bridge symbols in `scripts/build-engine-wasm.sh`
- [x] T005 [P] Add/refresh runtime interaction contract details in `.specify/specs/049-arpg-view-actors/contracts/interaction-contract.md`
- [x] T006 [P] Create native render framing regression test scaffold in `tests/native/engine/test_engine_render_suite.c`
- [x] T007 Create React integration sanity test scaffold for runtime bridge stability in `src/typescript/react/src/pages/__tests__/GameEnginePage.controller-arbitration.test.tsx`

**Checkpoint**: Foundation complete; user stories can proceed.

---

## User Story 1 - Centered ARPG Framing (Priority: P1) 🎯 MVP

**Goal**: Keep player and terrain in stable ARPG-centered framing during keyboard movement.

**Independent Test**: Player remains in centered follow region across movement and viewport changes with no corner anchoring.

### Tests for User Story 1

- [x] T008 [P] [US1] Add viewport-ratio camera framing assertions in `tests/native/engine/test_engine_render_suite.c`
- [x] T009 [P] [US1] Add movement-plus-resize framing behavior test in `src/typescript/react/src/pages/__tests__/GameEnginePage.controller-input.test.tsx`

### Implementation for User Story 1

- [x] T010 [US1] Implement centered follow-region camera policy in `src/native/engine/engine.c`
- [x] T011 [P] [US1] Align camera projection/view behavior for ARPG framing consistency in `src/native/engine/render/camera.c`
- [x] T012 [P] [US1] Ensure render viewport updates track runtime resize contract in `src/native/engine/render/renderer.c`
- [x] T013 [US1] Validate terrain/world centering transform contract in `src/native/engine/engine.c`
- [x] T014 [US1] Append US1 completion heartbeat evidence in `.specify/specs/049-arpg-view-actors/heartbeat-log.md`

**Checkpoint**: US1 is independently playable and testable as MVP.

---

## User Story 2 - Completed Context Menu Interactions (Priority: P1)

**Goal**: Make context menu actions target-aware and actionable with visible outcomes.

**Independent Test**: Right-click menu offers target-relevant options and at least one action mutates visible world feedback.

### Tests for User Story 2

- [x] T015 [P] [US2] Add target-aware menu behavior tests in `src/typescript/react/src/pages/__tests__/GameEnginePage.context-menu.test.tsx`
- [x] T016 [P] [US2] Add interaction dispatch contract tests in `tests/native/engine/test_engine_domain_contracts.c`

### Implementation for User Story 2

- [x] T017 [US2] Implement actor-target resolution and interaction dispatch entrypoint in `src/native/engine/engine.c`
- [x] T018 [P] [US2] Extend React context-menu state to include target/action intent mapping in `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T019 [US2] Implement actionable menu selection flow and visible feedback bridge in `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T020 [US2] Ensure no-target interaction path is non-breaking and deterministic in `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T021 [US2] Append US2 completion heartbeat evidence in `.specify/specs/049-arpg-view-actors/heartbeat-log.md`

**Checkpoint**: US2 interaction loop works independently.

---

## User Story 3 - Non-Player Actor Presence and Response (Priority: P2)

**Goal**: Populate scene with non-player actors that can be targeted and affected by interactions.

**Independent Test**: Non-player actors spawn reliably, are distinguishable, and only selected actors receive outcomes.

### Tests for User Story 3

- [x] T022 [P] [US3] Add actor spawn/visibility contract tests in `tests/native/engine/test_engine_world_suite.c`
- [x] T023 [P] [US3] Add actor-target specificity regression tests in `tests/native/engine/test_engine_ai_controller_suite.c`
- [x] T024 [P] [US3] Add React runtime actor-interaction flow test in `src/typescript/react/src/pages/__tests__/GameEnginePage.controller-arbitration.test.tsx`

### Implementation for User Story 3

- [x] T025 [US3] Add baseline non-player actor spawn setup in `src/native/engine/engine.c`
- [x] T026 [P] [US3] Define actor interaction eligibility/state transitions in `src/native/engine/engine.c`
- [x] T027 [US3] Route context action outcomes to selected actor only in `src/native/engine/engine.c`
- [x] T028 [US3] Render non-player actor distinctions for interaction readability in `src/native/engine/engine.c`
- [x] T029 [US3] Append US3 completion heartbeat evidence in `.specify/specs/049-arpg-view-actors/heartbeat-log.md`

**Checkpoint**: US3 actor loop works independently.

---

## Polish & Cross-Cutting Concerns

**Purpose**: Stabilize end-to-end behavior, docs, and quality gates.

- [x] T030 [P] Run and document local runtime validation steps in `.specify/specs/049-arpg-view-actors/quickstart.md`
- [x] T031 Run focused React behavior tests for scene/menu/actor flows in `src/typescript/react/src/pages/__tests__/GameEnginePage.context-menu.test.tsx`
- [x] T032 Run native bounded suites for render/controller/world contracts via `tests/native/engine/CMakeLists.txt`
- [x] T033 [P] Update plan-to-implementation notes with final contract deltas in `.specify/specs/049-arpg-view-actors/plan.md`
- [x] T034 Record final cross-story completion summary in `.specify/specs/049-arpg-view-actors/heartbeat-log.md`

---

## Dependencies & Execution Order

### Stage Dependencies

- Setup -> Foundational -> User Stories -> Polish
- User stories depend on Foundational completion.

### User Story Dependencies

- **US1 (P1)**: No dependency on other stories after Foundational.
- **US2 (P1)**: Depends on Foundational; may build on US1 runtime framing but remains independently testable.
- **US3 (P2)**: Depends on Foundational and uses interaction contract introduced in US2.

### Within Each User Story

- Tests first (expected to fail initially)
- Native/runtime contract implementation
- React/interop integration
- Story checkpoint heartbeat entry

### Parallel Opportunities

- T003, T005, T006, T007 can run in parallel after T001-T002.
- US1 test tasks T008-T009 can run in parallel.
- US1 implementation tasks T011-T012 can run in parallel after T010.
- US2 test tasks T015-T016 can run in parallel.
- US3 test tasks T022-T024 can run in parallel.
- Polish tasks T030 and T033 can run in parallel.

---

## Parallel Example: User Story 2

```bash
# Run US2 tests in parallel (different files)
Task: "T015 Add target-aware menu behavior tests in src/typescript/react/src/pages/__tests__/GameEnginePage.context-menu.test.tsx"
Task: "T016 Add interaction dispatch contract tests in tests/native/engine/test_engine_domain_contracts.c"

# Run independent implementation tasks in parallel after native dispatch entrypoint exists
Task: "T018 Extend React context-menu state to include target/action intent mapping in src/typescript/react/src/pages/GameEnginePage.tsx"
Task: "T020 Ensure no-target interaction path is non-breaking and deterministic in src/typescript/react/src/pages/GameEnginePage.tsx"
```

---

## Implementation Strategy

### MVP First (US1)

1. Complete Setup and Foundational phases.
2. Deliver US1 centered ARPG framing.
3. Validate US1 independently before expanding scope.

### Incremental Delivery

1. US1: centered framing and movement readability.
2. US2: actionable context menu and visible interaction outcomes.
3. US3: non-player actor population and actor-scoped responses.
4. Polish: cross-story validation and evidence capture.

### Team Parallel Strategy

1. One stream handles native camera/render contracts (US1 core).
2. One stream handles React context-menu interaction shell (US2).
3. One stream handles actor spawn/state behavior (US3) after US2 contract is stable.

---

## Notes

- Every task follows strict checklist format with Task ID and file path.
- [P] tasks are marked only where file-level independence exists.
- Story labels are applied only to user-story tasks.
- Heartbeat and confidence evidence tasks are included to satisfy orchestration requirements.