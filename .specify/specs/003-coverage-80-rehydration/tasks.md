# Tasks: Coverage 80 Rehydration for Entire Monorepo

**Input**: Design documents from `.specify/specs/003-coverage-80-rehydration/`  
**Prerequisites**: `plan.md`, `spec.md`, `research.md`, `data-model.md`, `contracts/coverage-lane-contract.md`

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish shared coverage-contract scaffolding and artifact conventions for all layers.

- [x] T001 Define shared coverage artifact roots and normalized output paths in .github/workflows/compose-ci.yml
- [x] T002 [P] Add coverage normalization helper scaffold in scripts/coverage-normalize-lane-result.sh
- [x] T003 [P] Add aggregate summary generator scaffold in scripts/coverage-aggregate-summary.sh
- [x] T004 [P] Add denominator policy loader scaffold in scripts/coverage-denominator-policy.sh
- [x] T005 [P] Add exception record validator scaffold in scripts/coverage-validate-exceptions.sh

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Implement contract and validation foundations required before story slices.

**CRITICAL**: No user story work starts before this phase is complete.

- [x] T006 Implement normalized lane result schema validation in scripts/validate-compose-ci-lane-contract.sh
- [x] T007 [P] Add coverage failure taxonomy mapping in scripts/coverage-normalize-lane-result.sh
- [x] T008 [P] Add denominator policy snapshot emission in scripts/coverage-denominator-policy.sh
- [x] T009 [P] Add tuple applicability (`not-applicable`) resolver in scripts/coverage-denominator-policy.sh
- [x] T010 Wire shared coverage helpers into merge-gated workflow skeleton in .github/workflows/compose-ci.yml
- [x] T011 Add aggregate summary publication step (always-on) in .github/workflows/compose-ci.yml

**Checkpoint**: Contract foundations and schema enforcement are in place.

---

## Phase 3: User Story 1 - Keep unit coverage merge-safe (Priority: P1) MVP

**Goal**: Every applicable unit lane enforces 80 percent minimum with deterministic pass/fail behavior.

**Independent Test**: Introduce a controlled unit coverage drop and verify attributed deterministic lane failure.

### Implementation for User Story 1

- [x] T012 [US1] Integrate native unit coverage normalization in scripts/run-native-c-tests-with-coverage.sh
- [x] T013 [US1] Integrate .NET unit coverage normalization in scripts/run-tests-with-coverage.sh
- [x] T014 [P] [US1] Integrate TypeScript API unit coverage normalization in scripts/run-tests-with-coverage.sh
- [x] T015 [P] [US1] Integrate React unit coverage normalization in scripts/run-tests-with-coverage.sh
- [x] T016 [P] [US1] Integrate Electron unit coverage normalization in scripts/run-tests-with-coverage.sh
- [x] T017 [P] [US1] Integrate React Native unit coverage normalization in scripts/run-tests-with-coverage.sh
- [x] T018 [P] [US1] Integrate shared UI unit coverage normalization in scripts/run-tests-with-coverage.sh
- [x] T019 [US1] Add unit-lane threshold enforcement wiring in .github/workflows/compose-ci.yml
- [x] T020 [US1] Publish unit tuple statuses in aggregate summary in scripts/coverage-aggregate-summary.sh

**Checkpoint**: Unit layer coverage enforcement is deterministic and attributable.

---

## Phase 4: User Story 2 - Protect integration contracts with 80 percent floor (Priority: P1)

**Goal**: Integration lanes enforce 80 percent minimum with clear cross-layer attribution.

**Independent Test**: Remove integration scenario coverage and verify threshold failure classification.

### Implementation for User Story 2

