# Tasks: V3 Runtime Baseline Reset

**Input**: Design documents from `.specify/specs/052-v3-pure-c-typescript-reset/`
**Prerequisites**: `plan.md`, `spec.md`, `research.md`, `data-model.md`, `contracts/`, `quickstart.md`

**Tests**: Include artifact-review and traceability validation tasks because the specification explicitly requires validation for baseline authority, evidence traceability, and lane readiness.

**Organization**: Tasks are grouped by user story so each planning outcome can be completed and reviewed independently.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel when the task touches a separate file and does not depend on incomplete work
- **[Story]**: Maps the task to a specific user story (`[US1]`, `[US2]`, `[US3]`)
- Every task below includes the exact file path to change or validate

## Setup (Shared Infrastructure)

**Purpose**: Re-run the required orchestration gates and align the active feature pointers before story work begins.

- [x] T001 Re-run extension preflight for `.specify/specs/052-v3-pure-c-typescript-reset/plan.md` via `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- [x] T002 Append startup confidence-gate evidence in `.specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md` via `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- [x] T003 Align the active feature pointer in `.specify/feature.json` to `.specify/specs/052-v3-pure-c-typescript-reset/`
- [x] T004 Align the active baseline registry in `.specify/specs/README.md` to `.specify/specs/052-v3-pure-c-typescript-reset/`

---

## Foundational (Blocking Prerequisites)

**Purpose**: Establish the shared planning schema and evidence logging that every user story depends on.

**⚠️ CRITICAL**: Complete these tasks before implementing any user story tasks.

- [x] T005 Define baseline authority, retained capability, evidence record, execution lane, archived lineage, and planning surface rules in `.specify/specs/052-v3-pure-c-typescript-reset/data-model.md`
- [x] T006 [P] Establish reusable heartbeat checkpoints and review evidence slots in `.specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md`
- [x] T007 [P] Reconcile the active artifact inventory and implementation structure in `.specify/specs/052-v3-pure-c-typescript-reset/plan.md`

**Checkpoint**: Shared planning contracts and evidence logging are ready for story-by-story execution.

---

## User Story 1 - Establish trusted baseline authority (Priority: P1) 🎯 MVP

**Goal**: Make Spec 052 the single trusted V3 baseline and keep archived lineage available without letting it remain authoritative.

**Independent Test**: Review the active spec set and confirm there is one authoritative reset spec, superseded specs are archived from the active baseline, and retained runtime scope is clear without consulting archived active specs.

### Validation for User Story 1

- [x] T008 [P] [US1] Add baseline-authority review evidence and archived-lineage audit entries in `.specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md`

### Implementation for User Story 1

- [x] T009 [US1] Update the single-authority baseline language, supersession status, and stale viewport exclusions in `.specify/specs/052-v3-pure-c-typescript-reset/spec.md`
- [x] T010 [P] [US1] Document source-first authority, scaffold-first truth, and lineage-only rationale in `.specify/specs/052-v3-pure-c-typescript-reset/research.md`
- [x] T011 [US1] Publish authoritative inputs, non-authoritative inputs, decision rules, and scope-gate evidence in `.specify/specs/052-v3-pure-c-typescript-reset/contracts/baseline-authority.md`

**Checkpoint**: User Story 1 is complete when Spec 052 can stand alone as the only active V3 authority.

---

## User Story 2 - Build evidence-backed planning inputs (Priority: P2)

**Goal**: Produce a repository-grounded inventory of retained capabilities, evidence paths, and stale-path decisions that downstream planning can trust.

**Independent Test**: Inspect the inventory and verify that each retained capability links to an evidence path and that stale pathways are explicitly identified as non-authoritative.

### Validation for User Story 2

- [x] T012 [P] [US2] Add retained-capability trace review entries and sampled evidence checks in `.specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md`

### Implementation for User Story 2

