# Tasks: Chat Bootstrap Fetch Failure Follow-up

**Input**: Design documents from `.specify/specs/046-chat-bootstrap-fetch-failure/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/chat-bootstrap-runtime-contract.md, quickstart.md

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Align feature artifacts and validation scaffolding for chat bootstrap failure classification/recovery.

- [ ] T001 Confirm feature scope and runtime-readiness validation baseline in .specify/specs/046-chat-bootstrap-fetch-failure/plan.md
- [ ] T002 Align bootstrap classification and recovery contract details in .specify/specs/046-chat-bootstrap-fetch-failure/contracts/chat-bootstrap-runtime-contract.md
- [ ] T003 [P] Add canonical startup-order validation commands in .specify/specs/046-chat-bootstrap-fetch-failure/quickstart.md

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Implement shared bootstrap classification/state primitives required by all user stories.

**CRITICAL**: No user story work should begin until this phase is complete.

- [ ] T004 Create bootstrap error taxonomy helpers for transport vs configuration failures in src/typescript/react/src/lib/api.ts
- [ ] T005 [P] Add reusable bootstrap lifecycle state utilities in src/typescript/react/src/lib/resilience-bootstrap.ts
- [ ] T006 [P] Add bootstrap transport/config remediation copy primitives in src/typescript/react/src/lib/copy.ts
- [ ] T007 Wire foundational bootstrap status derivation from shared helpers in src/typescript/react/src/App.tsx
- [ ] T008 Add foundational unit coverage for bootstrap classification and state helpers in src/typescript/react/src/lib/api.test.ts

**Checkpoint**: Foundational bootstrap primitives are complete; user stories can proceed.

---

## Phase 3: User Story 1 - Bootstrap succeeds when runtime is healthy (Priority: P1) 🎯 MVP

**Goal**: Ensure chat bootstrap reaches ready state and enables chat interactions when runtime/apps are healthy.

**Independent Test**: Bring up runtime then apps in canonical order, load the app, and confirm no fetch-failure banner and usable chat controls.

- [ ] T009 [P] [US1] Add healthy startup bootstrap-render test coverage in src/typescript/react/src/App.test.tsx
- [ ] T010 [P] [US1] Add createChatSession bootstrap success-path coverage in src/typescript/react/src/lib/api.test.ts
- [ ] T011 [US1] Implement deterministic bootstrapping-to-ready transition after session creation in src/typescript/react/src/App.tsx
- [ ] T012 [US1] Gate chat input and send controls on ready-state bootstrap completion in src/typescript/react/src/App.tsx
- [ ] T013 [US1] Record healthy-path runtime/apps startup verification steps in .specify/specs/046-chat-bootstrap-fetch-failure/quickstart.md

**Checkpoint**: US1 is independently functional and verifiable as the MVP.

---

## Phase 4: User Story 2 - Network/bootstrap errors are accurate and actionable (Priority: P1)

**Goal**: Distinguish transport/runtime unavailability errors from configuration-resolution errors with actionable remediation.

**Independent Test**: Stop runtime while frontend remains up, reload, and confirm transport-classified messaging; restore runtime and confirm error clears on retry/reload.

- [ ] T014 [P] [US2] Add bootstrap transport failure classification tests for fetch/network failures in src/typescript/react/src/lib/api.test.ts
- [ ] T015 [P] [US2] Add config-resolution vs transport banner rendering tests in src/typescript/react/src/App.test.tsx
- [ ] T016 [US2] Implement bootstrap fetch transport-error classifier in src/typescript/react/src/lib/api.ts
- [ ] T017 [US2] Render bootstrap_transport remediation guidance in src/typescript/react/src/App.tsx
- [ ] T018 [US2] Keep config_resolution messaging isolated from transport-error messaging in src/typescript/react/src/App.tsx
- [ ] T019 [US2] Update transport remediation copy to reference canonical runtime-before-apps order in src/typescript/react/src/lib/copy.ts

**Checkpoint**: US2 is independently functional with distinct transport/config error handling.

---

## Phase 5: User Story 3 - Startup order and drift diagnostics prevent recurrence (Priority: P2)

**Goal**: Add runtime readiness diagnostics that catch startup order and dependency mismatches before closure.

**Independent Test**: Run failure-path and healthy-path readiness diagnostics and verify clear pass/fail signals plus rendered-state evidence guidance.

- [ ] T020 [P] [US3] Add runtime-unavailable and recovery diagnostic scenarios in .specify/specs/046-chat-bootstrap-fetch-failure/quickstart.md
- [ ] T021 [P] [US3] Expand healthy/failure/recovery evidence expectations in .specify/specs/046-chat-bootstrap-fetch-failure/contracts/chat-bootstrap-runtime-contract.md
- [ ] T022 [US3] Improve timeout diagnostics and dependency-layer failure output in scripts/compose-profile-ready.sh
- [ ] T023 [US3] Add runtime-first remediation hints for startup-order mismatch in scripts/compose-run-profile.sh
- [ ] T024 [US3] Add task-level runtime/apps readiness validation entry in .vscode/tasks.json

**Checkpoint**: US3 diagnostics catch bootstrap dependency regressions deterministically.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Finalize evidence and closure artifacts spanning all stories.

- [ ] T025 [P] Consolidate one-window integrated-browser validation checklist in .specify/specs/046-chat-bootstrap-fetch-failure/quickstart.md
- [ ] T026 [P] Update post-implementation constitution and validation notes in .specify/specs/046-chat-bootstrap-fetch-failure/plan.md
- [ ] T027 Capture final classification/recovery readiness evidence summary in .specify/specs/046-chat-bootstrap-fetch-failure/research.md

---

## Dependencies & Execution Order

### Phase Dependencies

- Setup (Phase 1): No dependencies.
- Foundational (Phase 2): Depends on Setup completion; blocks all user stories.
- User Story phases (Phases 3-5): Depend on Foundational completion.
- Polish (Phase 6): Depends on completion of US1, US2, and US3.

### User Story Dependencies

- US1 (P1): Starts immediately after Foundational; no dependency on other stories.
- US2 (P1): Starts after Foundational; logically independent, but coordinate merges because US1/US2 share App and API client files.
- US3 (P2): Starts after Foundational and should land after P1 stories to validate final behavior and diagnostics.

### Within Each User Story

- Tests and validation tasks should be implemented before or alongside behavior changes they verify.
- Shared model/helper updates precede UI/rendering updates.
- Runtime diagnostics updates precede closure evidence updates.

---

## Parallel Opportunities

- Phase 1: T003 can run in parallel with T001-T002.
- Phase 2: T005 and T006 can run in parallel after T004 starts.
- US1: T009 and T010 can run in parallel.
- US2: T014 and T015 can run in parallel.
- US3: T020 and T021 can run in parallel.
- Polish: T025 and T026 can run in parallel.

### Parallel Example: User Story 1

- Run T009 and T010 together while aligning bootstrap ready-state behavior.

### Parallel Example: User Story 2

- Run T014 and T015 together before implementing T016-T019.

### Parallel Example: User Story 3

- Run T020 and T021 together, then implement script/task updates in T022-T024.

---

## Implementation Strategy

### MVP Scope

- MVP is User Story 1 only (Phase 3) after Setup and Foundational phases are complete.
- Validate healthy bootstrap readiness end-to-end before proceeding.

### Incremental Delivery

1. Complete Setup and Foundational phases.
2. Deliver US1 (healthy bootstrap success) and validate independently.
3. Deliver US2 (transport/config classification and recovery messaging) and validate independently.
4. Deliver US3 (runtime readiness diagnostics) and validate independently.
5. Finish Polish phase and capture closure evidence.

### Execution Notes

- Keep all changes scoped to chat bootstrap classification/recovery and runtime readiness validation surfaces.
- Preserve canonical runtime/apps startup order and one-window integrated-browser verification contract.
