---
spec_id: 002-phase1-npc-wildlife-controller
title: "Phase 1: NPC Wildlife Controller (Rendering + Physics + Simple AI)"
status: draft
created: 2026-05-06
depends_on: [001-game-engine-architecture]
---

# Phase 1 Implementation Plan

## Objective

Deliver a **complete game engine foundation** with rendering, physics, and a sample NPC controller pattern. This establishes the architecture and validates the C↔WASM interface before expanding mechanics.

## Scope

### What's In

1. **C Rendering Pipeline**
   - OpenGL context + GLFW window
   - Basic mesh rendering (cube, sphere primitives)
   - Frame buffer capture to shared memory
   - Camera management (fixed + orbital)

2. **C Physics Engine**
   - Rigid body simulation (box shapes)
   - Gravity + basic collision response
   - Ray casting for terrain queries

3. **NPC Wildlife Controller**
   - State machine: idle → patrol → investigate → idle
   - Grid-based pathfinding
   - Spawned as 3-5 instances in a simple world
   - Responds to "player_nearby" signal

4. **WASM Game Loop**
   - Main tick orchestrator
   - Calls physics_step → controller_update → render_frame
   - Frame timing + delta time calculation
   - Minimal telemetry (frame count, entity count)

5. **Integration Tests**
   - C engine compiles and initializes
   - WASM calls into C rendering
   - Physics simulation is deterministic
   - Controller state persists across frames

### What's Out (Phase 2+)

- Advanced rendering (skeletal animation, normal maps, lighting)
- Complex physics (ragdoll, fluid)
- Multiple AI controller types
- Procedural world generation
- Multiplayer/networking
- UI overlays (handled by existing React frontends)

## Implementation Phases

### Phase 1a: Rendering Foundation (Week 1)

**Tasks:**
1. Create `src/native/engine/render/` module structure
   - `window.c/h` – GLFW window init, main loop management
   - `shader.c/h` – GLSL compilation, uniforms
   - `mesh.c/h` – Vertex/index buffers, mesh definitions
   - `renderer.c/h` – Main render orchestrator

2. Set up OpenGL context
   - GLFW window (800×600, resizable)
   - OpenGL 3.3+ core profile
   - Simple default shader (vertex + fragment)

3. Render primitives
   - Cube mesh (24 vertices, indexed)
   - Sphere mesh (generated at runtime)
   - Basic material (color + position)

4. Frame buffer export
   - Render to default framebuffer
   - Read back RGBA data
   - Make accessible to WASM via shared memory (offset 0x00000)

**Success:** Frame of a rotating cube displays when called from native test

**Estimated Effort:** 3-4 days (CMake integration, GLFW setup, shader pipeline)

---

### Phase 1b: Physics Integration (Week 1-2)

**Tasks:**
1. Create `src/native/engine/physics/` module structure
   - `body.c/h` – Rigid body definitions
   - `collider.c/h` – Collision primitives (box, sphere)
   - `dynamics.c/h` – Force/impulse resolution (simple Euler integration)
   - `world.c/h` – World state + gravity

2. Implement rigid body simulation
   - Position, velocity, mass, shape
   - Apply forces (gravity, impulse)
   - Frame-independent timestep (fixed 60 Hz physics tick)

3. Collision detection + response
   - AABB vs AABB broad phase
   - Box vs sphere narrow phase
   - Simple impulse-based resolution

4. Expose to WASM
   - `physics_world_step(dt)`
   - `physics_add_body(id, mass, x, y, z, shape)`
   - `physics_update_body(id, position, velocity)`

**Success:** 
- Physics box falls under gravity
- Collision stops box at floor
- Deterministic: same sequence replays identically

**Estimated Effort:** 3-4 days (collision logic, impulse math, determinism validation)

---

### Phase 1c: NPC Controller Pattern (Week 2)

**Tasks:**
1. Create `src/native/engine/ai/` module structure
   - `controller.c/h` – Base controller interface
   - `state_machine.c/h` – Simple FSM
   - `navigation.c/h` – Pathfinding helpers
   - `perception.c/h` – Raycast/proximity queries

2. Wildlife controller implementation
   - **States:** idle, patrol, investigate, return_to_patrol
   - **Idle:** Stand in place, wait 5-10 seconds
   - **Patrol:** Walk along waypoint path
   - **Investigate:** Move toward last "player_nearby" signal
   - **Return:** Walk back to patrol start
   - **Transitions:** Time-based or signal-triggered

3. Grid-based pathfinding
   - Simple 20×20 grid of world
   - Mark obstacles/passable cells
   - A* or simple greedy pathfinding to waypoints

4. Perception signals
   - `controller_signal(id, "player_nearby", data)` – NPC enters alert
   - `world_send_signal(id, "death")` – NPC dies
   - Signals trigger state transitions

5. Expose to WASM
   - `controller_create(type, x, y, z)` → returns ID
   - `controller_update(id, dt)` → returns new state
   - `controller_signal(id, signal_name)` → triggers behavior