- [x] T013 [US2] Capture the retained module, contract, and runtime entry-point inventory in `.specify/specs/052-v3-pure-c-typescript-reset/codebase-scan.md`
- [x] T014 [P] [US2] Encode gameplay/customer-facing inclusion and exclusion checks in `.specify/specs/052-v3-pure-c-typescript-reset/v3-scope-gate-checklist.md`
- [x] T015 [US2] Update evidence-capture, scope-gate, and validation workflow guidance in `.specify/specs/052-v3-pure-c-typescript-reset/quickstart.md`

**Checkpoint**: User Story 2 is complete when retained capabilities and stale paths are traceable from live repository evidence.

---

## User Story 3 - Launch parallel V3 planning lanes (Priority: P3)

**Goal**: Turn the retained baseline into bounded execution lanes so teams can plan in parallel without reintroducing non-gameplay coupling.

**Independent Test**: Review the bootstrap planning surfaces and verify that each retained capability belongs to one lane with clear ownership and no dependency on excluded non-gameplay flows.

### Validation for User Story 3

- [x] T016 [P] [US3] Add lane-readiness review entries and owner handoff checkpoints in `.specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md`

### Implementation for User Story 3

- [x] T017 [US3] Define owned paths, single-lane ownership rules, and stitch points in `.specify/specs/052-v3-pure-c-typescript-reset/contracts/execution-lanes.md`
- [x] T018 [US3] Translate retained capabilities into lane slices, dependency order, merge cadence, and fallback rules in `.specify/specs/052-v3-pure-c-typescript-reset/v3-bootstrap-docket.md`
- [x] T019 [US3] Update downstream planning handoffs and lane-local validation guidance in `.specify/specs/052-v3-pure-c-typescript-reset/quickstart.md`

**Checkpoint**: User Story 3 is complete when downstream work can start from one lane owner per retained capability.

---

## Polish & Cross-Cutting Concerns

**Purpose**: Finish the reset with cross-artifact consistency and readiness evidence.

- [x] T020 [P] Cross-check authority, evidence, and lane ownership consistency across `.specify/specs/052-v3-pure-c-typescript-reset/spec.md`, `.specify/specs/052-v3-pure-c-typescript-reset/plan.md`, `.specify/specs/052-v3-pure-c-typescript-reset/research.md`, `.specify/specs/052-v3-pure-c-typescript-reset/data-model.md`, `.specify/specs/052-v3-pure-c-typescript-reset/contracts/baseline-authority.md`, `.specify/specs/052-v3-pure-c-typescript-reset/contracts/execution-lanes.md`, `.specify/specs/052-v3-pure-c-typescript-reset/codebase-scan.md`, `.specify/specs/052-v3-pure-c-typescript-reset/v3-scope-gate-checklist.md`, `.specify/specs/052-v3-pure-c-typescript-reset/v3-bootstrap-docket.md`, and `.specify/specs/052-v3-pure-c-typescript-reset/quickstart.md`
- [x] T021 Append the final readiness summary, artifact links, and reviewer notes in `.specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md`

## Implementation Tasks — Server-Authority WASM Multiplayer (T022+)

**Purpose**: Implement the full restore + new-build work identified in the v3-bootstrap-docket.

### Restore — Lane A (Native Engine)

- [x] T022 [P] [US-impl] Restore C engine from `origin/049-arpg-view-actors` into `src/native/engine/`: engine lifecycle (engine.h/engine.c), physics (body/collider/dynamics/world), AI (controller/navigation/perception/state_machine/wildlife_controller), render (camera/material/mesh/renderer/shader/window), world (entity/signals/world), wasm_main.c, engine CMakeLists.txt with EMSCRIPTEN targets
- [x] T023 [P] [US-impl] Restore WASM build scripts from `origin/049-arpg-view-actors`: `scripts/build-engine-wasm-emsdk.sh`, `scripts/build-engine-wasm.sh`, `scripts/build-wasm.sh`; verify WASM artifact produced by `scripts/build-engine-wasm-emsdk.sh`

