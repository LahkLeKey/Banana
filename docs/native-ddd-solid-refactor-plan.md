# Native C DDD + SOLID Refactor Plan

## Goal
Reduce coupling in `src/native/engine` by introducing explicit domain types, separating orchestration from domain behavior, and shrinking global mutable state behind focused services.

## Architecture Direction
- Apply dependency inversion at `engine.c`: keep ABI wrappers thin and delegate behavior to runtime domain/application services.
- Apply single-responsibility in runtime modules: one reason to change per file.
- Replace int/stringly contracts with typed domain values and parser/validator boundaries.
- Keep wrapper/public ABI stable unless explicitly approved.

## Bulk TODO Backlog

### Phase 1: Type Safety Foundations
- [x] Add typed parser helpers for build class, gear slot, and stat name in `runtime/player_builds`.
- [x] Refactor engine player-build ABI wrappers to use typed parsers instead of direct enum casts/string branching.
- [x] Add typed resource keys (`wood`, `ore`, `gold`) with parse helpers and centralized validation.
- [x] Replace merchant trade magic status literals with explicit domain result mapping.

### Phase 2: Engine Orchestration Cleanup
- [x] Introduce `EngineRuntimeState` aggregate to encapsulate `engine.c` globals.
- [x] Move player-focused tick behavior into `runtime/player_runtime_service`.
- [x] Move click-to-move raycast orchestration into `runtime/move_target_service`.
- [x] Isolate merchant bootstrap/trade orchestration into `runtime/merchant_service`.

### Phase 3: Domain Boundaries
- [x] Split `engine.c` into ABI façade + composition root.
- [x] Define application-service ports for terrain, player, merchant, and render submission flows.
- [x] Convert remaining stringly stat/resource/controller lookups to typed boundary adapters.

### Phase 4: Validation Hardening
- [x] Expand tests to cover typed adapters and invalid input paths for engine ABI wrappers.
- [x] Add regression tests around move-target and merchant error mapping.
- [x] Run full native build + CTest suite and capture evidence in artifacts.

### Phase 4 Bulk Execution Checklist
- [x] Add controller-kind typed domain tests and wire into native CMake.
- [x] Add engine ABI typed-boundary invalid input regression test.
- [x] Extend move-target tests with invalid viewport / repeated-click edge cases.
- [x] Extend merchant regression tests with boundary stock/gold transitions and seed-state reuse.
- [x] Run broad native CTest sweep and publish artifact evidence.

### Phase 5: Type-Safe Ports and Domain Contracts (Next Bulk TODOs)
- [x] Introduce typed domain value objects for screen-space click inputs and viewport dimensions, replacing ad-hoc float/int tuples at service boundaries.
- [x] Replace string callback signatures in merchant/resource application ports with typed-key gateway contracts (retain ABI adapters at the edge).
- [x] Add explicit result enums for move-target apply outcomes (accepted/rejected/raycast-failed) and map to ABI-compatible statuses in one place.
- [x] Add compile-time guard macros/static assertions around enum ranges and domain conversion tables used by runtime adapters.
- [x] Add focused native tests proving invalid-type inputs cannot cross domain boundaries without adapter validation.

## Execution Rule
For each TODO slice: keep ABI stable, refactor one seam at a time, build immediately, and add tests before moving to the next seam.

## Latest Execution Evidence
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase6-step2.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase6-step3.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase7-step1.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase7-step2.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase7-step3.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase7-step4.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase7-step5.txt`

### Phase 6: Engine Facade Decomposition (Next Bulk TODOs)
- [x] Extract merchant ABI orchestration from `engine.c` into a dedicated runtime facade to reduce global-state coupling at the ABI edge.
- [x] Add runtime facade regression coverage for delegation and guard behavior (`world_ready`, null item, invalid quantity).
- [x] Extract player/resource ABI wrapper orchestration from `engine.c` into a dedicated runtime facade with typed-key helpers.
- [x] Add runtime player/resource facade regression coverage for parse guards, key-based mutations, and null-player behavior.
- [x] Extract input diagnostics and click/move intent wrappers into `runtime/input_abi` with typed canvas and normalized point boundaries.
- [x] Add runtime input ABI regression coverage for click delegation, viewport normalization, and move-input cancellation guards.
- [ ] Extract remaining auxiliary wrappers (UI/system and sync helpers) into focused runtime facades while preserving public C API signatures.

### Phase 7: Bulk TODO Plan (Queued)
- [x] Extract player build/stat wrappers from `engine.c` into `runtime/player_build_abi` with typed stat/key value objects and boundary parsing.
- [x] Introduce a dedicated `runtime/ui_abi` facade to isolate UI context lifecycle and panel/dialog orchestration from `engine.c` globals.
- [x] Extract multiplayer sync wrappers into `runtime/player_sync_abi` so player staleness/active-count contracts live behind one facade.
- [x] Add compile-time ABI guard assertions ensuring public wrapper signatures remain stable while internal facades evolve.
- [x] Add seam-level regression tests for each new facade and rerun full CTest with archived evidence.
