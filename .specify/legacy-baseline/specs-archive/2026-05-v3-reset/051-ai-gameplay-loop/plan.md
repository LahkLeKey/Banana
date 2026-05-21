# Implementation Plan: Spec 051 Native+AI+WASM Rehydration

## Baseline Context

- Date: 2026-05-15
- Active feature: `.specify/specs/051-ai-gameplay-loop`
- Primary execution gap: AI loop edits exist, but runtime contract is broken by root native build drift and non-orchestrated WASM artifact lifecycle.

## Imported Baseline (from Spec 050)

- Multiplayer server-authoritative contracts are treated as binding constraints.
- Completed Spec 050 articles and tests are accepted as inherited evidence baseline.
- Remaining parity follow-up from Spec 050 remains tracked as non-blocking in unified execution.

## Objective

Ship a reproducible web-playable AI loop where engine, wrapper/native library, and WASM artifacts are all built from current migrated domains and launched by canonical runtime channels.

## Phased Plan

### Phase 1: Native Contract Rebind (Blocking)

1. Replace root `CMakeLists.txt` references to deleted legacy native files with current migrated domain sources.
2. Preserve `banana_native` target and wrapper ABI contract surface.
3. Restore missing anti-cheat shared domain type contracts used by migrated service headers.

### Phase 2: Wrapper-to-Domain Compatibility Layer (Blocking)

1. Replace `banana_wrapper.c` dependencies on removed `banana_core_*` and `banana_dal_*` implementations.
2. Route wrapper exports to active domain services (calc, batch, truck, ripeness, anticheat, ML wrapper modules).
3. Use compatibility adapters where domain service migrations are incomplete, with explicit TODO markers.

### Phase 3: Engine AI Loop Finalization

1. Ensure ML controller type registration and spawn path for non-player entities.
2. Ensure per-frame movement decisions are produced through ML bridge APIs.
3. Ensure entity sync back to world state and visible rendering path.
4. Ensure histogram tracks AI action and player input channels.

### Phase 4: WASM Build and Publish Orchestration (Web Viewport)

1. Standardize engine WASM build source-of-truth (CMake Emscripten target and/or script wrapper around it).
2. Ensure generated artifacts (`engine.js`, `engine.wasm`, `engine.version.json`) land in React web public path.
3. Integrate artifact generation into runtime channel orchestration (compose/scripts) so web viewport does not depend on manual local steps.
4. Enforce WASM parallel build contract (`-pthread`, `USE_PTHREADS`, pool-size budget, OpenMP-enabled worldgen loops).
5. Add CI hard gate that fails when WASM parallel budgets/contracts are not met.

### Phase 5: Runtime Validation and Spec Evidence

1. Validate root native build success.
2. Validate compose runtime profile startup success.
3. Validate web viewport AI movement behavior.
4. Record outcomes, residual risks, and follow-ups in heartbeat log.

## Exit Criteria

- Runtime channels no longer fail due to deleted legacy native source paths.
- Web viewport can load current WASM artifacts and display AI-driven actor motion.
- Spec artifacts remain synchronized and sufficiently detailed to resume work without transcript context.
