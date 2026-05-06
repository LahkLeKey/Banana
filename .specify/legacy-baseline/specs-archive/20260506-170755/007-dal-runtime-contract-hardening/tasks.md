---
description: "Task list for DAL Runtime Contract Hardening"
---

# Tasks: DAL Runtime Contract Hardening

**Input**: Design documents from `.specify/specs/007-dal-runtime-contract-hardening/`
**Prerequisites**: plan.md (required), spec.md (required), research.md, data-model.md, contracts/dal-runtime-contracts.md, quickstart.md

**Tests**: This feature requires explicit native, unit, and contract-lane validation tasks.

**Organization**: Tasks are grouped by user story so each story can be implemented and verified independently.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependency)
- **[Story]**: User story mapping (`US1`, `US2`, `US3`)
- Include exact file paths in each task description

## Phase 1: Setup (Shared Scope)

**Purpose**: Confirm bounded feature scope and validation entry points.

- [X] T001 Create 007 implementation notes index in `.specify/specs/007-dal-runtime-contract-hardening/README.md`
- [X] T002 [P] Capture in-scope DAL files list in `.specify/specs/007-dal-runtime-contract-hardening/analysis/in-scope-files.md`
- [X] T003 [P] Capture required validation command set in `.specify/specs/007-dal-runtime-contract-hardening/analysis/validation-commands.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Define deterministic DAL outcome matrix and status mapping expectations used by all stories.

**⚠️ CRITICAL**: User story tasks start only after this phase is complete.

- [X] T004 Define DAL outcome matrix in `.specify/specs/007-dal-runtime-contract-hardening/analysis/dal-outcome-matrix.md`
- [X] T005 Define native-to-managed status mapping matrix in `.specify/specs/007-dal-runtime-contract-hardening/analysis/status-mapping-matrix.md`
- [X] T006 Define remediation message expectations in `.specify/specs/007-dal-runtime-contract-hardening/analysis/remediation-expectations.md`

**Checkpoint**: Outcome categories, mapping expectations, and remediation rules are fixed.

---

## Phase 3: User Story 1 - Deterministic DAL Failure Outcomes (Priority: P1) 🎯 MVP

**Goal**: Ensure DAL non-success conditions in scope return explicit deterministic non-success outcomes.

**Independent Test**: Trigger unconfigured, dependency-unavailable, and query-failure DAL paths and confirm deterministic non-success behavior with no synthetic success payloads.

### Tests for User Story 1

- [X] T007 [P] [US1] Add native DAL contract tests in `tests/native/test_dal_contracts.c`
- [X] T008 [P] [US1] Add unit tests for DAL-focused status mapping outcomes in `tests/unit/StatusMappingTests.cs`

### Implementation for User Story 1

- [X] T009 [US1] Harden unconfigured DAL gating in `src/native/core/dal/banana_dal.c`
- [X] T010 [US1] Harden dependency-unavailable DAL behavior in `src/native/core/dal/banana_dal.c`
- [X] T011 [US1] Harden query-failure DAL behavior in `src/native/core/dal/banana_dal.c`
- [X] T012 [US1] Update DAL remediation message handling in `src/native/core/banana_status.c`
- [X] T013 [US1] Run `tests/native` and fix failing DAL scenarios in `tests/native/test_dal_contracts.c`
- [X] T014 [US1] Run targeted unit mapping tests and fix failures in `tests/unit/StatusMappingTests.cs`

**Checkpoint**: DAL failure outcomes are deterministic and non-success in all in-scope conditions.

---

## Phase 4: User Story 2 - Stable Upstream Error Contract (Priority: P2)

**Goal**: Keep ASP.NET mapping aligned with hardened native DAL outcomes while preserving success-path compatibility.

**Independent Test**: Verify deterministic status/body mapping for in-scope DAL outcome categories and compatible success behavior.

### Tests for User Story 2

- [X] T015 [P] [US2] Add/extend DAL mapping assertions in `tests/unit/StatusMappingTests.cs`
- [X] T016 [P] [US2] Add interop seam behavior assertions in `tests/unit/NativeBananaClientTests.cs`
- [X] T017 [P] [US2] Add contract-lane DAL mapping assertions in `tests/e2e/Contracts/E2eRunnerContractTests.cs`

### Implementation for User Story 2

- [X] T018 [US2] Align status mapping outcomes in `src/c-sharp/asp.net/Pipeline/StatusMapping.cs`
- [X] T019 [US2] Align managed status enum if required in `src/c-sharp/asp.net/NativeInterop/NativeStatusCode.cs`
- [X] T020 [US2] Update native status contract declarations if required in `src/native/wrapper/banana_wrapper.h`
- [X] T021 [US2] Verify interop/native status parity in `src/c-sharp/asp.net/NativeInterop/INativeBananaClient.cs` and `src/c-sharp/asp.net/NativeInterop/NativeMethods.cs`
- [X] T022 [US2] Run targeted unit tests for status and interop behavior (`StatusMappingTests`, `NativeBananaClientTests`)
- [X] T023 [US2] Run contract lane tests (`dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"`) and resolve regressions

