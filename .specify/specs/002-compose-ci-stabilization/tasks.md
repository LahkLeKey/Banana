# Tasks: Compose CI Stabilization

**Input**: Design documents from `.specify/specs/002-compose-ci-stabilization/`  
**Prerequisites**: `plan.md`, `spec.md`, `research.md`, `data-model.md`, `contracts/compose-ci-lane-contract.md`

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish shared lane tooling and artifact conventions used by all stories.

- [x] T001 Define shared lane artifact root and lane metadata variables (FR-001, FR-007) in .github/workflows/compose-ci.yml
- [x] T002 [P] Add compose lane preflight helper (FR-006) in scripts/compose-ci-preflight.sh
- [x] T003 [P] Add compose diagnostics collection helper (FR-002, FR-009) in scripts/compose-ci-collect-diagnostics.sh
- [x] T004 [P] Add artifact manifest helper (FR-003, FR-004, FR-013) in scripts/compose-ci-artifact-manifest.sh
- [x] T005 Ensure shell script LF and execute-bit contract (FR-006) in .gitattributes

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Wire common deterministic lane behavior before story-specific changes.

**⚠️ CRITICAL**: No user story work starts before this phase is complete.

- [x] T006 Integrate preflight helper execution into compose lanes (FR-001, FR-006) in .github/workflows/compose-ci.yml
- [x] T007 [P] Add lane result writer helper (FR-001, FR-002) in scripts/compose-ci-write-lane-result.sh
- [x] T008 [P] Add deterministic stage/error mapping (FR-001, FR-006) to scripts/compose-tests.sh
- [x] T009 [P] Add deterministic health timeout/stage mapping (FR-001, FR-006) to scripts/compose-runtime.sh
- [x] T010 [P] Add deterministic smoke stage/error mapping (FR-001, FR-006) to scripts/compose-electron.sh
- [x] T011 Add always-on diagnostics upload skeleton (FR-002, FR-003, FR-014) for compose lanes in .github/workflows/compose-ci.yml

**Checkpoint**: Shared lane contract is in place and user story implementation can proceed.

---

## Phase 3: User Story 1 - Keep Compose lanes merge-safe (Priority: P1) 🎯 MVP

**Goal**: Merge-gated compose lanes (`compose-tests`, `compose-runtime`, `compose-electron`) end with deterministic pass/fail outcomes and stage attribution.

**Independent Test**: Re-run the same baseline commit and verify lane outcomes and reason codes remain stable across runs.

### Implementation for User Story 1

- [ ] T012 [US1] Refactor compose-tests job to consume lane helpers and write lane result records in .github/workflows/compose-ci.yml
- [ ] T013 [US1] Refactor compose-runtime job to consume lane helpers and write lane result records in .github/workflows/compose-ci.yml
- [ ] T014 [US1] Refactor compose-electron job to consume lane helpers and write lane result records in .github/workflows/compose-ci.yml
- [ ] T015 [P] [US1] Add lane terminal summaries to GitHub step summary in .github/workflows/compose-ci.yml
- [ ] T016 [US1] Add deterministic lane contract validator script in scripts/validate-compose-ci-lane-contract.sh
- [ ] T017 [US1] Run lane contract validator inside compose jobs in .github/workflows/compose-ci.yml

**Checkpoint**: Compose lane pass/fail outcomes are deterministic and attributable.

---

## Phase 4: User Story 2 - Preserve actionable evidence on failures (Priority: P1)

**Goal**: Every failing or skipped lane publishes complete, lane-scoped evidence without missing-path artifact failures.

**Independent Test**: Force representative failures and optional-surface skips, then verify fallback artifacts and diagnostics are uploaded in the same run.

### Implementation for User Story 2

- [ ] T018 [US2] Extend compose lane jobs to always run diagnostics collection helpers in .github/workflows/compose-ci.yml
- [ ] T019 [P] [US2] Add fallback/skip manifest generation for optional surfaces in scripts/compose-ci-artifact-manifest.sh
- [ ] T020 [P] [US2] Update e2e runtime bootstrap diagnostics output in scripts/compose-e2e-bootstrap.sh
- [ ] T021 [P] [US2] Update e2e runtime teardown to preserve diagnostics on failure in scripts/compose-e2e-teardown.sh
- [ ] T022 [US2] Add artifact publication records and upload gates in .github/workflows/compose-ci.yml
- [ ] T023 [US2] Ensure integration/e2e optional project checks emit explicit skip evidence in .github/workflows/compose-ci.yml
- [ ] T024 [US2] Add compose-lane diagnostics contract notes in .specify/wiki/ci-compose-stabilization.md
- [ ] T025 [US2] Publish human-facing compose-lane diagnostics runbook in .wiki/ci-compose-stabilization.md
- [ ] T037 [US2] Add path-safe artifact upload guards for non-compose jobs (`typescript-api-tests`, `dotnet-integration-tests-coverage`) in .github/workflows/compose-ci.yml (FR-013)

### Implementation for User Story 4

- [ ] T038 [US4] Extend deterministic result records to merge-gated non-compose coverage/test jobs in .github/workflows/compose-ci.yml (FR-011, FR-014)
- [ ] T039 [P] [US4] Add permission failure remediation metadata for exit `126` evidence in scripts/compose-tests.sh and scripts/compose-ci-preflight.sh (FR-012)
- [ ] T040 [P] [US4] Add aggregate failed-job summary artifact generation in .github/workflows/compose-ci.yml (FR-014)

