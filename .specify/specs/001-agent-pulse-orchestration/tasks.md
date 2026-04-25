# Tasks: Agent Pulse Orchestration

**Input**: Design documents from `.specify/specs/001-agent-pulse-orchestration/`
**Prerequisites**: `plan.md`, `spec.md`, `research.md`, `data-model.md`, `contracts/autonomous-increment-plan.md`, `quickstart.md`

**Tests**: No net-new test suite requested. Validation relies on workflow contract checks and local dry-run orchestration.

**Organization**: Tasks are grouped by user story to preserve independently deliverable value.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no direct dependency)
- **[Story]**: User story mapping (`[US1]`, `[US2]`, `[US3]`)
- Include exact file paths in each task

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish durable files for plan-driven orchestration and management docs.

- [x] T001 Create default autonomous increment catalog file at `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`
- [x] T002 Create management index scaffold at `docs/automation/agent-pulse/README.md`
- [x] T003 [P] Create per-agent ledger directory and seed files under `docs/automation/agent-pulse/agents/`
- [x] T004 [P] Add automation recorder utility skeleton in `scripts/record-agent-pulse-activity.py`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Wire workflow and orchestration surfaces to repository-managed increment plans.

- [x] T005 Update workflow env in `.github/workflows/orchestrate-autonomous-self-training-cycle.yml` to use plan-path fallback and retain manual override support
- [x] T006 Update workflow defaults/comments in `.github/workflows/orchestrate-autonomous-self-training-cycle.yml` so management can identify where increment ownership is defined
- [x] T007 Confirm `scripts/workflow-orchestrate-sdlc.sh` plan-loading behavior supports the new default plan path contract without regressing existing JSON override precedence

**Checkpoint**: Workflow can load default increment plan from repository file.

---

## Phase 3: User Story 1 - Management Agent Visibility (Priority: P1)

**Goal**: Managers can see broad multi-agent autonomous activity with clear ownership.

**Independent Test**: Run local dry-run orchestration and verify at least 7 agent-scoped increments (including `technical-writer-agent`) plus updated management index output.

- [x] T008 [US1] Expand `docs/automation/agent-pulse/autonomous-self-training-default-increments.json` to include at least 7 distinct `agent:*` ownership lanes, including `agent:technical-writer-agent`
- [x] T009 [US1] Implement immutable per-run activity snapshot generation in `scripts/record-agent-pulse-activity.py`
- [x] T010 [US1] Wire default increment `change_command` entries in `docs/automation/agent-pulse/autonomous-self-training-default-increments.json` to call `scripts/record-agent-pulse-activity.py`
- [x] T011 [US1] Update `docs/automation/agent-pulse/README.md` to provide management deep-dive links per agent and explicit pointers to `Human-Reading-Guide.md` and `AI-Audit-Trails.md`

**Checkpoint**: Multi-agent visibility is discoverable without opening workflow internals.

---

## Phase 4: User Story 2 - Operational Traceability (Priority: P2)

**Goal**: Operators can trace increment ownership and run context for each agent lane.

**Independent Test**: Inspect generated ledgers and confirm run metadata (`run_id`, `attempt`, timestamp, intent) exists for each executed increment.

- [x] T012 [US2] Ensure every default increment in `docs/automation/agent-pulse/autonomous-self-training-default-increments.json` includes one explicit `agent:*` label and intent-rich `pr_body`
- [x] T013 [US2] Ensure recorder command usage in `docs/automation/agent-pulse/autonomous-self-training-default-increments.json` includes agent, increment ID, and intent summary arguments
- [x] T014 [US2] Document traceability fields, audience-lane conventions, and troubleshooting notes in `docs/automation/agent-pulse/README.md`

**Checkpoint**: Ownership and run traceability are complete and operator-readable.

---

## Phase 5: User Story 3 - Spec-Driven Plan Maintainability (Priority: P3)

**Goal**: Engineers can change default orchestration by editing one catalog file.

**Independent Test**: Modify one increment entry and rerun dry-run orchestration without editing inline workflow JSON.

- [x] T015 [US3] Remove or minimize inline default increment JSON in `.github/workflows/orchestrate-autonomous-self-training-cycle.yml` in favor of repository-managed plan path
- [x] T016 [US3] Update `.specify/specs/001-agent-pulse-orchestration/contracts/autonomous-increment-plan.md` if implementation-level contract deltas emerge
- [x] T017 [US3] Update `.specify/specs/001-agent-pulse-orchestration/quickstart.md` with the final plan-path workflow, override procedure, and dual-track doc inspection steps
- [x] T021 [US3] Implement native deterministic lane model in `src/native/core/domain/banana_agent_pulse_model.c` and `src/native/core/domain/banana_agent_pulse_model.h`
- [x] T022 [US3] Add native model CLI and deterministic renderer bridge in `src/native/core/domain/banana_agent_pulse_model_cli.c`, `scripts/build-deterministic-agent-pulse-plan.py`, and `scripts/generate-deterministic-agent-pulse-plan.sh`
- [x] T023 [US3] Wire native deterministic planner mode in `.github/workflows/orchestrate-autonomous-self-training-cycle.yml` and `scripts/workflow-orchestrate-sdlc.sh`
- [x] T024 [US3] Add deterministic native tests and CMake wiring in `tests/native/banana_agent_pulse_model_tests.c` and `CMakeLists.txt`

**Checkpoint**: Default orchestration is managed through a single plan file.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Validate governance and capture final confidence signals.

- [x] T018 Run AI governance validation: `python scripts/validate-ai-contracts.py`
- [x] T019 Run local dry-run orchestration: `bash scripts/workflow-orchestrate-sdlc.sh` with `BANANA_LOCAL_DRY_RUN=true`
- [x] T020 Review changed files for management readability across human and AI lanes and ensure `speckit-driven` provenance remains intact

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: Starts immediately
- **Phase 2 (Foundational)**: Depends on Phase 1
- **Phase 3 (US1)**: Depends on Phase 2
- **Phase 4 (US2)**: Depends on Phase 3 base outputs
- **Phase 5 (US3)**: Depends on Phase 2 and references outcomes from US1/US2
- **Phase 6 (Polish)**: Depends on completion of desired user stories

### User Story Dependencies

- **US1 (P1)**: First deliverable and management-visible MVP
- **US2 (P2)**: Builds on US1 artifacts by deepening run traceability
- **US3 (P3)**: Locks in maintainability by finalizing path-based default orchestration

### Parallel Opportunities

- T003 and T004 can run in parallel during setup.
- T005 and T006 can run in parallel once setup is complete.
- T016 and T017 can run in parallel during US3.

---

## Parallel Example: User Story 1

```bash
Task: "Implement immutable per-run activity snapshot generation in scripts/record-agent-pulse-activity.py"
Task: "Update docs/automation/agent-pulse/README.md to provide management deep-dive links per agent"
```

---

## Implementation Strategy

### MVP First (US1)

1. Complete setup and foundational phases.
2. Implement US1 tasks (T008-T011).
3. Validate manager can inspect multi-agent activity quickly.

### Incremental Delivery

1. Deliver US1 for management visibility.
2. Add US2 for operational traceability.
3. Complete US3 for long-term maintainability.

### Validation Gate

Run T018 and T019 before merge or workflow dispatch.
