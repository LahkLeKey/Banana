# Native DDD/SOLID Scan: Spec 055

Date: 2026-05-19
Scope: src/native C codebase

## Largest Native Files (Primary Split Targets)

1. src/native/engine/engine.c (~1041 lines)
2. src/native/engine/render/mesh.c (~368 lines)
3. src/native/engine/render/renderer.c (~350 lines)
4. src/native/engine/render/window.c (~294 lines)
5. src/native/engine/tools/domain/banana_asset_algorithms.c (~272 lines)
6. src/native/engine/ai/wildlife_controller.c (~233 lines)
7. src/native/engine/tools/banana_asset_compiler.c (~226 lines)
8. src/native/engine/physics/collider.c (~213 lines)

## Domain Responsibility Clusters

### Runtime Orchestration Domain
- Current hub: src/native/engine/engine.c
- Mixed concerns currently present:
  - Tick orchestration
  - Terrain lifecycle and chunk rebuild budget
  - Player binding registry and sync
  - Camera follow behavior
  - Rendering submission loop
  - Misc math/helpers and exported diagnostics

### Rendering Backend Domain
- Current files: src/native/engine/render/mesh.c, src/native/engine/render/renderer.c, src/native/engine/render/window.c
- Concerns:
  - API-facing render orchestration mixed with backend-specific details
  - Harder portability path for future renderer swap

Status update:
- Mesh domain decomposed from monolith into `render/mesh.c`, `render/mesh_banana.c`, and `render/mesh_terrain.c` with OpenMP-aware terrain mesh build loops.
- Added render-focused native regression (`tests/native/render_mesh_terrain_test.c`) to keep terrain mesh contract covered after split.
- Renderer orchestration split by extracting draw path into `render/renderer_draw.c` with shared internal contracts in `render/renderer_internal.h`, reducing `render/renderer.c` to frame/lifecycle concerns.
- Runtime terrain generation now consumes asset-domain tile contracts (`banana_tile_defs`, `banana_pick_tile_for_biome`) so terrain layering stays aligned with WFC/cellular biome semantics instead of independent hash-only buckets.
- World domain decomposition progressed by extracting `world_query_nearby` into `world/world_query.c`, isolating spatial query policy from world lifecycle functions in `world/world.c`.
- Physics domain decomposition progressed by extracting collision-pair resolution to `physics/world_collision.c`, replacing `physics/world.c` nested lookup loops with indexed resolution orchestration.
- Engine orchestration decomposition progressed by extracting camera basis and terrain height sampling helpers into runtime modules (`runtime/camera_basis.c`, `runtime/terrain_height.c`) and reducing duplicated math concerns in `engine.c`.
- Engine/runtime decomposition progressed by extracting controller dispatch (`runtime/controller_runtime.c`) and world metrics (`runtime/world_metrics.c`), shrinking controller/state query logic in `engine.c` and adding an OpenMP-backed runtime metric path.
- Engine ABI helper decomposition progressed with `runtime/world_telemetry.c` and `runtime/input_contract.c`, removing direct telemetry/index guards and inert input contract logic from `engine.c` in favor of focused runtime modules.
- Engine ABI decomposition progressed with `runtime/physics_abi.c` and `runtime/world_abi.c`, isolating physics/world bridge orchestration from the `engine.c` singleton host.
- Engine lifecycle decomposition progressed with `runtime/engine_lifecycle.c`, moving terrain build/bootstrap actor setup and shutdown cleanup concerns out of `engine.c`.
- Controller attach/create orchestration moved into `runtime/controller_attach.c`, reducing direct controller table mutation in `engine.c`.
- Tick orchestration host logic moved into `runtime/engine_tick.c`, reducing `engine.c` phase sequencing to a runtime module call boundary.
- Terrain host chunk indexing/rebuild wrappers moved into `runtime/terrain_host.c`, further shrinking terrain host concerns in `engine.c`.
- Player-facing ABI delegation moved into `runtime/player_api.c`, isolating player upsert/input/transform orchestration from the singleton host.
- Render material policy moved from `engine.c` into `runtime/render_material.c` to isolate entity-type visual mapping.
- Player motion host orchestration (camera-basis fallback + binding collection) moved into `runtime/player_motion_host.c`, shrinking residual movement orchestration in `engine.c`.
- Engine frame-buffer/render wrappers and singleton reset concerns moved into `runtime/engine_host.c`, removing remaining non-domain host utility logic and dead mat4 link retention from `engine.c`.
- Parallel baseline strengthened via `runtime/interlock.c` interlock context and map-based controller sync in `runtime/controller_sync.c`, reducing high-contention nested lookup cost while preserving thread-safe shared writes.
- Parallel runtime contract tightened: controller sync disable paths now fail-fast and no single-thread fallback is permitted in `runtime/controller_sync.c`.
- Controller sync parallel contract strengthened further: entity synchronization now uses strict map-only lookup path (no serial nested scan fallback) with fail-fast guards on critical parallel resources.
- Parallel contract is now single-path by construction for controller sync: runtime toggle APIs were removed, leaving no alternate non-parallel runtime branch.
- Added explicit parallel compute model verification via `runtime/parallel_model.c` and `runtime_parallel_amdahl_test.c` (Amdahl speedup, efficiency, monotonicity, asymptotic bound, and serial-fraction inference checks).
- Extended Amdahl enforcement from single-model math to all known native OpenMP kernels using `runtime/parallel_contract.c` inventory + `runtime_parallel_contract_test.c` gate (per-kernel speedup/efficiency floors and global mean-speedup floor).

