---

description: "Task list template for feature implementation"
---

# Tasks: [FEATURE NAME]

**Input**: Design documents from `.specify/specs/[###-feature-name]/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: The examples below include test tasks. Tests are OPTIONAL - only include them if explicitly requested in the feature specification.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: `src/`, `tests/` at repository root
- **Web app**: `backend/src/`, `frontend/src/`
- **Mobile**: `api/src/`, `ios/src/` or `android/src/`
- Paths shown below assume single project - adjust based on plan.md structure

<!--
  ============================================================================
  IMPORTANT: The tasks below are SAMPLE TASKS for illustration purposes only.

  The specify tasks command MUST replace these with actual tasks based on:
  - User stories from spec.md (with their priorities P1, P2, P3...)
  - Feature requirements from plan.md
  - Entities from data-model.md
  - Endpoints from contracts/

  Tasks MUST be organized by user story so each story can be:
  - Implemented independently
  - Tested independently
  - Delivered as an MVP increment

  DO NOT keep these sample tasks in the generated tasks.md file.
  ============================================================================
-->

## Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [ ] T001 Create project structure per implementation plan
- [ ] T002 Initialize [language] project with [framework] dependencies
- [ ] T003 [P] Configure linting and formatting tools
- [ ] T004 Run orchestration extension preflight (`.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`)
- [ ] T005 Record startup confidence gate + heartbeat (`.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start"`)

**Native testing note**: If the feature touches `src/native/**`, plan the test surface as bounded domain suites (for example physics, AI FSM, navigation, controller, render, world/signals) plus a thin orchestrator target. Avoid phase-based target names for new native tests.

---

## Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this stage is complete

Examples of foundational tasks (adjust based on your project):

- [ ] T006 Define domain policy module (rules/contracts) for this feature slice
- [ ] T007 [P] Define application orchestration module (flow/use cases)
- [ ] T008 [P] Define infrastructure adapter module (I/O, external command, persistence)
- [ ] T009 Create base models/entities that all stories depend on
- [ ] T010 Configure error handling and logging infrastructure
- [ ] T011 Setup environment configuration management

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## User Story 1 - [Title] (Priority: P1) 🎯 MVP

**Goal**: [Brief description of what this story delivers]

**Independent Test**: [How to verify this story works on its own]

### Tests for User Story 1 (OPTIONAL - only if tests requested) ⚠️

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [ ] T012 [P] [US1] Contract test for [endpoint] in tests/contract/test_[name].py
- [ ] T013 [P] [US1] Integration test for [user journey] in tests/integration/test_[name].py

### Implementation for User Story 1

- [ ] T014 [P] [US1] Create [Entity1] model in src/models/[entity1].py
- [ ] T015 [P] [US1] Create [Entity2] model in src/models/[entity2].py
- [ ] T016 [US1] Implement [Service] in src/services/[service].py (depends on T014, T015)
- [ ] T017 [US1] Implement [endpoint/feature] in src/[location]/[file].py
- [ ] T018 [US1] Add validation and error handling
- [ ] T019 [US1] Add logging for user story 1 operations
- [ ] T020 [US1] Append heartbeat entry for user-story completion checkpoint

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## User Story 2 - [Title] (Priority: P2)

**Goal**: [Brief description of what this story delivers]

**Independent Test**: [How to verify this story works on its own]

### Tests for User Story 2 (OPTIONAL - only if tests requested) ⚠️

- [ ] T021 [P] [US2] Contract test for [endpoint] in tests/contract/test_[name].py
- [ ] T022 [P] [US2] Integration test for [user journey] in tests/integration/test_[name].py

### Implementation for User Story 2

- [ ] T023 [P] [US2] Create [Entity] model in src/models/[entity].py
- [ ] T024 [US2] Implement [Service] in src/services/[service].py
- [ ] T025 [US2] Implement [endpoint/feature] in src/[location]/[file].py
- [ ] T026 [US2] Integrate with User Story 1 components (if needed)
- [ ] T027 [US2] Append heartbeat entry for user-story completion checkpoint

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## User Story 3 - [Title] (Priority: P3)

**Goal**: [Brief description of what this story delivers]

**Independent Test**: [How to verify this story works on its own]

### Tests for User Story 3 (OPTIONAL - only if tests requested) ⚠️

- [ ] T028 [P] [US3] Contract test for [endpoint] in tests/contract/test_[name].py
- [ ] T029 [P] [US3] Integration test for [user journey] in tests/integration/test_[name].py

### Implementation for User Story 3

- [ ] T030 [P] [US3] Create [Entity] model in src/models/[entity].py
- [ ] T031 [US3] Implement [Service] in src/services/[service].py
- [ ] T032 [US3] Implement [endpoint/feature] in src/[location]/[file].py
- [ ] T033 [US3] Append heartbeat entry for user-story completion checkpoint

**Checkpoint**: All user stories should now be independently functional

---

[Add more user story sections as needed, following the same pattern]

---

## Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [ ] TXXX [P] Documentation updates in docs/
- [ ] TXXX Code cleanup and refactoring
- [ ] TXXX Performance optimization across all stories
- [ ] TXXX [P] Additional unit tests (if requested) in tests/unit/
- [ ] TXXX Security hardening
- [ ] TXXX Run quickstart.md validation

---

## Dependencies & Execution Order

### Stage Dependencies

- **Setup**: No dependencies - can start immediately
- **Foundational**: Depends on Setup completion - BLOCKS all user stories
- **User Stories**: All depend on Foundational completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Polish**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational - No dependencies on other stories
- **User Story 2 (P2)**: Can start after Foundational - May integrate with US1 but should be independently testable
- **User Story 3 (P3)**: Can start after Foundational - May integrate with US1/US2 but should be independently testable

### Within Each User Story

- Tests (if included) MUST be written and FAIL before implementation
- Native tests should be split by bounded domain responsibility, not by implementation phase labels.
- Models before services
- Services before endpoints
- Core implementation before integration
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel
- Once Foundational completes, all user stories can start in parallel (if team capacity allows)
- All tests for a user story marked [P] can run in parallel
- Models within a story marked [P] can run in parallel
- Different user stories can be worked on in parallel by different team members

---

## Parallel Example: User Story 1

```bash
# Launch all tests for User Story 1 together (if tests requested):
Task: "Contract test for [endpoint] in tests/contract/test_[name].py"
Task: "Integration test for [user journey] in tests/integration/test_[name].py"

# Launch all models for User Story 1 together:
Task: "Create [Entity1] model in src/models/[entity1].py"
Task: "Create [Entity2] model in src/models/[entity2].py"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Setup
2. Complete Foundational (CRITICAL - blocks all stories)
3. Complete User Story 1
4. **STOP and VALIDATE**: Test User Story 1 independently
5. Deploy/demo if ready

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Deploy/Demo (MVP!)
3. Add User Story 2 → Test independently → Deploy/Demo
4. Add User Story 3 → Test independently → Deploy/Demo
5. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 1
   - Developer B: User Story 2
   - Developer C: User Story 3
3. Stories complete and integrate independently

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Verify tests fail before implementing
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- Avoid: vague tasks, same file conflicts, cross-story dependencies that break independence