**Checkpoint**: Failures and skips produce actionable evidence without missing-path upload errors.

---

## Phase 5: User Story 3 - Eliminate workflow runtime drift warnings (Priority: P2)

**Goal**: Merge-gated workflow executions avoid unowned Node runtime deprecation warnings.

**Independent Test**: Run merge-triggered workflow set and confirm no unowned deprecation warnings are present.

### Implementation for User Story 3

- [ ] T026 [US3] Upgrade runtime-backed action references in .github/workflows/compose-ci.yml to deprecation-safe versions
- [ ] T027 [P] [US3] Add workflow runtime compatibility check script in scripts/check-workflow-runtime-deprecations.sh
- [ ] T028 [US3] Run runtime compatibility check and publish results in .github/workflows/compose-ci.yml
- [ ] T029 [P] [US3] Add bounded exception registry for runtime warnings in .github/workflows/runtime-compatibility-exceptions.yml
- [ ] T030 [US3] Document owner/date exception policy in .specify/wiki/ci-runtime-compatibility.md
- [ ] T031 [US3] Document human runbook for runtime warning triage in .wiki/ci-runtime-compatibility.md

**Checkpoint**: Runtime deprecation drift is removed or explicitly owned with deadlines.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Validate the full feature slice and close SDLC/documentation loops.

- [ ] T032 [P] Run boundary validation for 002 spec (FR-010) and archive output in .artifacts/spec/002-compose-ci-stabilization-boundaries.json
- [ ] T033 [P] Run task traceability validation (FR-009) and archive output in .artifacts/spec/002-compose-ci-stabilization-traceability.json
- [ ] T034 Execute 10-run deterministic verification (FR-001, FR-003, FR-004) and record outcomes in .artifacts/spec/002-compose-ci-stabilization-verification.md
- [ ] T035 Update merge-gate troubleshooting section (FR-009) in README.md
- [ ] T036 Run wiki synchronization workflow script (FR-010) and capture output in .artifacts/spec/002-compose-ci-stabilization-wiki-sync.log
- [ ] T041 [P] Audit merge-gated runs for zero missing-path upload warnings (FR-013) and append findings in .artifacts/spec/002-compose-ci-stabilization-verification.md
- [ ] T042 [P] Validate permission-denied failures include remediation context (FR-012) and append findings in .artifacts/spec/002-compose-ci-stabilization-verification.md

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: starts immediately.
- **Phase 2 (Foundational)**: depends on Phase 1 and blocks all user stories.
- **Phase 3 (US1)**: starts after Phase 2.
- **Phase 4 (US2)**: starts after Phase 2, can run in parallel with late US1 tasks if no file conflicts.
- **US4 implementation tasks (T038-T040)**: start after Phase 2 and can run in parallel with US2 where files do not overlap.
- **Phase 5 (US3)**: starts after Phase 2, may run parallel to US2 where files do not overlap.
- **Phase 6 (Polish)**: after selected story completion.

### User Story Dependencies

- **US1 (P1)**: no dependency on other stories once foundational work is complete.
- **US2 (P1)**: depends on foundational helpers from Phases 1-2; independent from US1 feature logic.
- **US4 (P1)**: depends on foundational helpers from Phases 1-2; complements US2 evidence requirements.
- **US3 (P2)**: depends on foundational workflow structure; independent of US1/US2 business flow.

### Within Each User Story

- Update lane/job logic before adding summary/reporting steps.
- Add helper scripts before wiring workflow consumers.
- Verify contract outputs before polishing docs.

---

## Parallel Opportunities

- **Setup**: T002, T003, T004 can run in parallel after T001.
- **Foundational**: T008, T009, T010 can run in parallel after T006/T007.
- **US1**: T012, T013, T014 can be split by lane owner, then converge on T015-T017.
- **US2**: T019, T020, T021 can run in parallel before T022-T023.
- **US4**: T039 and T040 can run in parallel after T038 scaffolding.
- **US3**: T027 and T029 can run in parallel before T028.
- **Polish**: T032 and T033 can run in parallel.

---

## Parallel Example: User Story 1

```bash
# Parallel lane updates:
Task T012 -> .github/workflows/compose-ci.yml (compose-tests section)
Task T013 -> .github/workflows/compose-ci.yml (compose-runtime section)
Task T014 -> .github/workflows/compose-ci.yml (compose-electron section)

# Parallel helper work before final lane wiring:
Task T008 -> scripts/compose-tests.sh
Task T009 -> scripts/compose-runtime.sh
Task T010 -> scripts/compose-electron.sh
```

---

## Implementation Strategy

### MVP First (US1)

1. Complete Phase 1 and Phase 2.
2. Deliver Phase 3 (US1) deterministic lane outcomes.
3. Validate baseline reruns for deterministic pass/fail with stage attribution.

### Incremental Delivery

1. Add US1 deterministic lane outcomes.
2. Add US2 artifact and diagnostics guarantees.
3. Add US4 merge-gated aggregate and permission-remediation guarantees.
4. Add US3 runtime compatibility enforcement.
5. Finish with Phase 6 cross-cutting validation and documentation sync.

### Parallel Team Strategy

1. Engineer A: lane determinism and lane scripts (US1).
2. Engineer B: diagnostics and artifact publication (US2).
3. Engineer C: non-compose deterministic result and aggregate summary work (US4).
4. Engineer D: runtime compatibility checks and exception policy (US3).