**Success:**
- 3 NPC instances spawned
- Each patrols independently
- Send "player_nearby" signal → all enter investigate state
- State changes visualized in rendered positions

**Estimated Effort:** 4-5 days (state machine logic, pathfinding, signal wiring)

---

### Phase 1d: WASM Game Loop (Week 2-3)

**Tasks:**
1. Create `src/typescript/api/wasm/game-loop.wasm.ts`
   - Import C functions (via WASM imports)
   - Implement main game loop:
     ```typescript
     async function tick(dt: number) {
       engine.physicsStep(dt);
       engine.updateControllers(dt);
       engine.renderFrame();
       if (frameCount % 60 === 0) {
         api.emit_telemetry('game_tick', { ...state });
       }
     }
     ```

2. Link with C engine
   - CMake target: `wasm-game-loop` (exports .wasm module)
   - WASM imports all C functions
   - Shared linear memory for frame buffer + state

3. Test harness
   - Run game loop for 60 frames
   - Verify physics progresses
   - Verify controller states change
   - Verify frame buffer updates

**Success:**
- WASM module loads
- Game loop runs 60 frames deterministically
- Frame times logged (should be ~16ms @ 60 FPS)

**Estimated Effort:** 2-3 days (WASM toolchain, import/export wiring, harness)

---

### Phase 1e: Integration & Telemetry (Week 3)

**Tasks:**
1. End-to-end test
   - Render loop runs continuously
   - Physics and controllers update
   - 5 NPCs patrol the world
   - Scene captures to frame buffer

2. Save/load cycle
   - Serialize entity positions + controller states
   - Export to JSON
   - Load, verify deterministic replay

3. API telemetry
   - Send game state snapshot every 1 second
   - Track: frame count, entity count, controller states
   - Store in Postgres (new `game_state_telemetry` table)

4. React frontend integration
   - Display live entity positions on map
   - Show controller state per NPC
   - Real-time telemetry feed (WebSocket optional)

**Success:**
- Full cycle: render → physics → AI → save → telemetry → frontend
- No crashes over 5-minute run
- Save/load produces identical replay
- React dashboard shows live world state

**Estimated Effort:** 3-4 days (integration testing, API endpoints, frontend wiring)

---

## Dependencies & Sequencing

```
Phase 1a (Rendering)
    ↓
Phase 1b (Physics) [runs parallel with 1a if split across team]
    ↓
Phase 1c (AI Controller) [can start when rendering + physics export → WASM]
    ↓
Phase 1d (WASM Loop) [integrates all three]
    ↓
Phase 1e (Integration & Telemetry)
```

**Estimated Total Duration:** 3-4 weeks (single developer), 2 weeks (2-3 devs split by module)

## Risk Mitigations

| Risk | Mitigation |
|------|-----------|
| WASM/C interop complex | Start with simple C→WASM function calls; validate memory layout early |
| Determinism loss due to floating-point rounding | Use fixed-point for physics, log state snapshots for replay validation |
| OpenGL context issues on test runners | Use offscreen rendering (FBO) on headless systems; validate on dev machine first |
| Pathfinding inefficiency | Start with grid-based A*; optimize only if frame time > 5ms per tick |
| Controller state explosion | Keep state machine simple (4 states max); add complexity in Phase 2 |

## Success Criteria (Phase 1 Complete)

- [ ] C engine builds with `cmake --build build` (Linux/Windows/Mac)
- [ ] OpenGL renders first frame (cube rotation)
- [ ] Physics simulation is deterministic (replay produces identical results)
- [ ] 5 NPC instances patrol a 20×20 grid world
- [ ] WASM game loop orchestrates physics + AI + rendering
- [ ] Full tick cycle completes in < 16ms (60 FPS)
- [ ] Save/load cycle preserves controller state
- [ ] Telemetry events flow to ASP.NET API
- [ ] React frontend displays live entity positions + state
- [ ] 0 test failures (native + integration)

## Deliverables

1. **C Engine Core** (`src/native/engine/`)
   - render/, physics/, ai/, world/ modules
   - CMake targets for static library + WASM exports
   - ~3000 lines of C code

2. **WASM Game Loop** (`src/typescript/api/wasm/game-loop.wasm.ts`)
   - ~500 lines of TypeScript
   - Shared memory management
   - Telemetry emission

3. **Integration Tests** (`tests/native/phase1-engine-tests.c` + `tests/integration/phase1-engine.spec.ts`)
   - C unit tests (rendering, physics, AI)
   - WASM integration harness (60-frame simulation)
   - Determinism validation

4. **API Endpoints**
   - `POST /api/game/telemetry`
   - `POST /api/game/save`, `GET /api/game/load/:id`
   - New `game_state_telemetry` schema

5. **React Component**
   - World map view (2D top-down)
   - NPC status display
   - Telemetry time-series graph

## Next Steps (After Phase 1)

- Phase 2: Expand AI (predator/prey, learning state)
- Phase 3: Procedural world generation
- Phase 4: Advanced rendering (skeletal animation, lighting)
- Phase 5: Multiplayer synchronization
