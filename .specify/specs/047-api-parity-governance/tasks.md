# Tasks: API Parity Governance

**Input**: Design documents from `.specify/specs/047-api-parity-governance/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/api-parity-governance-contract.md, quickstart.md

**Tests**: Include contract and workflow regression tests because parity enforcement is a quality gate feature.

**Organization**: Tasks are grouped by user story so each story remains independently implementable and testable.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish parity-governance artifacts and command entry points shared by all stories.

- [x] T001 Create parity evidence index in .specify/specs/047-api-parity-governance/artifacts/README.md
- [x] T002 Create canonical overlap inventory seed in .specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json
- [x] T003 [P] Create parity exception ledger seed in .specify/specs/047-api-parity-governance/artifacts/parity-exceptions.json
- [x] T004 [P] Create parity drift report seed in .specify/specs/047-api-parity-governance/artifacts/parity-drift-report.json
- [x] T005 Register parity governance command aliases in src/typescript/api/package.json

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Build reusable parity comparison primitives that block all user stories until complete.

**CRITICAL**: User story implementation starts only after this phase is done.

- [x] T006 Implement route key normalization helper in scripts/api-parity/lib/normalize-route-key.mjs
- [x] T007 [P] Implement ASP.NET route surface loader in scripts/api-parity/lib/load-aspnet-route-surface.mjs
- [x] T008 [P] Implement Fastify route surface loader in scripts/api-parity/lib/load-fastify-route-surface.mjs
- [x] T009 Implement shared parity finding model in scripts/api-parity/lib/parity-finding-model.mjs
- [x] T010 Implement parity exception validation (owner/rationale/expiry) in scripts/api-parity/lib/validate-parity-exceptions.mjs
- [x] T011 Implement shell entrypoint for parity governance validation in scripts/validate-api-parity-governance.sh
- [x] T012 Add script-level validation coverage for parity governance runner in tests/scripts/test_validate_api_parity_governance.py

**Checkpoint**: Foundational parity primitives are ready for user-story delivery.

---

## Phase 3: User Story 1 - Build Parity Inventory (Priority: P1) 🎯 MVP

**Goal**: Create a complete inventory of overlapping ASP.NET and Fastify routes with explicit contract expectations.

**Independent Test**: Generate the overlap inventory and confirm every overlapping route includes method, path, expected status semantics, and response-shape expectations.

### Implementation for User Story 1

- [x] T013 [P] [US1] Add overlap inventory contract tests in tests/e2e/Contracts/ApiParityInventoryContractTests.cs
- [x] T014 [US1] Implement overlap inventory builder in scripts/api-parity/build-overlap-inventory.mjs
- [x] T015 [P] [US1] Implement inventory completeness assertion in scripts/api-parity/assert-inventory-coverage.mjs
- [x] T016 [US1] Implement missing-route gap detector in scripts/api-parity/detect-missing-route-gaps.mjs
- [x] T017 [US1] Persist normalized overlap inventory artifact in .specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json
- [x] T018 [US1] Wire inventory generation command into scripts/validate-api-parity-governance.sh
- [x] T019 [US1] Document parity inventory regeneration flow in .specify/specs/047-api-parity-governance/quickstart.md

**Checkpoint**: User Story 1 independently produces and validates a complete parity inventory.

---

## Phase 4: User Story 2 - Enforce Parity Checks (Priority: P1)

**Goal**: Fail when overlapping routes drift on status semantics or response-shape contracts unless a valid exception exists.

**Independent Test**: Run enforcement checks with injected drift and verify unresolved mismatches fail while parity or valid exceptions pass.

### Implementation for User Story 2

- [x] T020 [P] [US2] Add parity enforcement contract tests in tests/e2e/Contracts/ApiParityEnforcementContractTests.cs
- [x] T021 [US2] Implement status parity comparator in scripts/api-parity/compare-status-parity.mjs
- [x] T022 [US2] Implement response-shape parity comparator in scripts/api-parity/compare-shape-parity.mjs
- [x] T023 [US2] Implement actionable drift report generator in scripts/api-parity/build-drift-report.mjs
- [x] T024 [US2] Implement exception application logic with expiry enforcement in scripts/api-parity/apply-parity-exceptions.mjs
- [x] T025 [US2] Implement fail-closed parity gate evaluator in scripts/api-parity/evaluate-parity-gate.mjs
- [x] T026 [US2] Implement end-to-end parity enforcement command in scripts/api-parity/run-parity-gate.mjs
- [x] T027 [US2] Finalize gate and exception semantics in .specify/specs/047-api-parity-governance/contracts/api-parity-governance-contract.md

**Checkpoint**: User Story 2 independently detects and blocks unresolved status/shape parity drift.

---

## Phase 5: User Story 3 - Prevent Regression In Workflow (Priority: P2)

**Goal**: Ensure parity drift blocks delivery and produces auditable findings in automated workflows.

**Independent Test**: Trigger CI with known parity drift and confirm workflow fails with actionable parity findings; restore parity and confirm pass.

### Implementation for User Story 3

- [x] T028 [P] [US3] Add workflow parity regression tests in tests/e2e/Contracts/ApiParityWorkflowGateTests.cs
- [x] T029 [US3] Add API parity enforcement lane to .github/workflows/compose-ci.yml
- [x] T030 [US3] Extend lane result schema handling for api-parity lane in scripts/compose-ci-write-lane-result.sh
- [x] T031 [US3] Publish parity inventory, drift report, and exception ledger artifacts in .github/workflows/compose-ci.yml
- [x] T032 [US3] Add parity governance check invocation to scripts/workflow-orchestrate-sdlc.sh
- [x] T033 [US3] Add parity governance check invocation to scripts/workflow-orchestrate-triaged-item-pr.sh

**Checkpoint**: User Story 3 independently enforces parity in CI/orchestration and prevents workflow regressions.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Harden documentation, governance visibility, and traceability across all stories.

- [x] T034 [P] Add parity governance runbook in .specify/wiki/human-reference/api-parity-governance.md
- [x] T035 [P] Publish human-reference parity page in .wiki/api-parity-governance.md
- [x] T036 Add parity governance wiki sync step to scripts/workflow-sync-wiki.sh
- [x] T037 Add parity governance validator coverage to scripts/validate-spec-tasks-parity.py
- [x] T038 Execute and record full parity quickstart evidence in .specify/specs/047-api-parity-governance/quickstart.md

---

## Dependencies & Execution Order

### Phase Dependencies

- Setup (Phase 1): no dependencies.
- Foundational (Phase 2): depends on Phase 1 and blocks all user stories.
- User Story phases (Phases 3-5): depend on Phase 2 completion.
- Polish (Phase 6): depends on completion of selected user stories.

### User Story Dependencies

- US1 (P1): starts after Phase 2; no dependency on other user stories.
- US2 (P1): starts after Phase 2 and uses US1 inventory artifact format.
- US3 (P2): starts after Phase 2 and depends on US2 gate command output contract.

### Task Dependency Highlights

- T014 depends on T006-T010.
- T021-T026 depend on T014-T018 and T010.
- T029-T033 depend on T026.
- T038 depends on T017, T026, and T031.

---

## Parallel Opportunities

- Setup: T003 and T004 can run in parallel after T001.
- Foundational: T007 and T008 can run in parallel after T006.
- US1: T013 and T015 can run in parallel once foundational tasks complete.
- US2: T021 and T022 can run in parallel once T020 is in place.
- US3: T028 can run in parallel with T029 while workflow lane implementation is in progress.
- Polish: T034 and T035 can run in parallel.

---

## Parallel Example: User Story 1

```bash
Task T013 tests/e2e/Contracts/ApiParityInventoryContractTests.cs
Task T015 scripts/api-parity/assert-inventory-coverage.mjs
```

## Parallel Example: User Story 2

```bash
Task T021 scripts/api-parity/compare-status-parity.mjs
Task T022 scripts/api-parity/compare-shape-parity.mjs
```

## Parallel Example: User Story 3

```bash
Task T029 .github/workflows/compose-ci.yml
Task T028 tests/e2e/Contracts/ApiParityWorkflowGateTests.cs
```

---

## Implementation Strategy

### MVP Scope

- Complete Phase 1 and Phase 2.
- Deliver Phase 3 (US1) and validate inventory completeness.
- Treat US1 as the MVP release slice.

### Incremental Delivery

- Increment 1: US1 inventory baseline and missing-route detection.
- Increment 2: US2 status/shape enforcement and exception-aware fail-closed gate.
- Increment 3: US3 workflow regression prevention and CI artifact publishing.
- Increment 4: Phase 6 documentation, wiki sync, and final quickstart evidence.
