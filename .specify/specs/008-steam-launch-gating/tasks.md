# Tasks: Steam Launch Gating

**Input**: Design documents from `.specify/specs/008-steam-launch-gating/`

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Validation evidence is required for each user story and for failure-mode handling.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (`[US1]`, `[US2]`, `[US3]`)
- Include exact file paths in each task description

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Create feature scaffolding and shared contract constants used across native, API, and web flows.

- [x] T001 Create launch-gating readiness checklist scaffold in `.specify/specs/008-steam-launch-gating/checklists/steam-launch-gating-readiness.md`
- [x] T002 [P] Add launch-gate reason-code constants in `src/typescript/api/src/lib/contracts/launchGateReasonCodes.ts`
- [x] T003 [P] Add shared launch-gate policy definitions in `src/native/include/banana_launch_gate_policy.h`
- [x] T004 [P] Add blocked-launch UX type definitions in `src/typescript/react/src/lib/launchGateTypes.ts`
- [x] T005 Add validation runner script for feature evidence capture in `scripts/validate-steam-launch-gating.sh`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Implement base contracts and wiring that all user stories depend on.

**CRITICAL**: Complete this phase before starting any user story work.

- [x] T006 Extend native v3 ABI launch-decision contract in `src/native/include/banana_native_v3.h`
- [x] T007 Implement native launch-gate ABI bridge hooks in `src/native/engine/runtime/engine/engine_aux_abi.c`
- [x] T008 [P] Add API launch-gate domain service interface in `src/typescript/api/src/domains/player-identity-account/services/launchGateVerificationService.ts`
- [x] T009 [P] Add API remediation ticket service interface in `src/typescript/api/src/domains/player-identity-account/services/remediationTicketService.ts`
- [x] T010 [P] Add environment-mode policy resolver for API in `src/typescript/api/src/lib/contracts/environmentModePolicy.ts`
- [x] T011 Add Fastify route registration for launch-gate and remediation endpoints in `src/typescript/api/src/routes/v1/player.ts`
- [x] T012 Add launch-gate audit event envelope and serializer in `src/typescript/api/src/lib/launchGateAudit.ts`

**Checkpoint**: Shared launch-gate/native-ABI/API foundations are in place.

---

## Phase 3: User Story 1 - Block Unverified Runtime Launch (Priority: P1) 🎯 MVP

**Goal**: Enforce hard allow/deny launch decisions for production launch attempts.

**Independent Test**: Verify allow only for valid linked good-standing account, and deny for unlinked/suspended/stale/unavailable cases.

### Tests for User Story 1

- [x] T013 [P] [US1] Add API verify endpoint contract tests for allow/deny reason taxonomy in `src/typescript/api/src/routes/v1/launch-gate.contract.test.ts`
- [x] T014 [P] [US1] Add native launch matrix tests for production deny-safe behavior in `tests/native/runtime/engine/launch_gate_matrix_test.c`
- [x] T015 [P] [US1] Add stale-session handling tests for native launch decisions in `tests/native/player/player_sync_staleness_test.c`

### Implementation for User Story 1

- [x] T016 [US1] Implement `POST /v1/launch-gate/verify` and `GET /v1/launch-gate/status/{attemptId}` routes in `src/typescript/api/src/routes/v1/launch-gate.ts`
- [x] T017 [US1] Implement launch verification pipeline decision logic in `src/typescript/api/src/domains/player-identity-account/pipelines/verifyLaunchAttemptPipeline.ts`
- [x] T018 [US1] Implement launch decision persistence state store in `src/typescript/api/src/domains/player-identity-account/state/launchVerificationDecisionStore.ts`
- [x] T019 [US1] Enforce runtime hard gate before gameplay startup in `src/native/engine/runtime/engine/engine_lifecycle.c`
- [x] T020 [US1] Map launch decision reason codes to native blocked states in `src/native/engine/runtime/engine/engine_state.c`

**Checkpoint**: US1 is independently testable and satisfies production hard-gate behavior.

---

## Phase 4: User Story 2 - Complete Registration or Linking Through Web Flow (Priority: P2)

**Goal**: Provide actionable remediation for blocked users and require fresh re-verification after remediation.

**Independent Test**: Fail launch as unlinked, complete remediation flow, then pass only after a new verification attempt.

### Tests for User Story 2

- [x] T021 [P] [US2] Add remediation endpoint contract tests for ticket lifecycle and completion callback in `src/typescript/api/src/routes/v1/account-link-remediation.contract.test.ts`
- [x] T022 [P] [US2] Add remediation retry integration test covering relaunch-and-verify flow in `src/typescript/api/src/integration/launch-gate-remediation.integration.test.ts`
- [x] T023 [P] [US2] Add blocked-launch panel rendering and action tests in `src/typescript/react/src/components/LaunchBlockedPanel.test.tsx`

### Implementation for User Story 2

- [x] T024 [US2] Implement remediation ticket create/status/completion routes in `src/typescript/api/src/routes/v1/account-link-remediation.ts`
- [x] T025 [US2] Implement remediation ticket lifecycle service in `src/typescript/api/src/domains/player-identity-account/services/remediationTicketService.ts`
- [x] T026 [US2] Implement retry-intent endpoint and fresh-attempt handoff in `src/typescript/api/src/routes/v1/launch-gate.ts`
- [x] T027 [US2] Add launch-gate remediation API client methods in `src/typescript/react/src/lib/api.ts`
- [x] T028 [US2] Implement blocked-launch remediation UI panel and wiring in `src/typescript/react/src/components/LaunchBlockedPanel.tsx`