**Checkpoint**: Managed mapping contract is deterministic and consistent with hardened native outcomes.

---

## Phase 5: User Story 3 - Follow-Up Readiness Validation Closure (Priority: P3)

**Goal**: Ensure required lanes provide review-ready evidence for DAL hardening release readiness.

**Independent Test**: Execute required native/unit/contract lanes and produce evidence mapping lane outcomes to acceptance scenarios.

### Tests for User Story 3

- [X] T024 [P] [US3] Add DAL lane evidence template in `.specify/specs/007-dal-runtime-contract-hardening/analysis/lane-evidence-template.md`

### Implementation for User Story 3

- [X] T025 [US3] Run required native validation command and record evidence in `.specify/specs/007-dal-runtime-contract-hardening/analysis/native-lane-evidence.md`
- [X] T026 [US3] Run required unit validation command and record evidence in `.specify/specs/007-dal-runtime-contract-hardening/analysis/unit-lane-evidence.md`
- [X] T027 [US3] Run required contract-lane command and record evidence in `.specify/specs/007-dal-runtime-contract-hardening/analysis/contract-lane-evidence.md`
- [X] T028 [US3] Consolidate lane evidence into `.specify/specs/007-dal-runtime-contract-hardening/analysis/validation-summary.md`

**Checkpoint**: Validation evidence is available and maps to acceptance scenarios without rediscovery.

---

## Phase 6: Polish & Cross-Cutting Completion

**Purpose**: Final consistency, traceability, and closure updates.

- [X] T029 Reconcile terminology and status naming consistency across `.specify/specs/007-dal-runtime-contract-hardening/`
- [X] T030 [P] Update `.specify/specs/007-dal-runtime-contract-hardening/quickstart.md` with exact commands used during execution
- [X] T031 [P] Update `.specify/specs/007-dal-runtime-contract-hardening/contracts/dal-runtime-contracts.md` if implementation changed contract details
- [X] T032 Verify all expected analysis/evidence files exist in `.specify/specs/007-dal-runtime-contract-hardening/analysis/`
- [X] T033 Mark completion evidence and task statuses in `.specify/specs/007-dal-runtime-contract-hardening/tasks.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Phase 1; blocks all user stories.
- **Phase 3 (US1)**: Depends on Phase 2.
- **Phase 4 (US2)**: Depends on US1 DAL outcome hardening.
- **Phase 5 (US3)**: Depends on US1 and US2 validation-ready behavior.
- **Phase 6 (Polish)**: Depends on all user-story phases.

### User Story Dependencies

- **US1 (P1)**: MVP and highest risk-reduction slice.
- **US2 (P2)**: Requires hardened native outcomes from US1.
- **US3 (P3)**: Requires executable validation outputs from US1/US2.

### Parallel Opportunities

- T002 and T003 can run in parallel.
- T007 and T008 can run in parallel.
- T015, T016, and T017 can run in parallel.
- T030 and T031 can run in parallel.

---

## Implementation Strategy

### MVP First (US1 Only)

1. Complete Phase 1 and Phase 2.
2. Complete Phase 3 (US1).
3. Validate deterministic DAL non-success behavior before expanding scope.

### Incremental Delivery

1. Deliver US1 hardened DAL outcomes.
2. Deliver US2 managed mapping and contract stability.
3. Deliver US3 lane evidence package.
4. Complete final polish and closure tasks.

### Notes

- Keep scope bounded to DAL contract hardening.
- Preserve cross-layer contract alignment before adding new status categories.
- Prefer smallest sufficient validation surface for each increment.

## Completion Evidence (2026-04-26)

- Managed DAL contract hardening updates implemented and validated in targeted unit and contract lanes.
- Native DAL contract test added and registered; native lane passed via preset-driven CMake CLI (`4/4 native tests passed`, including `banana_test_dal_contracts`).
- CMake triage updates applied in-repo (`CMakePresets.json`, root preset settings, conflicting tests-folder override removal); CMake Tools session still reports no active preset, but feature validation is complete via CLI evidence.
- Reconciliation evidence recorded in `analysis/dal-implementation-reconciliation.md` and `analysis/terminology-reconciliation.md`.
- Analysis completeness check output: `ANALYSIS_007_COMPLETENESS=PASS`.
- Remaining execution blockers: none for feature 007 validation scope.
