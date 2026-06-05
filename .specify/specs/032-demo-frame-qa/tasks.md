---

description: "Task list for Demo Frame QA Evidence"
---

# Tasks: Demo Frame QA Evidence

**Input**: Design documents from `/specs/032-demo-frame-qa/`

**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Only the validation explicitly called out in plan.md is included below.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [X] T001 Create artifacts output placeholder in artifacts/native/032-demo-frame-qa/.gitkeep

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**CRITICAL**: No user story work can begin until this phase is complete

- [X] T002 Create demo frame export config + structs in src/native/engine/runtime/engine/demo_frame_export.h
- [X] T003 Implement export scaffolding with BANANA_DEMO_FRAME_EXPORT and output dir parsing in src/native/engine/runtime/engine/demo_frame_export.c
- [X] T004 Update frame buffer accessors for export in src/native/engine/render/renderer.h and src/native/engine/render/renderer.c
- [X] T005 Add capture context plumbing (scenario, tick, scene info) in src/native/engine/runtime/engine/engine_host.h

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Visual QA evidence from demo tests (Priority: P1) MVP

**Goal**: Export demo frames with contextual metadata for human review.

**Independent Test**: Run a demo test with export enabled and confirm a review-ready bundle exists with metadata.

### Tests for User Story 1 (OPTIONAL - only if tests requested)

- [X] T006 [P] [US1] Add bundle validation script in tests/native/feedback/validate_demo_frame_bundle.sh
- [X] T007 [US1] Add Windows-only CTest smoke in tests/native/CMakeLists.txt to run banana_engine_win32_dx12_poc autotest with export enabled and call validate_demo_frame_bundle.sh

### Implementation for User Story 1

- [X] T008 [US1] Implement BMP writer plus manifest/per-frame metadata output in src/native/engine/runtime/engine/demo_frame_export.c
- [X] T009 [P] [US1] Integrate export helper into src/native/engine/win32_dx12_poc/app/main.c autotest path and propagate export failures
- [X] T010 [P] [US1] Populate scene key/variant context in src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c for export metadata
- [X] T011 [P] [US1] Integrate export helper into tests/native/feedback/native_feedback_loop_factory.c and propagate export failures

**Checkpoint**: User Story 1 is fully functional and independently testable

---

## Phase 4: User Story 2 - Developers control visual capture scope (Priority: P2)

**Goal**: Allow configurable capture cadence and scenario scope for demo runs.

**Independent Test**: Configure a run with a subset of scenarios and confirm only those frames are exported at the configured cadence.

### Implementation for User Story 2

- [X] T012 [US2] Add BANANA_DEMO_FRAME_INTERVAL and BANANA_DEMO_FRAME_FORMAT overrides in src/native/engine/runtime/engine/demo_frame_export.c
- [X] T013 [P] [US2] Update scripts/run-war-test-suites.sh to pass BANANA_DEMO_FRAME_* envs and respect scenario scope
- [X] T014 [P] [US2] Update scripts/run-native-feedback-loop.sh to pass BANANA_DEMO_FRAME_* envs for scenario runs
- [X] T015 [P] [US2] Update scripts/refresh-coherent-world-evidence.sh to emit bundles under artifacts/native/032-demo-frame-qa

**Checkpoint**: User Stories 1 and 2 are independently testable

---

## Phase 5: User Story 3 - Human QA review workflow exists (Priority: P3)

**Goal**: Provide a documented, repeatable QA review workflow and template.

**Independent Test**: Complete a review record using the checklist and template for a new bundle.

### Implementation for User Story 3

- [X] T016 [P] [US3] Add review record template in .specify/specs/032-demo-frame-qa/contracts/demo-frame-qa-review-record.md
- [X] T017 [US3] Update .specify/specs/032-demo-frame-qa/contracts/demo-frame-qa.md with retention/cleanup expectations and link to the template
- [X] T018 [US3] Update .specify/specs/032-demo-frame-qa/quickstart.md to reference the checklist, template, and review record location

**Checkpoint**: All user stories are now independently functional

---

## Phase N: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [X] T019 [P] Validate quickstart commands and update .specify/specs/032-demo-frame-qa/quickstart.md if any corrections are needed

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational phase completion
- **Polish (Final Phase)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2)
- **User Story 2 (P2)**: Can start after Foundational (Phase 2)
- **User Story 3 (P3)**: Can start after Foundational (Phase 2)

### Within Each User Story

- Tests (if included) must be completed before implementation changes they validate
- Export helper changes should land before caller integration
- Story complete before moving to next priority

### Parallel Opportunities

- T006 can run in parallel with T008 once Foundational tasks are complete
- T009, T010, and T011 can proceed in parallel after T008
- T013, T014, and T015 can proceed in parallel after T012

---

## Parallel Example: User Story 1

```bash
# After T008 is complete:
Task: "Integrate export helper into src/native/engine/win32_dx12_poc/app/main.c autotest path and propagate export failures"
Task: "Populate scene key/variant context in src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c for export metadata"
Task: "Integrate export helper into tests/native/feedback/native_feedback_loop_factory.c and propagate export failures"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (blocks all stories)
3. Complete Phase 3: User Story 1
4. Validate User Story 1 independently

### Incremental Delivery

1. Setup + Foundational
2. User Story 1 -> validate
3. User Story 2 -> validate
4. User Story 3 -> validate
5. Polish & cross-cutting updates

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Quickstart validation should remain lightweight and non-destructive
