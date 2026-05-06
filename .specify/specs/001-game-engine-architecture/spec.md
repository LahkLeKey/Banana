---
spec_id: 001-game-engine-architecture
title: "Game Engine Architecture: C Core + WASM Orchestration"
status: draft
created: 2026-05-06
updated: 2026-05-06
---

# Game Engine Architecture Specification

## Vision

Transform Banana from an AI classification system into a **fully-featured game engine** with:
- **C-native implementation** for rendering, physics, and AI (no high-level abstractions)
- **Per-mechanic AI controllers** replacing the monolithic classifier
- **WASM orchestration layer** as the top-level game loop
- **Open world mechanics** driven by individual, reusable controller instances
- **Homebrew rendering pipeline** (OpenGL/native graphics, no Three Fiber)

## Strategic Context

**From:** Single-purpose AI classifier (banana detection)  
**To:** Extensible game engine for open-world mechanics  
**Driver:** Full native control over rendering, physics, and AI behavior patterns

## Architecture Layers

### Layer 1: C Engine Core (`src/native/engine/`)

The foundation of all runtime behavior, split into focused modules:

#### 1.1 Rendering Module (`engine/render/`)
- **Purpose:** Homebrew OpenGL rendering pipeline
- **Components:**
  - `window.c/h` – Window creation, lifecycle management (GLFW or raw X11/Win32)
  - `shader.c/h` – Shader compilation, binding, uniform management
  - `mesh.c/h` – Mesh definition, vertex/index buffer management
  - `material.c/h` – Material/texture abstraction
  - `camera.c/h` – View/projection matrix calculations
  - `renderer.c/h` – Main render loop orchestrator (receives draw calls, outputs frames)
- **Key Functions:**
  - `render_init(width, height)` – Initialize rendering context
  - `render_frame()` – Execute single frame (clear, draw, present)
  - `render_draw_mesh(mesh, transform, material)` – Queue mesh for rendering
  - `render_shutdown()` – Cleanup
- **Output:** Raw frame buffer; rendered frames available to WASM via shared memory
- **No Dependencies:** No Three Fiber, no abstraction layer; direct OpenGL calls

#### 1.2 Physics Module (`engine/physics/`)
- **Purpose:** Deterministic physics simulation
- **Components:**
  - `body.c/h` – Rigid body definitions (position, velocity, mass, shape)
  - `collider.c/h` – Collision detection (AABB, sphere, capsule)
  - `dynamics.c/h` – Force/impulse resolution
  - `world.c/h` – Physics world state, step management
- **Key Functions:**
  - `physics_world_create()` – Create physics world
  - `physics_body_create(mass, shape)` – Add rigid body
  - `physics_world_step(dt)` – Advance simulation by delta time
  - `physics_query_raycast(origin, direction)` – Collision queries
- **Properties:** Frame-rate independent, deterministic for replay/sync

#### 1.3 AI Controller Framework (`engine/ai/`)
- **Purpose:** Reusable controller pattern for game mechanics
- **Components:**
  - `controller.c/h` – Base controller interface (init, update, destroy)
  - `state_machine.c/h` – Simple FSM for controller behavior (idle, patrol, pursue, etc.)
  - `perception.c/h` – Sense queries (raycast, proximity, signal)
  - `navigation.c/h` – Basic pathfinding, movement helpers
- **Key Types:**
  ```c
  typedef struct {
    uint32_t id;
    char name[256];
    void *state;  // Controller-specific state
    float (*update)(struct ControllerInstance *self, float dt);
    void (*on_signal)(struct ControllerInstance *self, const char *signal);
    void (*destroy)(struct ControllerInstance *self);
  } ControllerInstance;
  ```
- **Lifecycle:** Controllers are created per NPC/wildlife/mechanic instance; WASM requests controller updates per frame

#### 1.4 World State Module (`engine/world/`)
- **Purpose:** Manage game objects, entities, and spatial queries
- **Components:**
  - `entity.c/h` – Entity definition (ID, position, controller binding)
  - `world.c/h` – World state (entity list, grid-based spatial index)
  - `signals.c/h` – Inter-controller event system (damage, death, interaction)
- **Key Functions:**
  - `world_create()` – Initialize world
  - `world_spawn_entity(type, position)` – Add entity
  - `world_tick(dt)` – Update all entities
  - `world_query_nearby(position, radius)` – Spatial queries
  - `world_send_signal(target_id, signal_name, data)` – Controller messaging

### Layer 2: WASM Orchestration (`src/typescript/api/wasm/`)

Top-level game loop and state coordination:

#### 2.1 Game Loop (`game-loop.wasm.ts`)
- **Responsibilities:**
  - Main tick orchestrator (call C functions in sequence)
  - Frame timing and delta time calculation
  - State synchronization between C runtime and WASM
  - API telemetry emission (frame time, entity count, etc.)