### Restore — Lane B (API Routes)

- [x] T024 [P] [US-impl] Restore API routes and plugins from `origin/049-arpg-view-actors`: `src/typescript/api/src/routes/game-session.ts` (WebSocket server-authority), auth middleware, Fastify plugins (rate-limit, feature-flags, audit-log, drift-detection), chat/training/corpus/streaming routes; verify `bun run build` green and WebSocket upgrade on `/game-session` returns 101

### Restore — Lane C (React Web Portal)

- [x] T025 [P] [US-impl] Restore React game portal from `origin/049-arpg-view-actors`: `src/typescript/react/src/pages/GameEnginePage.tsx`, `wasm/WasmViewport.tsx`, `workers/bananaWasmWorker.ts`, `lib/wasmRuntime.ts`, `lib/wasmKernels.ts`, `lib/hooks/useWasmWorker.ts`, `components/GameTelemetry.tsx`, `components/GameWorldMap.tsx`; verify Vite dev renders WASM canvas

### Restore — Lane D (Shared Netcode Domains)

- [x] T026 [P] [US-impl] Restore shared netcode domains from `origin/049-arpg-view-actors`: `src/typescript/shared/ui/src/domains/NetcodeDomain.ts`, `PredictionDomain.ts`, `ReplicationDomain.ts`, `ConnectionManager.ts`, `InputDomain.ts`, `ViewportDomain.ts`; verify build green and prediction/rollback unit tests pass

### New Build — Server-Authority

- [x] T027 [US-impl] Add `engine_tick_serialize()` export to C engine: `src/native/engine/engine_serialize.h` + `engine_serialize.c`; output is JSON snapshot of full world state; unit test verifies snapshot schema round-trips correctly (slice A1)
- [x] T028 [US-impl] Wire server-side native C engine tick into game-session loop: `src/typescript/api/src/services/nativeEngine.ts` (N-API/FFI binding to `engine_tick_serialize()`); extend `game-session.ts` to call tick per frame, broadcast snapshot to all session WebSocket clients, validate inputs before apply (slice B1 + S1)
- [x] T029 [P] [US-impl] Add multiplayer UI components to React portal: `src/typescript/react/src/components/PlayerListOverlay.tsx` (in-game player list), `src/typescript/react/src/pages/SessionRoomPage.tsx` (pre-game room selection); wire to session WebSocket (slice C1)
- [x] T030 [US-impl] Wire PredictionDomain + ReplicationDomain to real server tick snapshots in `src/typescript/shared/ui/src/domains/`: client applies server snapshot, rolls back prediction state, re-simulates from reconcile point; integration test with mock server snapshot stream (slice D1 + S2)

### Overworld Authority + Native Controller Extension

