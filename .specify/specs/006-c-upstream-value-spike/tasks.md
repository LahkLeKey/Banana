---
description: "Task list for C Upstream Value Reassessment SPIKE"
---

# Tasks: C Upstream Value Reassessment SPIKE

**Input**: Design documents from `.specify/specs/006-c-upstream-value-spike/`
**Prerequisites**: plan.md (required), spec.md (required), research.md, data-model.md, contracts/spike-contracts.md, quickstart.md

**Tests**: No runtime test tasks are required for this planning-only SPIKE. Validation is artifact completeness and consistency checks.

**Organization**: Tasks are grouped by user story to keep each decision artifact independently reviewable.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependency)
- **[Story]**: User story mapping (`US1`, `US2`, `US3`)
- Include exact file paths in each task description

## Phase 1: Setup (Shared Discovery Scaffolding)

**Purpose**: Create reusable SPIKE output scaffolding and source references.

- [X] T001 Create SPIKE analysis workspace index in `.specify/specs/006-c-upstream-value-spike/analysis/README.md`
- [X] T002 [P] Create candidate template skeleton in `.specify/specs/006-c-upstream-value-spike/analysis/candidate-template.md`
- [X] T003 [P] Create scoring rubric worksheet in `.specify/specs/006-c-upstream-value-spike/analysis/scorecard-rubric.md`
- [X] T004 [P] Create upstream impact matrix template in `.specify/specs/006-c-upstream-value-spike/analysis/upstream-impact-template.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Establish evaluation rules and baseline inventory constraints used by all stories.

**⚠️ CRITICAL**: User story tasks start only after this phase is complete.

- [X] T005 Define candidate inclusion/exclusion criteria in `.specify/specs/006-c-upstream-value-spike/analysis/inventory-criteria.md`
- [X] T006 Define normalized scoring formula and interpretation notes in `.specify/specs/006-c-upstream-value-spike/analysis/scoring-method.md`
- [X] T007 Define confidence and uncertainty handling policy in `.specify/specs/006-c-upstream-value-spike/analysis/confidence-policy.md`
- [X] T008 Define required validation-lane mapping rules in `.specify/specs/006-c-upstream-value-spike/analysis/validation-lane-mapping-rules.md`

**Checkpoint**: Shared SPIKE method is fixed and reusable across all story outputs.

---

## Phase 3: User Story 1 - Prioritized Native Opportunity List (Priority: P1) 🎯 MVP

**Goal**: Produce a ranked list of native C opportunities with transparent scoring rationale.

**Independent Test**: Review `.specify/specs/006-c-upstream-value-spike/analysis/ranked-recommendations.md` and confirm at least 5 candidates, each with complete score dimensions and ranking notes.

### Implementation for User Story 1

- [X] T009 [US1] Build initial candidate inventory (minimum 5) in `.specify/specs/006-c-upstream-value-spike/analysis/candidate-inventory.md`
- [X] T010 [US1] Map each candidate to native surfaces in `.specify/specs/006-c-upstream-value-spike/analysis/native-surface-map.md`
- [X] T011 [US1] Score all candidates using normalized rubric in `.specify/specs/006-c-upstream-value-spike/analysis/candidate-scorecards.md`
- [X] T012 [US1] Produce ranked recommendation table in `.specify/specs/006-c-upstream-value-spike/analysis/ranked-recommendations.md`
- [X] T013 [US1] Document ranking rationale and tie-break decisions in `.specify/specs/006-c-upstream-value-spike/analysis/ranking-rationale.md`

**Checkpoint**: A complete ranked list exists and can be reviewed without additional discovery.

---

## Phase 4: User Story 2 - Follow-Up Story Readiness Packet (Priority: P2)

**Goal**: Produce story-ready handoff packets for top candidates.

**Independent Test**: Review `.specify/specs/006-c-upstream-value-spike/analysis/readiness-packets.md` and confirm top 3 candidates each contain bounded scope, acceptance intent, and validation expectations.

### Implementation for User Story 2

- [X] T014 [US2] Select top 3 candidates from ranked list and record selection basis in `.specify/specs/006-c-upstream-value-spike/analysis/top-candidate-selection.md`
- [X] T015 [P] [US2] Draft readiness packet for top candidate #1 in `.specify/specs/006-c-upstream-value-spike/analysis/readiness-packet-1.md`
- [X] T016 [P] [US2] Draft readiness packet for top candidate #2 in `.specify/specs/006-c-upstream-value-spike/analysis/readiness-packet-2.md`
- [X] T017 [P] [US2] Draft readiness packet for top candidate #3 in `.specify/specs/006-c-upstream-value-spike/analysis/readiness-packet-3.md`
- [X] T018 [US2] Consolidate top-3 packets into handoff bundle in `.specify/specs/006-c-upstream-value-spike/analysis/readiness-packets.md`
- [X] T019 [US2] Capture unresolved questions and mitigations in `.specify/specs/006-c-upstream-value-spike/analysis/readiness-open-questions.md`

**Checkpoint**: Follow-up story drafting can start directly from SPIKE handoff artifacts.

---

## Phase 5: User Story 3 - Upstream Impact Transparency (Priority: P3)

**Goal**: Make cross-domain impact and validation obligations explicit for top candidates.

**Independent Test**: Review `.specify/specs/006-c-upstream-value-spike/analysis/upstream-impact-matrix.md` and confirm each top candidate includes impacted domains, contract surfaces, and required validation lanes.

### Implementation for User Story 3

- [X] T020 [US3] Build top-candidate upstream domain impact matrix in `.specify/specs/006-c-upstream-value-spike/analysis/upstream-impact-matrix.md`
- [X] T021 [US3] Document contract/env assumptions impacted per top candidate in `.specify/specs/006-c-upstream-value-spike/analysis/contract-impact-notes.md`
- [X] T022 [US3] Define per-candidate validation lane expectations in `.specify/specs/006-c-upstream-value-spike/analysis/validation-lane-plan.md`
- [X] T023 [US3] Record cross-domain dependency and sequencing notes in `.specify/specs/006-c-upstream-value-spike/analysis/cross-domain-sequencing.md`
- [X] T024 [US3] Summarize residual release risks in `.specify/specs/006-c-upstream-value-spike/analysis/residual-risk-summary.md`

**Checkpoint**: Upstream impact is explicit and review-ready for delivery planning.

---

## Phase 6: Polish & Cross-Cutting Completion

**Purpose**: Final consistency, traceability, and closure updates.

- [X] T025 Reconcile terminology consistency across all 006 artifacts in `.specify/specs/006-c-upstream-value-spike/`
- [X] T026 [P] Cross-check `spec.md` success criteria against generated outputs and record evidence in `.specify/specs/006-c-upstream-value-spike/analysis/success-criteria-evidence.md`
- [X] T027 [P] Update `.specify/specs/006-c-upstream-value-spike/quickstart.md` with exact artifact review commands actually used
- [X] T028 Update `.specify/specs/006-c-upstream-value-spike/contracts/spike-contracts.md` if any deliverable contract detail changed during execution
- [X] T029 Run artifact completeness check by confirming expected files listed in `.specify/specs/006-c-upstream-value-spike/analysis/README.md`
- [X] T030 Mark completion evidence and task statuses in `.specify/specs/006-c-upstream-value-spike/tasks.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Phase 1; blocks all user stories.
- **Phase 3 (US1)**: Depends on Phase 2.
- **Phase 4 (US2)**: Depends on Phase 3 ranked results.
- **Phase 5 (US3)**: Depends on top-candidate selection from Phase 4.
- **Phase 6 (Polish)**: Depends on all user-story phases.

