# Tasks: Specify-Driven Drift Realignment

**Input**: Design documents from `.specify/specs/004-trim-vibe-drift/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: No explicit TDD requirement was requested in the feature spec, so this task list focuses on implementation and validation workflow tasks.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish `.specify`-native orchestration entry points and workflow registration.

- [x] T001 [FR-001] Create dedicated workflow definition in .specify/workflows/drift-realignment/workflow.yml
- [x] T002 [P] [FR-010] Register drift realignment workflow in .specify/workflows/workflow-registry.json
- [x] T003 [P] [FR-010] Create workflow usage notes in .specify/workflows/drift-realignment/README.md

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core `.specify` scripts and hooks that MUST exist before user-story implementation.

**CRITICAL**: No user story work can begin until this phase is complete.

- [x] T004 [FR-001] Create orchestration entry script in .specify/scripts/bash/orchestrate-drift-realignment.sh
- [x] T005 [P] [FR-003] Create scope-boundary validator in .specify/scripts/bash/validate-spec-boundaries.sh
- [x] T006 [P] [FR-005] Create task-traceability validator in .specify/scripts/bash/validate-task-traceability.sh
- [x] T007 [FR-010] Add lifecycle hooks for drift orchestration in .specify/extensions.yml
- [x] T008 [FR-004] Document foundational `.specify` command flow in .specify/specs/004-trim-vibe-drift/quickstart.md

**Checkpoint**: Foundation ready - user story implementation can now begin.

---

## Phase 3: User Story 1 - Scope-First Feature Definition (Priority: P1) MVP

**Goal**: Ensure every feature spec has explicit scope boundaries and assumptions before implementation.

**Independent Test**: Run the `.specify` workflow for one new feature and verify generated spec content includes in-scope, out-of-scope, and assumptions sections.

### Implementation for User Story 1

- [x] T009 [P] [US1] [FR-003] Add mandatory In Scope and Out of Scope sections in .specify/templates/spec-template.md
- [x] T010 [US1] [FR-003] Add scope-boundary quality checks in .specify/templates/checklist-template.md
- [x] T011 [P] [US1] [FR-003] Add scope enforcement step in .specify/workflows/drift-realignment/workflow.yml
- [x] T012 [US1] [FR-003] Add scope-boundary checklist items in .specify/templates/checklist-template.md
- [x] T013 [US1] [FR-004] Document scope-first author flow in .specify/specs/004-trim-vibe-drift/quickstart.md

**Checkpoint**: User Story 1 is complete when scope boundaries are mandatory and visible in generated specs.

---

## Phase 4: User Story 2 - Drift Detection During Planning (Priority: P2)

**Goal**: Detect and flag planning tasks that do not map to approved requirements.

**Independent Test**: Run `.specify` orchestration with one feature and confirm non-traceable tasks are surfaced with drift categories before task approval.

### Implementation for User Story 2

- [x] T014 [P] [US2] [FR-005] Implement requirement extraction and task mapping in .specify/scripts/bash/validate-task-traceability.sh
- [x] T015 [P] [US2] [FR-006] Implement drift categorization output in .specify/scripts/bash/validate-task-traceability.sh
- [x] T016 [US2] [FR-006] Add traceability-gate step in .specify/workflows/drift-realignment/workflow.yml
- [x] T017 [US2] [FR-005] Add before_tasks hook wiring for traceability checks in .specify/extensions.yml
- [x] T018 [US2] [FR-006] Add local drift-check command examples in .specify/specs/004-trim-vibe-drift/quickstart.md

**Checkpoint**: User Story 2 is complete when drift checks can fail planning flows on non-traceable tasks.

---

## Phase 5: User Story 3 - Auditable Realignment Decisions (Priority: P3)

**Goal**: Preserve an auditable record of defer/split/reject decisions and ownership.

**Independent Test**: Execute one `.specify`-driven realignment cycle and confirm each deferred or split item has a logged decision with rationale and destination.

### Implementation for User Story 3

- [x] T019 [P] [US3] [FR-009] Create decision-log template in .specify/templates/decision-log-template.md
- [x] T020 [US3] [FR-009] Add decision recording helper in .specify/scripts/bash/record-realignment-decision.sh
- [x] T021 [US3] [FR-009] Add post-plan decision-log step in .specify/workflows/drift-realignment/workflow.yml
- [x] T022 [US3] [FR-010] Add decision-log artifact metadata in .specify/workflows/workflow-registry.json
- [x] T023 [US3] [FR-009] Align decision schema details in .specify/specs/004-trim-vibe-drift/contracts/drift-realignment-workflow.md

**Checkpoint**: User Story 3 is complete when realignment decisions are consistently recorded and reviewable.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Final hardening and parity checks for `.specify` orchestration.

- [x] T024 [P] [FR-010] Add end-to-end `.specify` workflow command sequence in .specify/workflows/drift-realignment/README.md
- [x] T025 [P] [FR-005] Add validation command matrix in .specify/specs/004-trim-vibe-drift/quickstart.md
- [x] T026 [FR-010] Add orchestration troubleshooting notes in .specify/workflows/drift-realignment/README.md
- [x] T027 [P] [FR-010] Update plan pointer verification notes in .github/copilot-instructions.md
- [x] T028 [FR-010] Add AI contract validation note for `.specify` changes in .specify/specs/004-trim-vibe-drift/quickstart.md
- [x] T029 [P] [FR-011] Add temporary wiki freeze allowlist in .specify/wiki/human-reference-allowlist.txt
- [x] T030 [FR-011][FR-012] Enforce wiki allowlist and mirror parity in scripts/wiki-consume-into-specify.sh and scripts/validate-ai-contracts.py
- [x] T031 [FR-012] Prevent wiki sync worktree writes into repository .wiki in scripts/workflow-sync-wiki.sh

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately.
- **Foundational (Phase 2)**: Depends on Setup completion - blocks all user stories.
- **User Story phases (Phase 3-5)**: Depend on Foundational completion.
- **Polish (Phase 6)**: Depends on completion of selected user stories.

### User Story Dependencies

- **US1 (P1)**: Starts after Foundational; establishes mandatory boundary inputs used by later drift checks.
- **US2 (P2)**: Starts after Foundational and should follow US1 so drift checks validate the new scope boundaries.
- **US3 (P3)**: Starts after US2 so decision logs can capture drift findings and outcomes.

### Within Each User Story

- Template and contract changes before workflow step wiring.
- Validation logic before orchestration README guidance.
- Story checkpoint validation before moving to next priority.

### Parallel Opportunities

- T002 and T003 can run in parallel during Setup.
- T005 and T006 can run in parallel during Foundational.
- T009 and T011 can run in parallel in US1.
- T014 and T015 can run in parallel in US2.
- T019 can run in parallel with early US3 workflow metadata updates.
- T024, T025, and T027 can run in parallel in Polish.

---

## Parallel Example: User Story 2

```bash
# Build drift validation logic in parallel:
Task: "Implement requirement extraction and task mapping in .specify/scripts/bash/validate-task-traceability.sh"
Task: "Implement drift categorization output in .specify/scripts/bash/validate-task-traceability.sh"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup.
2. Complete Phase 2: Foundational.
3. Complete Phase 3: User Story 1.
4. Validate that `.specify` workflow output now requires explicit scope boundaries.
5. Share as the first incremental `.specify` governance improvement.

### Incremental Delivery

1. Deliver US1 to enforce scope boundaries at intake via `.specify` templates and workflow steps.
2. Deliver US2 to automate drift detection at planning time via `.specify` validators and hooks.
3. Deliver US3 to make defer and split decisions auditable via `.specify` decision logs.
4. Finish with Phase 6 cross-cutting orchestration readiness updates.

### Parallel Team Strategy

1. One contributor handles `.specify` template updates (US1).
2. One contributor implements `.specify` validator scripts and workflow wiring (US2).
3. One contributor handles decision-log artifacts and schema alignment (US3).
4. Coordinate on checkpoints between phases to preserve traceability.

---

## Notes

- [P] tasks indicate no direct file-level conflict with sibling parallel tasks.
- [US1]/[US2]/[US3] labels preserve requirement traceability.
- Keep orchestration entry points rooted in `.specify` and treat other file changes as downstream outputs.
- Run repository validation commands documented in quickstart before PR creation.