- **Pseudocode:**
  ```typescript
  async function gameLoop(dt: number) {
    // 1. Physics step
    engine.physicsStep(dt);
    
    // 2. Update all controllers
    for (const controller of world.controllers) {
      controller.update(dt);
    }
    
    // 3. Render frame
    engine.renderFrame();
    
    // 4. Emit telemetry (optional, sparse)
    if (frameCount % 60 === 0) {
      api.emit_telemetry('game_tick', { entity_count, frame_ms });
    }
  }
  ```
- **Inputs:** Controller updates, physics queries
- **Outputs:** Rendered frames, telemetry events

#### 2.2 Game State Bridge (`game-state-bridge.ts`)
- **Purpose:** Serialize/deserialize game state for API persistence
- **Functions:**
  - `serialize_world_state()` – Export to JSON (for save games, replays)
  - `deserialize_world_state(json)` – Load from JSON
  - `sync_to_api()` – Send significant state changes to ASP.NET API

### Layer 3: Existing Infrastructure (Unchanged)

#### 3.1 ASP.NET API (`src/c-sharp/asp.net/`)
- **New Responsibility:** Game engine state persistence
- **Endpoints:**
  - `POST /api/game/save` – Persist world state
  - `GET /api/game/load/:save_id` – Retrieve saved world
  - `POST /api/game/telemetry` – Accept game telemetry events
  - `GET /api/game/static-data` – Serve world configuration, controller definitions
- **No Breaking Changes:** Existing ML/classification endpoints remain (separate concerns)

#### 3.2 React/Electron/Mobile Frontends
- **Role:** Game state consumers (HTTP clients)
- **New:** Display game telemetry, world state, controller status
- **Integration:** Real-time game state feed from API (polling or WebSocket)

## C ↔ WASM Interface Contract

### ABI Exports (C → WASM)

All C functions accessible to WASM must be marked `WASM_EXPORT`:

```c
WASM_EXPORT float engine_tick(float dt);
WASM_EXPORT void engine_render_frame(void);
WASM_EXPORT uint8_t* engine_get_frame_buffer(void);

WASM_EXPORT void physics_world_step(float dt);
WASM_EXPORT void physics_add_body(PhysicsBodyId id, float mass, float x, float y, float z);

WASM_EXPORT ControllerInstanceId controller_create(const char *type, float x, float y, float z);
WASM_EXPORT float controller_update(ControllerInstanceId id, float dt);
WASM_EXPORT void controller_signal(ControllerInstanceId id, const char *signal_name);

WASM_EXPORT EntityId world_spawn_entity(EntityType type, float x, float y, float z);
WASM_EXPORT void world_tick(float dt);
```

### Memory Layout (Shared Memory Region)

- Offset 0x00000: Frame buffer (e.g., 1920×1080×4 = 8.3 MB)
- Offset 0x800000: Entity state array (compressed XYZ + flags)
- Offset 0x900000: Physics world state snapshot
- Offset 0xA00000: Events/signals queue (for inter-controller messaging)

### Calling Convention

- WASM calls C via direct wasm32 imports
- C returns primitive types or pointers to shared memory
- String data passed as (offset, length) pairs in linear memory

## Data Structures (Preliminary)

### Entity
```c
typedef struct {
  uint32_t id;
  EntityType type;
  float position[3];
  float rotation[4];  // quaternion
  ControllerInstanceId controller_id;  // 0 if none
  uint32_t flags;
} Entity;
```

### Controller Instance
```c
typedef struct {
  uint32_t id;
  char type_name[256];
  void *state;  // Type-specific state
  float (*update)(struct ControllerInstance *self, float dt);
  void (*on_signal)(struct ControllerInstance *self, const char *signal);
} ControllerInstance;
```

## Phase 1 Deliverables

### Milestone 1: Rendering Foundation
- OpenGL context + window management
- Basic mesh/material rendering
- Frame buffer export to WASM
- First render call from WASM game loop

### Milestone 2: Physics Integration
- Rigid body simulation
- Collision detection
- Physics-driven entity motion

### Milestone 3: Sample Controller (NPC Wildlife)
- Simple patrol/idle state machine
- Collision-based pathfinding (grid-based)
- Interaction signal handling (e.g., "player_nearby")
- Spawned as multiple instances in world

### Milestone 4: Integration & Telemetry
- WASM game loop orchestrating all three
- Frame time telemetry to API
- Entity/controller state serialization
- Save/load cycle validated

## Unknowns & Decisions (Post-Phase-1)

- Rendering detail level (skeleton rigging, animation blending)
- Physics breadth (ragdoll, cloth, fluid dynamics)
- Multiplayer synchronization strategy
- Controller complexity (behavior trees, learning state)
- Terrain/world generation approach

## Success Criteria

1. ✓ C engine compiles with CMake
2. ✓ WASM module successfully links C exports
3. ✓ First frame rendered and captured
4. ✓ Physics objects move deterministically
5. ✓ NPC controller instances patrol a simple world
6. ✓ Full tick cycle completes in WASM orchestrator
7. ✓ Telemetry events flowing to API
8. ✓ Save/load cycle preserves controller state
