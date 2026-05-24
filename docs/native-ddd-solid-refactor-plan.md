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
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase8-step1.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase8-step2.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase8-step3.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase9-step1.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase9-step2.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase9-step4.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase10-step1.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase10-step2.txt`
- Full native CTest output artifact: `artifacts/native/ctest-debug-2026-05-23-phase11-step1.txt`

### Phase 6: Engine Facade Decomposition (Next Bulk TODOs)
- [x] Extract merchant ABI orchestration from `engine.c` into a dedicated runtime facade to reduce global-state coupling at the ABI edge.
- [x] Add runtime facade regression coverage for delegation and guard behavior (`world_ready`, null item, invalid quantity).
- [x] Extract player/resource ABI wrapper orchestration from `engine.c` into a dedicated runtime facade with typed-key helpers.
- [x] Add runtime player/resource facade regression coverage for parse guards, key-based mutations, and null-player behavior.
- [x] Extract input diagnostics and click/move intent wrappers into `runtime/input_abi` with typed canvas and normalized point boundaries.
- [x] Add runtime input ABI regression coverage for click delegation, viewport normalization, and move-input cancellation guards.
- [x] Extract remaining auxiliary wrappers (UI/system and sync helpers) into focused runtime facades while preserving public C API signatures.

### Phase 7: Bulk TODO Plan (Queued)
- [x] Extract player build/stat wrappers from `engine.c` into `runtime/player_build_abi` with typed stat/key value objects and boundary parsing.
- [x] Introduce a dedicated `runtime/ui_abi` facade to isolate UI context lifecycle and panel/dialog orchestration from `engine.c` globals.
- [x] Extract multiplayer sync wrappers into `runtime/player_sync_abi` so player staleness/active-count contracts live behind one facade.
- [x] Add compile-time ABI guard assertions ensuring public wrapper signatures remain stable while internal facades evolve.
- [x] Add seam-level regression tests for each new facade and rerun full CTest with archived evidence.

### Phase 8: Auxiliary Wrapper Decomposition (Queued)
- [x] Extract controller orchestration and world-telemetry wrappers into `runtime/engine_aux_abi` to reduce `engine.c` dispatch coupling.
- [x] Add focused seam test for auxiliary ABI wrapper behavior and ensure explicit pass/fail output in terminal feedback.
- [x] Extract terrain service-port guards into `runtime/terrain_abi` to keep `engine.c` free of service-pointer branching.
- [x] Extract active-player resolution and resource gateway assembly from `engine.c` into a dedicated runtime gateway helper.
- [x] Rerun focused + full native CTest and publish phase8-step3 evidence.

### Phase 9: Bulk TODO Plan (Queued)
- [x] Introduce typed `RuntimeEngineContext` builders (single constructor) to remove repeated context assembly blocks in `engine.c`.
- [x] Add compile-time ABI guard assertions for terrain and player gateway service-port function pointer signatures.
- [x] Move residual merchant/player resource wrapper glue from `engine.c` into explicit runtime adapter modules.
- [x] Add end-to-end seam test validating merchant trade calls through the new player gateway helper.

### Phase 10: Bulk TODO Plan (Queued)
- [x] Introduce typed adapter context tokens to remove temporary module-level trade state in `runtime/player_merchant_adapter`.
- [x] Add compile-time layout/sentinel guards for adapter-local context and gateway composition invariants.
- [x] Split merchant query ABI (`get_price`) from mutation ABI (`trade_buy`/`trade_sell`) into separate explicit runtime use-case facades.
- [x] Add concurrency-focused seam tests that prove per-call gateway isolation under repeated buy/sell sequences.

### Phase 11: Bulk TODO Plan (Planned)
- [x] Split merchant query ABI (`get_price`) from mutation ABI (`trade_buy`/`trade_sell`) into dedicated runtime facades (`merchant_query_abi` and `merchant_trade_abi`) while keeping exported engine wrapper signatures stable.
- [x] Route `player_merchant_adapter` through the new trade-only facade so mutation flows are isolated from pricing concerns.
- [x] Add seam tests covering price-query isolation, world-ready guards for trade flows, and null-port behavior for each facade.
- [x] Add concurrency-focused seam tests proving per-call gateway isolation under repeated interleaved buy/sell sequences.
- [x] Run focused native tests (`runtime_compile_guard_abi`, `runtime_player_merchant_adapter`, `runtime_merchant_query_abi`, `runtime_merchant_trade_abi`) then full CTest sweep and archive artifacts.

### Phase 12: Legacy Compatibility Retirement (In Progress)
- [x] Remove `runtime/merchant_abi` compatibility wrapper after all production callers migrate to query/trade facades.
- [x] Remove obsolete `runtime_merchant_abi_test` target and source after facade seam tests supersede wrapper coverage.
- [ ] Run focused + full native CTest sweep and archive evidence for compatibility retirement.

### Phase 13: Runtime Orchestration Decoupling (In Progress)
- [x] Remove hidden per-tick orchestration globals in `runtime/engine_composition.c` by introducing explicit context-passing callbacks through `runtime/engine_tick` and `runtime/tick_phases`.
- [x] Introduce bounded-context orchestration modules (`runtime/orchestration/player_tick`, `runtime/orchestration/terrain_tick`, `runtime/orchestration/render_tick`) and migrate composition callbacks from monolithic `engine_composition.c`.
- [x] Split camera policy from camera application: `runtime/camera_follow_policy` computes target eye/target, while render adapter applies camera state to backend.
- [x] Isolate DX12 fallback projection policy from backend transport (`backend_dx12_projection_policy`), including explicit tests for camera-centered fallback quads.
- [x] Move win32 POC objective logic behind a dedicated gameplay objective policy service to avoid ad-hoc scene transitions in `win32_dx12_poc/main.c`.
- [x] Add architecture guard tests enforcing dependency direction: orchestration -> domain services -> render/infra adapters (never reverse).
- [x] Remove hidden service-port module state in `runtime/engine_composition.c` by storing ports in `EngineRuntimeState` and wiring orchestration through explicit context.
- [x] Extract right-click normalization from `runtime/engine_tick.c` into `runtime/input_click_policy` with focused policy tests and guardrails.
- [x] Extract terrain rebuild chunk budget from `runtime/engine_tick.c` into `runtime/tick_budget_policy` with configurable clamp rules and focused tests.
- [x] Extract right-click polling/dispatch from `runtime/engine_tick.c` into `runtime/tick_input_phase` so tick orchestration no longer owns input event plumbing.
- [x] Extract post-gameplay controller sync + camera follow + render dispatch from `runtime/engine_tick.c` into `runtime/tick_post_phase`.

### Phase 14: Feature 005 Governance And Modularization Ledger
- [x] Activate `.specify/specs/005-native-runtime-render-modularization/` as the governing feature for ongoing native runtime/render modularization.
- [x] Treat `.specify/specs/005-native-runtime-render-modularization/tasks.md` as the dependency-ordered execution ledger for remaining runtime/render concern splits.
- [x] Enforce the constitution rule that native implementation files should stay small and single-responsibility, with `CMakeLists.txt` as the only standing large-file exception.
- [ ] Capture feature-005 evidence artifacts under `artifacts/native/005-*.txt` as the remaining DX12 diagnostics and architecture-guard slices land.