- [x] T031 [US-impl] Enforce single static authoritative room (`overworld`) and remove player-facing room-id entry from `src/typescript/api/src/routes/game-session.ts`, `src/typescript/react/src/pages/SessionRoomPage.tsx`, and `src/typescript/react/src/lib/api.ts`
- [x] T032 [US-impl] Add authoritative server TPS/lag telemetry to snapshot/status payloads and expose it in in-game overlays via `src/typescript/api/src/routes/game-session.ts`, `src/typescript/react/src/lib/api.ts`, `src/typescript/react/src/components/PlayerListOverlay.tsx`, and `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T033 [US-impl] Expand API gameplay route tests to cover overworld-only joins, roomless status access, and TPS contract fields in `src/typescript/api/src/routes/game-session.test.ts`
- [x] T034 [US-impl] Add compose runtime channel for overworld validation and log inspection in `docker-compose.yml`, `docker/README.md`, and `.specify/specs/052-v3-pure-c-typescript-reset/quickstart.md`
- [x] T035 [US-impl] Move `PlayerEntity` into `src/native/engine` as a GUID-rooted base entity with pluggable controller strategy (`human`, AI/NPC, automation) and shared movement/state serialization hooks in `src/native/engine/engine.h`, `src/native/engine/engine.c`, `src/native/engine/engine_serialize.h`, and `src/native/engine/engine_serialize.c`
- [x] T036 [US-impl] Bridge API session player mappings to native controller instances (human input + AI spawn surfaces) in `src/typescript/api/src/services/nativeEngine.ts` and `src/typescript/api/src/routes/game-session.ts`, with coverage updates in `src/typescript/api/src/routes/game-session.test.ts`

---

### Stage Dependencies

- **Setup**: Starts immediately and must finish before foundational work
- **Foundational**: Depends on Setup and blocks all user stories
- **User Story 1 (P1)**: Starts after Foundational and establishes the authoritative baseline
- **User Story 2 (P2)**: Depends on User Story 1 because evidence inventory must inherit from the settled baseline authority
- **User Story 3 (P3)**: Depends on User Story 2 because lane ownership must be assigned from the retained-capability inventory
- **Polish**: Depends on all selected user stories being complete

### User Story Dependencies

- **US1**: No dependency on other user stories after Foundational
- **US2**: Depends on US1 baseline authority outputs in `spec.md`, `research.md`, and `contracts/baseline-authority.md`
- **US3**: Depends on US2 evidence inventory outputs in `codebase-scan.md`, `v3-scope-gate-checklist.md`, and `quickstart.md`

### Within Each User Story

- Validation entries should be prepared before story implementation is marked complete
- Governance rules should be updated before downstream guidance that depends on them
- Inventory artifacts should be updated before lane assignment artifacts
- Heartbeat evidence should be appended at each story checkpoint

### Parallel Opportunities

- Setup task T003 and T004 can run in parallel after T001 and T002
- Foundational task T006 and T007 can run in parallel after T005
- In US1, T008 and T010 can run in parallel while T009 is in review
- In US2, T012 and T014 can run in parallel before T015 consolidates operator guidance
- In US3, T016 can run in parallel with T017 while T018 waits for lane rules to settle
- T020 can be split into parallel review passes by artifact owner before T021 consolidates the readiness record

---

## Parallel Example: User Story 1

```bash
# Prepare the audit trail and authority rationale together:
Task: "Add baseline-authority review evidence and archived-lineage audit entries in .specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md"
Task: "Document source-first authority, scaffold-first truth, and lineage-only rationale in .specify/specs/052-v3-pure-c-typescript-reset/research.md"
```

## Parallel Example: User Story 2

```bash
# Build traceability evidence and scope-gate checks together:
Task: "Add retained-capability trace review entries and sampled evidence checks in .specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md"
Task: "Encode gameplay/customer-facing inclusion and exclusion checks in .specify/specs/052-v3-pure-c-typescript-reset/v3-scope-gate-checklist.md"
```

## Parallel Example: User Story 3

```bash
# Prepare lane review evidence while defining the lane contract:
Task: "Add lane-readiness review entries and owner handoff checkpoints in .specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md"
Task: "Define owned paths, single-lane ownership rules, and stitch points in .specify/specs/052-v3-pure-c-typescript-reset/contracts/execution-lanes.md"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Setup
2. Complete Foundational
3. Complete User Story 1
4. Validate that Spec 052 stands alone as the active V3 authority
5. Pause for baseline review before expanding planning scope

### Incremental Delivery

1. Complete Setup + Foundational to stabilize the planning schema
2. Deliver User Story 1 to settle authority and archived lineage
3. Deliver User Story 2 to make retained capabilities traceable from live evidence
4. Deliver User Story 3 to divide retained work into bounded parallel lanes
5. Finish with Polish to confirm cross-artifact consistency and readiness

### Parallel Team Strategy

1. One planner handles Setup + Foundational gate work
2. After Foundational, baseline authority review and research updates can proceed together for US1
3. After US1, evidence-inventory and scope-gate tasks can split across owners for US2
4. After US2, lane-contract and lane-docket tasks can split across owners for US3