**Checkpoint**: US2 remediation and relaunch journey is independently testable.

---

## Phase 5: User Story 3 - Practical Development Workflow Guardrails (Priority: P3)

**Goal**: Enforce mode-specific policy so development remains practical while production remains non-bypassable.

**Independent Test**: Development mode can follow explicit fallback policy with audit markers, while production mode always denies when verification is incomplete.

### Tests for User Story 3

- [x] T029 [P] [US3] Add API mode-policy contract tests for override precedence and production denial safeguards in `src/typescript/api/src/routes/v1/launch-gate.mode-policy.contract.test.ts`
- [x] T030 [P] [US3] Add native mode-policy tests for non-bypassable production behavior in `tests/native/runtime/engine/launch_gate_mode_policy_test.c`
- [x] T031 [P] [US3] Add development-fallback UX indicator tests in `src/typescript/react/src/pages/SessionRoomPage.launch-mode.test.tsx`

### Implementation for User Story 3

- [x] T032 [US3] Implement trusted launch-mode resolution in `src/native/engine/runtime/engine/engine_host.c`
- [x] T033 [US3] Enforce production override ignore rules in `src/native/scaffold/native_entry.c`
- [x] T034 [US3] Enforce API mode precedence and override-source filtering in `src/typescript/api/src/domains/player-identity-account/services/launchGateVerificationService.ts`
- [x] T035 [US3] Emit launch decision audit fields for mode/override decisions in `src/typescript/api/src/lib/launchGateAudit.ts`
- [x] T036 [US3] Surface non-production launch-mode disclosure banner in `src/typescript/react/src/pages/SessionRoomPage.tsx`

**Checkpoint**: US3 guardrails are independently testable with explicit mode-policy evidence.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Finalize validation, evidence, and release-readiness checks across all stories.

- [x] T037 [P] Capture API launch-gate contract validation evidence in `artifacts/api/008-launch-gate-contract-status.txt`
- [x] T038 [P] Capture native allow/deny and failure-mode matrix evidence in `artifacts/native/008-launch-gate-allow-deny-matrix.txt`
- [x] T039 [P] Capture remediation and retry evidence in `artifacts/api/008-remediation-flow.txt`
- [x] T040 Run quickstart validation workflow and record results in `artifacts/native/008-failure-mode-retry.txt`
- [x] T041 Update feature quickstart with finalized run commands and evidence references in `.specify/specs/008-steam-launch-gating/quickstart.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- Setup (Phase 1): starts immediately.
- Foundational (Phase 2): depends on Setup; blocks all user stories.
- User Stories (Phase 3-5): each depends on Foundational completion.
- Polish (Phase 6): depends on completion of selected user stories.

### User Story Dependencies

- US1 (P1): no dependency on other stories after Foundational completion.
- US2 (P2): depends on US1 route contracts and reason codes being available.
- US3 (P3): can run after Foundational, but should integrate after US1 decision paths stabilize.

### Dependency Graph

- US1 -> US2
- US1 -> US3 (integration-level dependency only for shared decision semantics)

---

## Parallel Opportunities

- Setup: T002, T003, and T004 can run in parallel.
- Foundational: T008, T009, and T010 can run in parallel.
- US1: T013, T014, and T015 can run in parallel.
- US2: T021, T022, and T023 can run in parallel.
- US3: T029, T030, and T031 can run in parallel.
- Polish: T037, T038, and T039 can run in parallel.

## Parallel Example: User Story 1

```bash
Task T013: src/typescript/api/src/routes/v1/launch-gate.contract.test.ts
Task T014: tests/native/runtime/engine/launch_gate_matrix_test.c
Task T015: tests/native/player/player_sync_staleness_test.c
```

## Parallel Example: User Story 2

```bash
Task T021: src/typescript/api/src/routes/v1/account-link-remediation.contract.test.ts
Task T022: src/typescript/api/src/integration/launch-gate-remediation.integration.test.ts
Task T023: src/typescript/react/src/components/LaunchBlockedPanel.test.tsx
```

## Parallel Example: User Story 3

```bash
Task T029: src/typescript/api/src/routes/v1/launch-gate.mode-policy.contract.test.ts
Task T030: tests/native/runtime/engine/launch_gate_mode_policy_test.c
Task T031: src/typescript/react/src/pages/SessionRoomPage.launch-mode.test.tsx
```

---

## Implementation Strategy

### MVP First (US1)

1. Complete Phase 1 and Phase 2.
2. Complete US1 (Phase 3).
3. Validate production hard-gate behavior with US1 independent tests.
4. Ship/demonstrate MVP if acceptance criteria pass.

### Incremental Delivery

1. Deliver US1 hard gating.
2. Add US2 remediation and relaunch.
3. Add US3 dev-vs-production safeguards.
4. Finalize with cross-cutting validation artifacts.

### Team Parallelization

1. Team completes Setup and Foundational together.
2. Then split by story owner:
   - Engineer A: US1
   - Engineer B: US2
   - Engineer C: US3
3. Merge once each story passes its independent test criteria.

---

## Notes

- All tasks use strict checklist format with task IDs and file paths.
- `[P]` indicates no file-level conflict and no incomplete dependency requirement.
- Validation tasks include blocked-launch behavior and transient failure-mode retry evidence.
