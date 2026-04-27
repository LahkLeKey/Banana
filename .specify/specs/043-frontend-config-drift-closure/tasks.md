# Tasks: Frontend Config Drift Closure

**Input**: Design documents from `.specify/specs/043-frontend-config-drift-closure/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

## Phase 1: Setup (Shared Infrastructure)

- [ ] T001 Confirm active feature path and planning artifacts in `.specify/specs/043-frontend-config-drift-closure/`
- [ ] T002 Capture baseline repro (`API base: <unset>`) using canonical `Frontends: API + React` launch flow in `.vscode/launch.json`

---

## Phase 2: Foundational (Blocking Prerequisites)

- [ ] T003 Define canonical frontend API-base contract and failure semantics in `.specify/wiki/human-reference/Build-Run-Test-Commands.md`
- [ ] T004 [P] Add a scriptable drift diagnostic entry point at `scripts/validate-frontend-config-drift.sh`
- [ ] T005 [P] Add VS Code task wrapper(s) for frontend config drift diagnostics in `.vscode/tasks.json`

**Checkpoint**: Frontend drift diagnosis can run before story implementation.

---

## Phase 3: User Story 1 - React launch never shows `<unset>` (Priority: P1) 🎯 MVP

**Goal**: Ensure canonical React launch resolves and surfaces a non-empty API base; fail loudly with remediation when empty.

**Independent Test**: Launch `Frontends: API + React` and verify UI shows concrete API base or explicit config error with remediation steps.

- [ ] T006 [US1] Harden API base resolution error semantics in `src/typescript/react/src/lib/api.ts`
- [ ] T007 [US1] Surface explicit configuration failure UX in `src/typescript/react/src/App.tsx`
- [ ] T008 [US1] Add/adjust React resolution tests in `src/typescript/react/src/lib/api.test.ts`
- [ ] T009 [US1] Validate React story end-to-end with canonical compose launch and runtime assertions using `scripts/compose-run-profile.sh` and `scripts/compose-profile-ready.sh`

**Checkpoint**: React UI no longer silently degrades to `<unset>` on healthy canonical startup.

---

## Phase 4: User Story 2 - Drift is detectable/actionable (Priority: P1)

**Goal**: Provide one diagnostics flow that pinpoints failing layer and remediation.

**Independent Test**: Run diagnostics command and receive PASS/FAIL per layer (compose interpolation, container env, effective runtime hints) plus recovery commands.

- [ ] T010 [US2] Implement layered diagnostic checks and remediation output in `scripts/validate-frontend-config-drift.sh`
- [ ] T011 [US2] Wire diagnostic task entry in `.vscode/tasks.json` and document command usage in `.specify/wiki/human-reference/Build-Run-Test-Commands.md`
- [ ] T012 [US2] Validate diagnostics against healthy runtime and collect sample output behavior via terminal checks

**Checkpoint**: Developers can isolate drift source without trial-and-error.

---

## Phase 5: User Story 3 - Cross-slice contract alignment (Priority: P2)

**Goal**: Keep React, Electron, and React Native web API-base contracts aligned and regression-resistant.

**Independent Test**: Contract check fails when any required key is removed/renamed, and passes when all mappings are present.

- [ ] T013 [US3] Add contract validation checks (keys + launch ordering assumptions) in `scripts/validate-frontend-config-drift.sh`
- [ ] T014 [US3] Ensure compose and launch contract references remain explicit in `docker-compose.yml` and `.vscode/launch.json`
- [ ] T015 [US3] Validate slice env mappings via compose config and service env checks for React/Electron/React Native

**Checkpoint**: All frontend slices share one explicit, verifiable API-base contract.

---

## Phase 6: Polish & Cross-Cutting Concerns

- [ ] T016 [P] Final docs cleanup for drift diagnostics/runbook in `.specify/wiki/human-reference/Build-Run-Test-Commands.md`
- [ ] T017 Run relevant validations (`get_errors`, shell syntax checks, targeted compose checks) and summarize outcomes in `.specify/specs/043-frontend-config-drift-closure/tasks.md`
- [ ] T018 Mark all completed tasks in `.specify/specs/043-frontend-config-drift-closure/tasks.md`

---

## Dependencies & Execution Order

- Phase 1 -> Phase 2 -> User Stories (US1/US2/US3) -> Phase 6
- US1 is MVP and should be completed first.
- US2 depends on foundational diagnostics scaffolding (T004/T005).
- US3 depends on diagnostics layer and contract docs from US2.

## Parallel Opportunities

- T004 and T005 can run in parallel.
- In US1, T008 can run after initial resolution behavior changes (T006/T007).
- In polish, T016 can run in parallel with T017.

## Implementation Strategy

### MVP First

1. Complete Phase 1 and Phase 2.
2. Deliver US1 (T006-T009) and verify canonical React launch behavior.
3. Ship if `API base: <unset>` is eliminated in healthy startup path.

### Incremental Delivery

1. Add US2 diagnostics flow.
2. Add US3 cross-slice contract assertions.
3. Finalize docs and validation summary.