### User Story Dependencies

- **US1 (P1)**: First deliverable and MVP for candidate selection.
- **US2 (P2)**: Uses US1 ranking output to produce story-ready packets.
- **US3 (P3)**: Uses US1/US2 outputs to expose cross-domain impact and validation.

### Parallel Opportunities

- T002/T003/T004 can run in parallel.
- T015/T016/T017 can run in parallel after T014.
- T026/T027/T028 can run in parallel during final polish.

---

## Parallel Example: User Story 2

```bash
# After selecting top 3 candidates (T014), draft packets in parallel:
Task: "Draft readiness packet for top candidate #1 in .specify/specs/006-c-upstream-value-spike/analysis/readiness-packet-1.md"
Task: "Draft readiness packet for top candidate #2 in .specify/specs/006-c-upstream-value-spike/analysis/readiness-packet-2.md"
Task: "Draft readiness packet for top candidate #3 in .specify/specs/006-c-upstream-value-spike/analysis/readiness-packet-3.md"
```

---

## Implementation Strategy

### MVP First (US1 Only)

1. Complete Phase 1 and Phase 2.
2. Complete Phase 3 (US1).
3. Validate ranked candidate output for immediate decision support.

### Incremental Delivery

1. Deliver US1 ranked opportunities.
2. Deliver US2 story-ready packets for top 3 candidates.
3. Deliver US3 upstream impact/validation transparency.
4. Finish with Phase 6 consistency and evidence closure.

### Notes

- This SPIKE is planning-only: no production code behavior changes are required.
- Story outputs should be reviewable independently at each checkpoint.
- Keep file paths and artifact names stable to preserve traceability into follow-up stories.

## Completion Evidence (2026-04-26)

- US1 artifacts complete: candidate inventory, surface map, scorecards, ranked recommendations, ranking rationale.
- US2 artifacts complete: top-candidate selection, three readiness packets, consolidated packet bundle, open questions.
- US3 artifacts complete: upstream impact matrix, contract impact notes, validation lane plan, sequencing notes, residual risk summary.
- Polish artifacts complete: success criteria evidence, quickstart execution commands, contract reconciliation note.
- Artifact completeness gate: `ANALYSIS_COMPLETENESS=PASS`.
