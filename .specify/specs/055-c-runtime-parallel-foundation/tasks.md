# Tasks: C Runtime Parallel Foundation

**Input**: Design documents from `.specify/specs/055-c-runtime-parallel-foundation/`
**Prerequisites**: plan.md, spec.md

## Setup (Shared Infrastructure)

- [ ] T001 Run extension preflight and capture evidence in heartbeat log
- [ ] T002 Record startup confidence gate for execution session
- [ ] T003 Confirm baseline native configure/build/test commands and current coverage output

## Foundational (Blocking)

- [ ] T004 Define native tick phase boundaries and ownership map in `src/native/engine/engine.c` + companion headers
- [ ] T005 Extract at least one bloated runtime concern from `engine.c` into a bounded C module under `src/native/engine/`
- [ ] T006 Add/adjust exported native APIs for C-owned multiplayer reconciliation lifecycle
- [ ] T007 Update WASM export list and TypeScript bridge contracts for new C APIs

## User Story 1 - Native Tick Decomposition (P1)

- [ ] T008 [US1] Refactor `engine_tick` into explicit phase helpers while preserving deterministic order
- [ ] T009 [US1] Add native tests covering phase ordering and deterministic fallback behavior
- [ ] T010 [US1] Document phase ownership and thread-safety constraints in code comments/docs

## User Story 2 - C-First Multiplayer Sync Ownership (P2)

- [ ] T011 [US2] Move remote player stale/deactivate logic from TypeScript-side loops to C API ownership
- [ ] T012 [US2] Keep TypeScript bridge limited to transport parse + routing for moved concerns
- [ ] T013 [US2] Add tests for reconnect/disconnect and partial snapshot handling against C-owned sync boundaries

## User Story 3 - Renderer-Agnostic Runtime Contract (P3)

- [ ] T014 [US3] Ensure simulation APIs remain renderer-agnostic and avoid renderer-specific branching
- [ ] T015 [US3] Update architecture notes/spec artifacts describing simulation vs renderer adapter boundaries

## DDD/SOLID and Coverage Hardening

- [ ] T016 Apply SRP decomposition checklist to touched native modules (domain policy vs adapter separation)
- [ ] T017 Add/rename tests to reflect bounded domains rather than implementation phases
- [ ] T018 Run native coverage script and record gate result (`scripts/run-native-c-tests-with-coverage.sh`)
- [ ] T019 If coverage below target, add focused tests for extracted modules until gate passes

## Validation and Evidence

- [ ] T020 Run native configure/build/test sequence and capture outputs
- [ ] T021 Run relevant API/React contract tests if bridge payloads changed
- [ ] T022 Append heartbeat evidence for each completed execution slice