### AI/Controller Domain
- Current files: src/native/engine/ai/wildlife_controller.c and runtime sync logic
- Concerns:
  - Controller policy is separate, but mirror/update orchestration crosses domains

Status update:
- Controller system orchestration split into lifecycle and dispatch modules (`ai/controller_system_lifecycle.c`, `ai/controller_system_dispatch.c`).
- Dispatch paths now use explicit OpenMP scheduling for update and signal fan-out loops.

### Asset Generation Domain
- Current files: src/native/engine/tools/domain/banana_asset_algorithms.c and tool entrypoint files
- Concerns:
  - Strong algorithmic core already exists, but test surface needs expansion

Status update:
- `banana_asset_algorithms.c` is now slimmed to contract wiring.
- WFC generation and cellular automata logic are split into focused modules (`banana_asset_wfc.c`, `banana_asset_cellular.c`) with OpenMP-enabled execution paths.

### Physics/Collision Domain
- Current file: src/native/engine/physics/collider.c
- Concerns:
  - Good candidate for focused domain tests and perf gates

Status update:
- Collision domain split completed into focused modules (`collider_narrow_phase.c`, `collider_broad_phase.c`, `collider_resolve.c`) with OpenMP-enabled broad-phase candidate enumeration.
- Added native regression coverage for broad-phase pair detection (`tests/native/collider_broad_phase_test.c`).
- Raycast path extracted from `physics/world.c` into `physics/raycast.c` with OpenMP-guided parallel nearest-hit search and dedicated native regression test (`tests/native/physics_raycast_test.c`).

## DDD/SOLID Split Map (Recommended)

## Priority 1: Engine Hub Decomposition

1. src/native/engine/runtime/tick_orchestrator.c
- Owns phase sequencing only
- No direct policy/math/interop logic beyond orchestration

Status: Partially complete via `runtime/tick_phases.c` + `runtime/render_submit.c` + `runtime/camera_follow.c` + `runtime/player_registry.c` + `runtime/player_motion.c` extraction seams.

2. src/native/engine/runtime/terrain_runtime.c
- Owns terrain build/draw/chunk budget and dirty region handling
- Exposes narrow API to tick orchestrator

Status: Implemented via `runtime/terrain_runtime.c` plus prior `runtime/terrain_generation.c`, `runtime/terrain_mutation.c`, and `runtime/tick_phases.c` seams.

3. src/native/engine/runtime/player_registry.c
- Owns GUID-to-entity binding, lifecycle, and pending inputs
- Keeps multiplayer ownership native-first

4. src/native/engine/runtime/camera_follow.c
- Owns camera basis and follow policy
- Isolated for renderer-adapter portability

5. src/native/engine/runtime/render_submit.c
- Owns entity traversal/material mapping/draw submit
- Keeps simulation and render-adapter seams explicit

## Priority 2: Renderer Boundary Hardening

6. src/native/engine/render/backend_gl/*.c
- Move GL-specific implementation behind backend adapter boundary

7. src/native/engine/render/backend_stub/*.c
- Keep strict fallback policy decisions explicit where needed

## Priority 3: Math and Utility Isolation

8. src/native/engine/math/mat4.c and src/native/engine/math/vec3.c
- Move utility math out of engine.c
- Enable direct unit tests without runtime boot

## Parallel Interlocked Execution Notes

- Maintain strict parallel mode as default/required where policy specifies (already active in controller sync).
- Keep phase boundaries deterministic and explicit even when execution is parallelized.
- Introduce interlocks only at clear boundaries:
  - controller update complete before entity mirror pass
  - simulation phase complete before render submit
  - terrain chunk rebuild budget complete before render draw call path
- Avoid cross-phase hidden mutation by routing writes through owning domain module APIs.

## Test Expansion Plan (Native)

Add or expand tests in tests/native:

1. tests/native/runtime_tick_orchestrator_test.c
- Verifies deterministic phase order and fail-fast invariants

2. tests/native/runtime_player_registry_test.c
- Verifies GUID upsert, reconnect, deactivate, and stale lifecycle behavior

3. tests/native/runtime_controller_sync_test.c
- Verifies strict parallel enabled contract and entity mirror consistency

4. tests/native/runtime_terrain_budget_test.c
- Verifies chunk budget and dirty-region behavior

5. tests/native/runtime_camera_follow_test.c
- Verifies camera target consistency and edge-case safety

6. tests/native/runtime_render_submit_test.c
- Verifies active entity traversal and submission contract shape

7. tests/native/math_mat4_test.c
- Verifies extracted matrix helpers in isolation

8. tests/native/collider_perf_test.c
- Verifies collision throughput baseline and regression guard

## Recommended Execution Order

1. Complete runtime terrain and player registry extraction from engine.c
2. Extract render submit and camera follow
3. Extract math utilities
4. Introduce/expand native tests per module
5. Run native coverage flow and enforce gate

## Coverage and Evidence

- Command: scripts/run-native-c-tests-with-coverage.sh
- Gate: maintain at least 80% line coverage while increasing coverage for extracted modules
- Evidence: append per-slice outcomes to .specify/specs/055-c-runtime-parallel-foundation/heartbeat-log.md