- [x] T021 [US2] Normalize native integration coverage including DAL paths in scripts/run-native-c-tests-with-coverage.sh
- [x] T022 [US2] Normalize ASP.NET integration coverage for controller-service-pipeline surfaces in scripts/run-tests-with-coverage.sh
- [x] T023 [US2] Normalize TypeScript API integration coverage in scripts/run-tests-with-coverage.sh
- [x] T024 [P] [US2] Add cross-layer interop coverage entry-point attribution in scripts/coverage-normalize-lane-result.sh
- [x] T025 [US2] Enforce integration tuple threshold gates in .github/workflows/compose-ci.yml
- [x] T026 [US2] Enforce preflight contract classification for integration dependencies in scripts/compose-ci-preflight.sh
- [x] T027 [US2] Publish integration tuple statuses in aggregate summary in scripts/coverage-aggregate-summary.sh

**Checkpoint**: Integration layer gates preserve cross-layer contract confidence.

---

## Phase 5: User Story 3 - Keep e2e runtime channels at 80 percent exercised surface (Priority: P1)

**Goal**: E2e lanes for API+React, Electron, and mobile channels enforce 80 percent flow-based denominator.

**Independent Test**: Remove declared flow coverage and verify flow-attributed threshold failures.

### Implementation for User Story 3

- [x] T028 [US3] Define machine-readable e2e flow manifest format in .specify/specs/003-coverage-80-rehydration/contracts/coverage-lane-contract.md
- [x] T029 [US3] Add API+React e2e flow manifest and normalization mapping in scripts/coverage-normalize-lane-result.sh
- [x] T030 [P] [US3] Add Electron e2e flow manifest and normalization mapping in scripts/coverage-normalize-lane-result.sh
- [x] T031 [P] [US3] Add mobile e2e flow manifest and normalization mapping in scripts/coverage-normalize-lane-result.sh
- [x] T032 [US3] Add e2e retry-budget classification (`flake_contract_violation`) in .github/workflows/compose-ci.yml
- [x] T033 [US3] Enforce e2e tuple threshold gates in .github/workflows/compose-ci.yml
- [x] T034 [US3] Publish e2e tuple statuses in aggregate summary in scripts/coverage-aggregate-summary.sh

**Checkpoint**: E2e coverage gating is denominator-driven and channel-attributable.

---

## Phase 6: User Story 4 - Keep coverage evidence one-pass diagnosable (Priority: P1)

**Goal**: Failures across any lanes are diagnosable in one run without reruns.

**Independent Test**: Trigger multi-lane failures and confirm aggregate + lane evidence completeness.

### Implementation for User Story 4

- [x] T035 [US4] Add failing-lanes-first ordering to human summary generation in scripts/coverage-aggregate-summary.sh
- [x] T036 [US4] Include failure class, threshold, measured value, and deficit in lane summaries in scripts/coverage-normalize-lane-result.sh
- [x] T037 [P] [US4] Ensure fallback evidence bundle generation for pre-publication failures in .github/workflows/compose-ci.yml
- [x] T038 [US4] Add machine-readable aggregate artifact upload and retention in .github/workflows/compose-ci.yml
- [x] T039 [US4] Add one-pass triage checklist section in README.md

**Checkpoint**: First-pass triage is sufficient for all failing tuple paths.

---

## Phase 7: User Story 5 - Govern temporary exceptions without weakening baseline (Priority: P2)

**Goal**: Exceptions are temporary, accountable, and never silently suppress invalid failures.

**Independent Test**: Validate active, malformed, and expired exception behavior.

### Implementation for User Story 5

- [x] T040 [US5] Implement exception metadata schema checks in scripts/coverage-validate-exceptions.sh
- [x] T041 [US5] Wire exception evaluation into lane normalization pipeline in scripts/coverage-normalize-lane-result.sh
- [x] T042 [P] [US5] Enforce expiry behavior and invalid-exception fallback to baseline in scripts/coverage-validate-exceptions.sh
- [x] T043 [US5] Surface active exceptions in lane and aggregate summaries in scripts/coverage-aggregate-summary.sh
- [x] T044 [US5] Add exception governance operator guide in .specify/wiki/coverage-80-exceptions.md
- [x] T045 [US5] Add human-facing exception remediation runbook in .wiki/coverage-80-exceptions.md

**Checkpoint**: Exception governance is strict, visible, and time-bounded.

---

## Phase 8: User Story 6 - Keep path portability and contract parity with 002 (Priority: P2)

