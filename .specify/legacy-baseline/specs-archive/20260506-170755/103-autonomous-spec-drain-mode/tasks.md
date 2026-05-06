# Tasks: Autonomous Spec Drain Mode

**Input**: Design documents from `.specify/specs/103-autonomous-spec-drain-mode/`
**Prerequisites**: spec.md, plan.md

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel
- **[Story]**: User story mapping (US1/US2/US3)

## Phase 1: Setup (Shared Infrastructure)

- [x] T001 Create run-state artifact directory contract in `artifacts/orchestration/spec-drain/` and document schema in `.specify/specs/103-autonomous-spec-drain-mode/plan.md`.
- [x] T002 Add `spec-drain` mode argument parsing to `scripts/workflow-orchestrate-sdlc.sh` with safe defaults.
- [x] T003 [P] Add workflow input flags for autonomy and kill switch in `.github/workflows/orchestrate-autonomous-spec-drain.yml` (new or existing orchestrator workflow extension).

## Phase 2: Foundational (Blocking Prerequisites)

- [x] T004 Implement checkpoint read/write helper in `scripts/workflow-spec-drain-state.sh` (new file) with atomic write behavior.
- [x] T005 [P] Implement runnable-spec discovery and blocked reason classification in `scripts/workflow-orchestrate-sdlc.sh`.
- [x] T006 [P] Implement failure-budget tracking (attempts + terminal stop) in `scripts/workflow-orchestrate-sdlc.sh`.
- [x] T007 Implement deterministic stop reason emission (`exhausted`, `budget_exceeded`, `policy_blocked`, `kill_switch`) in `scripts/workflow-orchestrate-sdlc.sh`.

**Checkpoint**: Autonomous loop prerequisites complete; user-story execution can begin.

## Phase 3: User Story 1 - Run Until Empty (Priority: P1)

**Goal**: Autonomously execute runnable specs sequentially until none remain.

**Independent Test**: Dry-run over a fixture set shows each spec processed once and final stop reason is `exhausted`.

- [x] T008 [US1] Implement next-spec selection loop in `scripts/workflow-orchestrate-sdlc.sh`.
- [x] T009 [US1] Integrate per-spec branch and PR orchestration via `scripts/workflow-orchestrate-triaged-item-pr.sh`.
- [x] T010 [US1] Add mandatory validation gate sequence (existing validators + required checks) before merge step.
- [x] T011 [US1] Emit per-spec evidence JSON (`artifacts/orchestration/spec-drain/<run-id>/<spec>/result.json`).

## Phase 4: User Story 2 - Failure Budget Stop (Priority: P1)

**Goal**: Stop autonomous execution when configured failure budget is exceeded.

**Independent Test**: Inject repeated failing spec; run stops exactly at budget threshold with summary evidence.

- [x] T012 [US2] Add configurable budget env vars (`BANANA_DRAIN_MAX_FAILURES`, `BANANA_DRAIN_MAX_RETRIES`) in workflow and script.
- [x] T013 [US2] Add retry/backoff behavior for transient failure classes in `scripts/workflow-orchestrate-sdlc.sh`.
- [x] T014 [US2] Add terminal summary JSON with failed-spec list and failure-class counts.

## Phase 5: User Story 3 - Resume After Interruption (Priority: P1)

**Goal**: Resume from persisted checkpoint after interruption without duplicate processing.

**Independent Test**: Mid-run interruption followed by restart continues from next pending spec with no reprocessing.

- [x] T015 [US3] Persist cursor/checkpoint after each successful or terminal spec attempt.
- [x] T016 [US3] Implement startup recovery path that loads last checkpoint and validates integrity.
- [x] T017 [US3] Add idempotency guard to skip already-completed specs listed in checkpoint.

## Phase 6: Validation and Guardrails

- [x] T018 Add CI smoke workflow/job for `spec-drain` dry-run with fixture specs in `.github/workflows/orchestrate-autonomous-spec-drain.yml`.
- [x] T019 [P] Add restart simulation test script `scripts/test-spec-drain-resume.sh`.
- [x] T020 [P] Add kill-switch test (`BANANA_AUTONOMY_STOP=true`) asserting immediate safe stop.
- [x] T021 Run `python scripts/validate-ai-contracts.py` and include evidence path in summary artifact.

## Phase 7: Documentation and Ops Handoff

- [x] T022 Update operator docs in `.specify/wiki/human-reference/operations/` with activation, stop controls, and troubleshooting.
- [x] T023 Add examples for resume and failure-budget interpretation to runbook docs.
- [x] T024 Add final implementation notes and evidence index to `.specify/specs/103-autonomous-spec-drain-mode/`.
