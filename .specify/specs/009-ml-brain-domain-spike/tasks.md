---
description: "Task list for ML Brain Domain SPIKE"
---

# Tasks: ML Brain Domain SPIKE

**Input**: Design documents from `.specify/specs/009-ml-brain-domain-spike/`
**Prerequisites**: plan.md (required), spec.md (required), research.md, data-model.md, contracts/ml-brain-domain-contract.md, quickstart.md

**Tests**: This SPIKE requires artifact-level verification gates and validation-lane mapping evidence.

**Organization**: Tasks are grouped by user story so each story can be completed and reviewed independently.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: User story mapping (`US1`, `US2`, `US3`)
- Include exact file paths in each task description

## Phase 1: Setup (Shared Scope)

**Purpose**: Create SPIKE workspace scaffolding and artifact index.

- [ ] T001 Create SPIKE analysis index in `.specify/specs/009-ml-brain-domain-spike/analysis/README.md`
- [ ] T002 [P] Capture in-scope native/wrapper ML files in `.specify/specs/009-ml-brain-domain-spike/analysis/in-scope-files.md`
- [ ] T003 [P] Capture validation lane command references in `.specify/specs/009-ml-brain-domain-spike/analysis/validation-commands.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Define normalized method and scoring/risk conventions for all stories.

**⚠️ CRITICAL**: User story tasks start only after this phase is complete.

- [ ] T004 Define canonical terminology rules in `.specify/specs/009-ml-brain-domain-spike/analysis/terminology-rules.md`
- [ ] T005 Define shared comparison dimensions in `.specify/specs/009-ml-brain-domain-spike/analysis/comparison-dimensions.md`
- [ ] T006 Define confidence and uncertainty policy in `.specify/specs/009-ml-brain-domain-spike/analysis/confidence-policy.md`

**Checkpoint**: Shared SPIKE method is fixed and reusable.

---

## Phase 3: User Story 1 - Canonical Domain Definitions (Priority: P1) 🎯 MVP

**Goal**: Establish non-overlapping Left/Right/Full brain definitions and primary family mappings.

**Independent Test**: Review domain definitions and confirm each family has one primary mapping with rationale.

### Tests for User Story 1

- [ ] T007 [P] [US1] Add domain-definition review checklist in `.specify/specs/009-ml-brain-domain-spike/analysis/domain-definition-checklist.md`

### Implementation for User Story 1

- [ ] T008 [US1] Draft Left/Right/Full domain definitions in `.specify/specs/009-ml-brain-domain-spike/analysis/brain-domain-definitions.md`
- [ ] T009 [US1] Create primary model-family mappings in `.specify/specs/009-ml-brain-domain-spike/analysis/model-family-mappings.md`
- [ ] T010 [US1] Document domain boundaries and out-of-domain rules in `.specify/specs/009-ml-brain-domain-spike/analysis/domain-boundaries.md`
- [ ] T011 [US1] Record known domain risks in `.specify/specs/009-ml-brain-domain-spike/analysis/domain-risk-register.md`

**Checkpoint**: Canonical domain vocabulary and primary mapping are complete.

---

## Phase 4: User Story 2 - Domain-to-Model Mapping and Tradeoff Matrix (Priority: P2)

**Goal**: Produce normalized cross-family tradeoff matrix and ranking rationale.

**Independent Test**: Validate matrix covers regression, binary, transformer across all normalized dimensions.

### Tests for User Story 2

- [ ] T012 [P] [US2] Add matrix completeness checklist in `.specify/specs/009-ml-brain-domain-spike/analysis/matrix-completeness-checklist.md`

### Implementation for User Story 2

- [ ] T013 [US2] Build normalized comparison matrix in `.specify/specs/009-ml-brain-domain-spike/analysis/model-family-tradeoff-matrix.md`
- [ ] T014 [US2] Score and rank model-family/domain fit in `.specify/specs/009-ml-brain-domain-spike/analysis/domain-fit-scorecards.md`
- [ ] T015 [US2] Document ranking rationale and tie-break policy in `.specify/specs/009-ml-brain-domain-spike/analysis/ranking-rationale.md`
- [ ] T016 [US2] Capture uncertainty impacts on ranking in `.specify/specs/009-ml-brain-domain-spike/analysis/uncertainty-impact-notes.md`

**Checkpoint**: Comparison and ranking outputs are review-ready.

---

## Phase 5: User Story 3 - Follow-Up Story Readiness for ML Domains (Priority: P3)

**Goal**: Convert SPIKE outputs into actionable follow-up packets per domain.

**Independent Test**: Confirm at least one bounded readiness packet exists for left, right, and full domains with validation expectations.

### Tests for User Story 3

- [ ] T017 [P] [US3] Add readiness-packet quality checklist in `.specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-checklist.md`

### Implementation for User Story 3

- [ ] T018 [US3] Draft Left Brain readiness packet in `.specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-left-brain.md`
- [ ] T019 [US3] Draft Right Brain readiness packet in `.specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-right-brain.md`
- [ ] T020 [US3] Draft Full Brain readiness packet in `.specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-full-brain.md`
- [ ] T021 [US3] Consolidate packet bundle in `.specify/specs/009-ml-brain-domain-spike/analysis/readiness-packets.md`
- [ ] T022 [US3] Define lane expectations and contract impacts in `.specify/specs/009-ml-brain-domain-spike/analysis/validation-lane-plan.md`

**Checkpoint**: Follow-up implementation candidates are selection-ready without rediscovery meetings.

---

## Phase 6: Polish & Cross-Cutting Completion

**Purpose**: Final consistency, success-criteria evidence, and closure tracking.

- [ ] T023 Reconcile terminology consistency across `.specify/specs/009-ml-brain-domain-spike/`
- [ ] T024 [P] Cross-check success criteria and record evidence in `.specify/specs/009-ml-brain-domain-spike/analysis/success-criteria-evidence.md`
- [ ] T025 [P] Update `.specify/specs/009-ml-brain-domain-spike/quickstart.md` with exact execution review commands used
- [ ] T026 Verify analysis artifact completeness in `.specify/specs/009-ml-brain-domain-spike/analysis/`
- [ ] T027 Mark completion evidence and task statuses in `.specify/specs/009-ml-brain-domain-spike/tasks.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Setup; blocks all user stories.
- **Phase 3 (US1)**: Depends on Foundational.
- **Phase 4 (US2)**: Depends on US1 canonical definitions.
- **Phase 5 (US3)**: Depends on US1 and US2 outputs.
- **Phase 6 (Polish)**: Depends on all user-story phases.

### User Story Dependencies

- **US1 (P1)**: MVP and semantic contract foundation.
- **US2 (P2)**: Requires US1 terms and mappings.
- **US3 (P3)**: Requires US1/US2 artifacts for packetization.

### Parallel Opportunities

- T002 and T003 can run in parallel.
- T007 and T012 can be prepared in parallel once foundational tasks complete.
- T018, T019, and T020 can run in parallel.
- T024 and T025 can run in parallel.

---

## Implementation Strategy

### MVP First (US1 Only)

1. Complete Phase 1 and Phase 2.
2. Complete Phase 3 (US1).
3. Validate canonical mapping agreement before proceeding.

### Incremental Delivery

1. Deliver US1 definitions and mappings.
2. Deliver US2 tradeoff matrix and ranking outputs.
3. Deliver US3 readiness packets and lane mappings.
4. Complete final polish and closure updates.

### Notes

- This SPIKE is planning-focused and does not change production behavior.
- Preserve native/wrapper contract boundaries while evaluating domain strategy.
- Keep follow-up recommendations bounded and implementation-ready.