**Goal**: Coverage evidence remains portable and contract-aligned with 002 across local and CI.

**Independent Test**: Compare local and CI runs for schema/path parity and portability.

### Implementation for User Story 6

- [x] T046 [US6] Enforce workspace-relative evidence paths in all coverage helpers in scripts/coverage-normalize-lane-result.sh
- [x] T047 [US6] Add parity checks between spec/data-model/contracts and validator behavior in scripts/validate-compose-ci-lane-contract.sh
- [x] T048 [P] [US6] Validate path-safety and absolute-path rejection in scripts/validate-compose-ci-lane-contract.sh
- [x] T049 [US6] Add portability verification notes and examples in .specify/specs/003-coverage-80-rehydration/quickstart.md
- [x] T050 [US6] Update copilot plan reference validation guidance in .github/copilot-instructions.md

**Checkpoint**: Contract parity and portability are enforced and auditable.

---

## Phase 9: Polish & Cross-Cutting Concerns

**Purpose**: Close verification windows, traceability, and release readiness.

- [x] T051 [P] Run 10-run baseline verification window and archive results in .artifacts/spec/003-coverage-80-rehydration-verification.md
- [x] T052 [P] Run traceability check from FR/SC to tasks and archive in .artifacts/spec/003-coverage-80-rehydration-traceability.md
- [x] T053 Validate aggregate tuple completeness for one representative run in .artifacts/spec/003-coverage-80-rehydration-verification.md
- [x] T054 Validate zero missing-artifact publication warnings in .artifacts/spec/003-coverage-80-rehydration-verification.md
- [x] T055 Validate exception lifecycle behavior with sample records in .artifacts/spec/003-coverage-80-rehydration-verification.md
- [x] T056 Add release-readiness summary for coverage contract closure in .artifacts/spec/003-coverage-80-rehydration-release-readiness.md

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: starts immediately.
- **Phase 2 (Foundational)**: depends on Phase 1 and blocks story phases.
- **Phases 3-6 (US1-US4)**: begin after Phase 2; can partially overlap when file conflicts are managed.
- **Phases 7-8 (US5-US6)**: begin after foundational schema/summary wiring is stable.
- **Phase 9 (Polish)**: after targeted story completion.

### User Story Dependencies

- **US1 (P1)**: first MVP slice after foundational setup.
- **US2 (P1)**: depends on foundational contract and builds on shared normalization.
- **US3 (P1)**: depends on foundational contract and e2e manifest decisions.
- **US4 (P1)**: depends on lane and aggregate outputs from US1-US3.
- **US5 (P2)**: depends on lane/aggregate outputs for exception surfacing.
- **US6 (P2)**: depends on lane validators and summary outputs for parity checks.

---

## Parallel Opportunities

- Setup: T002-T005 can run in parallel after T001.
- Foundational: T007-T009 can run in parallel after T006.
- US1: T014-T018 can run in parallel per domain.
- US2: T022-T024 can run in parallel by ecosystem.
- US3: T030-T031 can run in parallel by channel.
- US4: T037 and T038 can run in parallel after T035/T036.
- US5: T042 can run in parallel with T043 after T040/T041.
- Polish: T051 and T052 can run in parallel.

---

## Implementation Strategy

### MVP First

1. Complete Phase 1 and Phase 2 foundations.
2. Deliver US1 unit coverage gating.
3. Validate deterministic threshold behavior on baseline runs.

### Incremental Delivery

1. Add US2 integration gating and dependency classification.
2. Add US3 e2e flow-manifest denominator enforcement.
3. Add US4 one-pass diagnosability.
4. Add US5 exception governance.
5. Add US6 parity/portability closure.
6. Finish with Phase 9 verification and readiness artifacts.

### Suggested Validation Ownership

1. Native owner validates native unit/integration tuple coverage.
2. API owner validates ASP.NET and TS API integration tuple coverage.
3. Runtime owner validates e2e channel tuple coverage.
4. Delivery owner validates aggregate summary, parity, and exception governance.
