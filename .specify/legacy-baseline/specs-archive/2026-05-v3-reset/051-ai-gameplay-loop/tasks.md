# Tasks: Spec 051 Native+AI+WASM Docket

## Inherited Completion Baseline (Merged from Spec 050)

- [x] M001 Multiplayer authority articles I-V completed and validated.
- [x] M002 Shared UI domain contracts validated (Netcode/Replication/Prediction/ConnectionManager).
- [x] M003 API session and anti-cheat orchestration baseline validated.
- [x] M004 Native anti-cheat + ABI baseline validated.
- [ ] M005 Non-blocking parity follow-up: direct TypeScript API native invocation parity.

## Stage 0: Context Preservation (Blocking)

- [x] T001 Document current execution reality in `spec.md` (working paths, failures, risks)
- [x] T002 Expand `plan.md` with phased native-domain + WASM orchestration flow
- [x] T003 Replace checklist with dependency-ordered docket and acceptance gates

## Stage 1: Root Native Build Rebind (Blocking)

- [ ] T010 Update root `CMakeLists.txt` source lists to active migrated native domains
- [ ] T011 Ensure root native target builds without deleted `src/native/core/*` dependencies
- [ ] T012 Keep wrapper ABI target and symbol contract unchanged while rerouting implementation

## Stage 2: Wrapper/Domain Compatibility Integration (Blocking)

- [ ] T020 Replace `banana_wrapper.c` calls to removed `banana_core_*`/`banana_dal_*` with current domain services/adapters
- [ ] T021 Complete minimum callable service APIs for batch/truck/ripeness/status/classify surfaces used by wrapper
- [ ] T022 Normalize anti-cheat domain shared type contract includes across migrated services

## Stage 3: Engine AI Loop Completion

- [ ] T030 Finalize ML controller registration/spawn path for non-player actors
- [ ] T031 Finalize per-frame ML movement compute path and entity sync
- [ ] T032 Validate on-screen movement behavior in web viewport
- [ ] T033 Verify histogram updates for `ai_action` and `player_action`

## Stage 4: WASM Orchestration for Web Main Viewport

- [ ] T040 Consolidate WASM build contract (CMake Emscripten target and script orchestration)
- [ ] T041 Ensure artifacts publish to React web public WASM path with version metadata
- [ ] T042 Integrate WASM artifact generation into canonical runtime orchestration scripts/channels
- [ ] T043 Ensure runtime startup messaging is actionable when Emscripten/artifacts are missing
- [ ] T044 Enforce WASM parallel contract in build tooling (`-pthread`, `USE_PTHREADS`, `PTHREAD_POOL_SIZE` budget, OpenMP)
- [ ] T045 Add/maintain CI hard gate job for WASM parallel contract validation

## Stage 5: End-to-End Runtime Validation and Evidence

- [ ] T050 Run and verify root native build in runtime-aligned environment
- [ ] T051 Run compose runtime+apps profile successfully
- [ ] T052 Confirm AI playing on screen in web viewport
- [ ] T053 Update `heartbeat-log.md` with evidence, residual risks, and next follow-ups
- [ ] T054 Record WASM parallel gate evidence (contract check + regression tests)

## Parallelization Rules

- T020-T022 may run in parallel after T010-T012.
- T030-T033 may start after T020-T022.
- T040-T043 can run in parallel with T030-T033 after T010-T012.
- T050-T053 execute last after all previous stages complete.
