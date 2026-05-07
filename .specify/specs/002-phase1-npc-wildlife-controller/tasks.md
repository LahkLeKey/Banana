---
spec_id: 002-phase1-npc-wildlife-controller
title: "Phase 1 Implementation Tasks: NPC Wildlife Controller"
description: "Granular, dependency-ordered task breakdown for Phase 1 (Rendering + Physics + NPC AI + WASM Loop)"
status: ready
created: 2026-05-06
---

# Phase 1 Tasks: NPC Wildlife Controller

**Feature**: Game Engine Architecture - Phase 1 (Rendering Foundation → Physics → NPC Controller → WASM Loop → Integration)
**Scope**: Reference [Phase 1 Implementation Plan](./plan.md)
**Effort Estimate**: 3-4 weeks (1 dev), 2 weeks (2-3 devs split by module)
**Team Size**: 1-3 developers (parallelizable by module)

---

## Phase 0: Setup (Project Infrastructure)

**Purpose**: Initialize CMake structure and dependencies for C engine + WASM integration
**Duration**: 1 day
**Blocker**: None – can start immediately

- [x] T001 Create `src/native/engine/` directory structure with `render/`, `physics/`, `ai/`, `world/` subdirectories

- [x] T002 Create `src/native/engine/CMakeLists.txt` with targets for:
  - `banana_engine_core` (static library, C files)
  - `banana_engine_wasm` (WASM exports from C)
  - Build flags: `-std=c11`, `-O2`, `-Wall -Wextra`, position-independent code
  - Reference: [Phase 1a plan](./plan.md#phase-1a-rendering-foundation-week-1)

- [x] T003 [P] Add GLFW, OpenGL, and Emscripten toolchain to root `CMakeLists.txt`:
  - Linux: apt-get glfw3-dev, libgl1-mesa-dev
  - macOS: brew install glfw
  - Windows: vcpkg GLFW, OpenGL headers
  - WASM: Emscripten SDK (if not already present)
  - Document in [BUILD.md](../../../BUILD.md) or root `README.md`

- [x] T004 [P] Create `tests/native/phase1-engine-tests.c` with test framework setup:
  - Include minunit.h or similar C test harness
  - Stub functions for rendering, physics, AI tests
  - Build target: `test_phase1_engine` linked to `banana_engine_core`
  - Reference: [Integration Tests section](./plan.md#integration-tests)

**Success**:
- Directory structure matches [Architecture Layers in spec](../../001-game-engine-architecture/spec.md#architecture-layers)
- CMake builds successfully (`cmake --build build`)
- Test harness compiles

**Validation**:
```bash
cd /c/Github/Banana
cmake -B build
cmake --build build --target test_phase1_engine
# Should compile with 0 errors
```

---

## Phase 1a: Rendering Foundation (Week 1)

**Goal**: OpenGL rendering pipeline that outputs RGBA frames to shared memory
**Independent Test**: Render a rotating cube; verify frame buffer contains non-zero RGBA data
**Estimated Duration**: 3-4 days (split across 2 developers is 1.5-2 days)
**Parallelizable**: Window + Shader setup can run in parallel with Mesh + Renderer orchestration

### Tasks

#### Subtask 1a.1: Window & OpenGL Context

- [x] T005 Create `src/native/engine/render/window.h` with:
  ```c
  typedef struct Window Window;
  Window* window_create(int width, int height, const char *title);
  int window_is_open(Window *w);
  void window_poll_events(Window *w);
  void window_swap_buffers(Window *w);
  void window_destroy(Window *w);
  ```
  - Reference: [Rendering Module spec](../../001-game-engine-architecture/spec.md#11-rendering-module-enginerender)

- [x] T006 Implement `src/native/engine/render/window.c`:
  - Use GLFW for cross-platform window creation
  - Initialize OpenGL 3.3+ core profile context
  - Set viewport to window size
  - Enable VSync
  - Window size: 800×600 (resizable)
  - Error handling: Log GLFW/GL errors to stderr
  - **Success Criteria**:
    - Window opens without crash
    - OpenGL version logged (should be ≥ 3.3)
    - GLFW callbacks registered for resize, input
  - **Risk Mitigation**: Test on headless CI with offscreen FBO fallback (xvfb-run on Linux)

#### Subtask 1a.2: Shader Pipeline

- [x] T007 [P] Create `src/native/engine/render/shader.h` with:
  ```c
  typedef struct Shader Shader;
  Shader* shader_create(const char *vert_src, const char *frag_src);
  void shader_use(Shader *s);
  void shader_set_mat4(Shader *s, const char *name, float mat[16]);
  void shader_set_vec3(Shader *s, const char *name, float x, float y, float z);
  void shader_destroy(Shader *s);
  ```
  - Reference: [Rendering Module spec](../../001-game-engine-architecture/spec.md#11-rendering-module-enginerender)

- [x] T008 [P] Implement `src/native/engine/render/shader.c`:
  - Compile vertex + fragment shaders
  - Link into program
  - Cache uniforms (position, color, view/projection matrices)
  - Error logging: Compile errors to stderr with line numbers
  - Default shader: Simple vertex position + color → fragment output
  - **Success Criteria**:
    - Shader compiles from inline GLSL strings
    - Uniforms settable via `shader_set_*` functions
    - No GL errors on Linux/macOS/Windows

#### Subtask 1a.3: Mesh Management

- [x] T009 [P] Create `src/native/engine/render/mesh.h` with:
  ```c
  typedef struct Mesh Mesh;
  Mesh* mesh_create_cube(void);
  Mesh* mesh_create_sphere(int segments);
  void mesh_render(Mesh *m);
  void mesh_destroy(Mesh *m);
  ```

- [x] T010 [P] Implement `src/native/engine/render/mesh.c`:
  - Cube: 24 vertices (6 faces × 4 verts), indexed (36 indices)
  - Sphere: UV-sphere generation at runtime (segments = 16 default)
  - VAO/VBO/EBO management (per-mesh)
  - `mesh_render()` calls glDrawElements
  - **Success Criteria**:
    - Cube renders with correct topology
    - Sphere is visually sphere-like (16 segments min)
    - No memory leaks (validated with Valgrind)
  - **Risk Mitigation**: Cap sphere segments to 64 to prevent excessive geometry

#### Subtask 1a.4: Material & Rendering Orchestrator

- [x] T011 Create `src/native/engine/render/material.h` with:
  ```c
  typedef struct Material {
    float color[3];  // RGB
    Shader *shader;
  } Material;
  ```

- [x] T012 [P] Create `src/native/engine/render/camera.h` with:
  ```c
  typedef struct Camera Camera;
  Camera* camera_create(float fov, float aspect, float near, float far);
  void camera_set_position(Camera *c, float x, float y, float z);
  void camera_look_at(Camera *c, float eye[3], float center[3], float up[3]);
  void camera_get_view_matrix(Camera *c, float out[16]);
  void camera_get_projection_matrix(Camera *c, float out[16]);
  void camera_destroy(Camera *c);
  ```

- [x] T013 [P] Implement `src/native/engine/render/camera.c`:
  - Fixed 60° FOV, perspective projection
  - Orbital camera helper (for rotating view around a point)
  - Matrix math (no external libraries; use inline matrix ops)
  - **Success Criteria**: Camera matrices produce correct perspective

- [x] T014 Create `src/native/engine/render/renderer.h` with:
  ```c
  typedef struct Renderer Renderer;
  Renderer* renderer_create(Window *w, int frame_width, int frame_height);
  void renderer_begin_frame(Renderer *r);
  void renderer_draw_mesh(Renderer *r, Mesh *m, float pos[3], float rot[4], Material *mat);
  void renderer_end_frame(Renderer *r);
  uint8_t* renderer_get_frame_buffer(Renderer *r);  // RGBA data at shared mem offset 0x00000
  void renderer_destroy(Renderer *r);
  ```
  - Reference: [Rendering Module ABI](../../001-game-engine-architecture/spec.md#c--wasm-interface-contract)

- [x] T015 Implement `src/native/engine/render/renderer.c`:
  - Frame buffer setup (render to screen for native, FBO for WASM)
  - Draw calls: set material shader, camera matrices, mesh geometry
  - Frame buffer readback: glReadPixels to linear memory (RGBA, 8-bit)
  - **Success Criteria**:
    - Frame renders to window
    - Frame buffer readable via `renderer_get_frame_buffer()`
    - Frame data is valid RGBA (non-zero pixels)
  - **Risk Mitigation**: On headless, use offscreen FBO; on Windows/Mac, ensure context is current

#### Subtask 1a.5: Integration & Testing

- [x] T016 Add test in `tests/native/phase1-engine-tests.c`:
  ```c
  void test_rendering_pipeline() {
    Window *w = window_create(800, 600, "Test");
    Renderer *r = renderer_create(w, 800, 600);
    Camera *c = camera_create(60.0f, 800.0f/600.0f, 0.1f, 100.0f);
    Mesh *cube = mesh_create_cube();
    Material mat = {.color = {1.0f, 0.0f, 0.0f}, .shader = NULL};  // Shader from renderer

    for (int i = 0; i < 10; i++) {
      renderer_begin_frame(r);
      float pos[] = {0, 0, -3};
      float rot[] = {0, 0, 0, 1};  // identity quat
      renderer_draw_mesh(r, cube, pos, rot, &mat);
      renderer_end_frame(r);
    }

    uint8_t *fb = renderer_get_frame_buffer(r);
    assert(fb != NULL, "Frame buffer should be non-NULL");
    // Check for non-zero pixels in frame (sanity check)

    mesh_destroy(cube);
    renderer_destroy(r);
    window_destroy(w);
  }
  ```

- [x] T017 Build and run test:
  ```bash
  cd build
  ./test_phase1_engine
  ```
  - **Success**: Test passes, frame rendered without crashes

---

## Phase 1b: Physics Integration (Week 1-2)

**Goal**: Deterministic rigid-body physics with gravity, collision response, and determinism validation
**Independent Test**: Drop a box → falls under gravity → stops at floor; identical sequence replays
**Estimated Duration**: 3-4 days
**Parallelizable**: Body + Collider definitions can run in parallel with Dynamics implementation

### Tasks

#### Subtask 1b.1: Rigid Body Definitions

- [x] T018 Create `src/native/engine/physics/body.h` with:
  ```c
  typedef uint32_t PhysicsBodyId;

  typedef enum {
    SHAPE_BOX,
    SHAPE_SPHERE,
    SHAPE_CAPSULE
  } ShapeType;

  typedef struct {
    float x, y, z;
    float width, height, depth;
  } BoxShape;

  typedef struct PhysicsBody {
    PhysicsBodyId id;
    float pos[3];
    float vel[3];
    float acc[3];
    float mass;
    float friction;
    ShapeType shape_type;
    union {
      BoxShape box;
      // sphere, capsule...
    } shape;
  } PhysicsBody;

  PhysicsBody* physics_body_create(PhysicsBodyId id, float mass, float x, float y, float z, ShapeType shape);
  void physics_body_apply_force(PhysicsBody *b, float fx, float fy, float fz);
  void physics_body_apply_impulse(PhysicsBody *b, float ix, float iy, float iz);
  void physics_body_destroy(PhysicsBody *b);
  ```
  - Reference: [Physics Module spec](../../001-game-engine-architecture/spec.md#12-physics-module-enginephysics)

- [x] T019 [P] Implement `src/native/engine/physics/body.c`:
  - Store body state (position, velocity, mass)
  - Force accumulation (apply_force adds to acceleration)
  - Impulse application (instantaneous velocity change)
  - **Success Criteria**: Body properties settable and readable

#### Subtask 1b.2: Collision Detection

- [x] T020 [P] Create `src/native/engine/physics/collider.h` with:
  ```c
  typedef struct {
    PhysicsBodyId body1, body2;
    float penetration;
    float contact_point[3];
    float normal[3];
  } Collision;

  typedef struct CollisionList {
    Collision *collisions;
    int count;
  } CollisionList;

  int collider_aabb_vs_aabb(PhysicsBody *b1, PhysicsBody *b2, Collision *out);
  CollisionList collider_broad_phase(PhysicsBody *bodies, int count);
  void collider_resolve_collision(Collision *c, PhysicsBody *b1, PhysicsBody *b2);
  ```

- [x] T021 [P] Implement `src/native/engine/physics/collider.c`:
  - AABB vs AABB narrow-phase collision (box-box overlap test)
  - Broad-phase: O(n²) all-pairs (acceptable for phase 1: ≤ 20 bodies)
  - Collision normal + penetration depth calculation
  - **Success Criteria**:
    - Two boxes collide when overlapping
    - Penetration depth computed correctly
    - No false positives/negatives
  - **Risk Mitigation**: Cap body count to 50; if performance degrades, add spatial hash in Phase 2

#### Subtask 1b.3: Dynamics & Time-Stepping

- [x] T022 Create `src/native/engine/physics/dynamics.c`:
  - Euler integration: `v += a * dt`, `p += v * dt`
  - Fixed timestep: 60 Hz physics tick (dt = 1/60 ≈ 0.0167s)
  - Gravity constant: 9.81 m/s² downward
  - Impulse-based collision response:
    ```c
    void physics_resolve_collision(Collision *c, PhysicsBody *b1, PhysicsBody *b2) {
      // Compute relative velocity at contact
      float rel_vel[3] = {b2->vel[0] - b1->vel[0], ...};

      // Apply impulse along collision normal
      float impulse = -(1.0f + e) * dot(rel_vel, normal) / (1/m1 + 1/m2);
      vec3_add(b1->vel, vec3_scale(normal, impulse / b1->mass));
      vec3_add(b2->vel, vec3_scale(normal, -impulse / b2->mass));
    }
    ```
  - **Success Criteria**: Box falls and stops on collision

#### Subtask 1b.4: Physics World Management

- [x] T023 Create `src/native/engine/physics/world.h` with:
  ```c
  typedef struct {
    PhysicsBody *bodies;
    int count;
    int capacity;
    float gravity[3];
  } PhysicsWorld;

  PhysicsWorld* physics_world_create(void);
  PhysicsBodyId physics_world_add_body(PhysicsWorld *w, float mass, float x, float y, float z, ShapeType shape);
  void physics_world_step(PhysicsWorld *w, float dt);
  PhysicsBody* physics_world_get_body(PhysicsWorld *w, PhysicsBodyId id);
  void physics_world_destroy(PhysicsWorld *w);
  ```

- [x] T024 Implement `src/native/engine/physics/world.c`:
  - World state: dynamic body array (start with capacity 100)
  - `physics_world_step()`: For each body: apply forces → integrate → detect collisions → resolve
  - Gravity: Pull all non-fixed bodies downward at 9.81 m/s²
  - **Success Criteria**: Multiple bodies simulate deterministically
  - **Determinism Validation**:
    - Record initial state, run 1000 frames
    - Load initial state, run 1000 frames again
    - Positions should match byte-for-byte (or within FLT_EPSILON per axis)

#### Subtask 1b.5: Physics Testing

- [x] T025 Add physics test in `tests/native/phase1-engine-tests.c`:
  ```c
  void test_physics_falling_box() {
    PhysicsWorld *w = physics_world_create();

    // Floor (fixed, immobile)
    PhysicsBodyId floor_id = physics_world_add_body(w, 0.0f, 0, -1, 0, SHAPE_BOX);
    PhysicsBody *floor = physics_world_get_body(w, floor_id);
    floor->pos[1] = -1.0f;  // At y=-1

    // Falling box (mass 1)
    PhysicsBodyId box_id = physics_world_add_body(w, 1.0f, 0, 5, 0, SHAPE_BOX);

    // Simulate 500 frames (≈ 8.3 seconds at 60 Hz)
    for (int i = 0; i < 500; i++) {
      physics_world_step(w, 1.0f / 60.0f);
    }

    PhysicsBody *box = physics_world_get_body(w, box_id);
    assert(box->pos[1] > -1.0f && box->pos[1] < 0.0f, "Box should rest on floor");
    assert(fabs(box->vel[1]) < 0.01f, "Box velocity should be near-zero");

    physics_world_destroy(w);
  }
  ```

- [x] T026 Test determinism:
  - Run test harness, capture frame-by-frame positions
  - Reset world state, re-run, verify positions match within FLT_EPSILON
  - **Success**: Determinism test passes

---

## Phase 1c: NPC Controller Pattern (Week 2)

**Goal**: Reusable AI controller framework with state machine, pathfinding, and signal wiring
**Independent Test**: Spawn 3 NPCs, verify patrol state → send signal → all enter investigate state
**Estimated Duration**: 4-5 days
**Parallelizable**: Controller interface + State machine can run in parallel with Navigation + Perception

### Tasks

#### Subtask 1c.1: Controller Base Interface

- [x] T027 Create `src/native/engine/ai/controller.h` with:
  ```c
  typedef uint32_t ControllerInstanceId;

  typedef enum {
    CTRL_STATE_IDLE = 0,
    CTRL_STATE_PATROL = 1,
    CTRL_STATE_INVESTIGATE = 2,
    CTRL_STATE_RETURN = 3,
    CTRL_STATE_DEAD = 4
  } ControllerState;

  typedef struct ControllerInstance {
    ControllerInstanceId id;
    char type[64];
    float position[3];
    ControllerState state;
    void *state_data;  // FSM state (allocated per controller)
    float (*update)(struct ControllerInstance *self, float dt);
    void (*on_signal)(struct ControllerInstance *self, const char *signal, void *data);
    void (*destroy)(struct ControllerInstance *self);
  } ControllerInstance;

  typedef ControllerInstance* (*ControllerFactory)(ControllerInstanceId id, float x, float y, float z);

  ControllerInstance* controller_create(const char *type, ControllerInstanceId id, float x, float y, float z);
  float controller_update(ControllerInstance *c, float dt);
  void controller_signal(ControllerInstance *c, const char *signal, void *data);
  void controller_destroy(ControllerInstance *c);
  ```
  - Reference: [AI Controller Framework spec](../../001-game-engine-architecture/spec.md#13-ai-controller-framework-engineai)

- [x] T028 Implement `src/native/engine/ai/controller.c`:
  - Controller registry (type name → factory function)
  - Generic `controller_update()` / `controller_signal()` dispatch to instance methods
  - **Success Criteria**: Controllers creatable and destroyable without crashes

#### Subtask 1c.2: State Machine

- [x] T029 [P] Create `src/native/engine/ai/state_machine.h` with:
  ```c
  typedef struct {
    ControllerState current;
    float state_timer;
    int waypoint_index;
    float last_signal_pos[3];
  } FSMData;

  ControllerState fsm_update(FSMData *fsm, float dt, ControllerState next_state);
  void fsm_signal(FSMData *fsm, const char *signal);
  ```

- [x] T030 [P] Implement `src/native/engine/ai/state_machine.c`:
  - **Idle State**: Stand still for 5-10 seconds, then transition to Patrol
  - **Patrol State**: Walk to waypoints in sequence, repeat
  - **Investigate State**: Walk toward last known "player_nearby" signal position
  - **Return State**: Walk back to patrol start
  - Transitions: Time-based (idle→patrol after timer) or signal-based (any→investigate on "player_nearby")
  - **Success Criteria**:
    - State transitions occur on schedule
    - Signal triggers correct state change
    - No state machine deadlock

#### Subtask 1c.3: Navigation

- [x] T031 Create `src/native/engine/ai/navigation.h` with:
  ```c
  typedef struct {
    float waypoints[10][3];  // 10 waypoint max
    int waypoint_count;
  } Pathfinding;

  void pathfinding_init(Pathfinding *pf, float width, float depth);
  void pathfinding_add_waypoint(Pathfinding *pf, float x, float y, float z);
  int pathfinding_get_next_waypoint(Pathfinding *pf, int current_index);
  float pathfinding_move_toward(float pos[3], float target[3], float speed, float dt);
  ```

- [x] T032 [P] Implement `src/native/engine/ai/navigation.c`:
  - Simple waypoint-following (no actual A*, just linear waypoint list)
  - 20×20 grid world: 4 corners as default patrol waypoints
  - `pathfinding_move_toward()`: Linear interpolation toward target at speed
  - **Success Criteria**: NPCs move along waypoint paths

#### Subtask 1c.4: Perception

- [x] T033 [P] Create `src/native/engine/ai/perception.h` with:
  ```c
  int perception_raycast(float origin[3], float direction[3], float max_dist, float hit_point[3]);
  int perception_nearby(float pos[3], float radius, float *nearby_positions, int max_count);
  ```

- [x] T034 [P] Implement `src/native/engine/ai/perception.c`:
  - Raycast: Simple line-of-sight check against axis-aligned world bounds
  - Nearby: Query all entities within radius (simple distance check)
  - **Success Criteria**: Raycasts don't crash; proximity queries work

#### Subtask 1c.5: Wildlife Controller Specialization

- [x] T035 Create `src/native/engine/ai/wildlife_controller.h` with:
  ```c
  typedef struct {
    FSMData fsm;
    Pathfinding pathfinding;
    float move_speed;  // units/sec
    float investigation_radius;
  } WildlifeControllerData;

  ControllerInstance* wildlife_controller_create(ControllerInstanceId id, float x, float y, float z);
  ```

- [x] T036 Implement `src/native/engine/ai/wildlife_controller.c`:
  - Patrol speed: 2 units/sec
  - Investigation speed: 4 units/sec
  - Investigation radius: 15 units
  - **on_signal("player_nearby")**: Record signal position, transition to Investigate
  - **on_signal("death")**: Transition to Dead (no further updates)
  - `update(dt)`: Poll FSM, move toward waypoint, check time-based transitions
  - **Success Criteria**: All 4 states working, signals trigger correctly

#### Subtask 1c.6: Controller System & Testing

- [x] T037 Create `src/native/engine/ai/controller_system.h` with:
  ```c
  typedef struct {
    ControllerInstance *instances;
    int count;
    int capacity;
  } ControllerSystem;

  ControllerSystem* controller_system_create(void);
  ControllerInstanceId controller_system_spawn(ControllerSystem *sys, const char *type, float x, float y, float z);
  void controller_system_update(ControllerSystem *sys, float dt);
  void controller_system_signal_all(ControllerSystem *sys, const char *signal, void *data);
  void controller_system_destroy(ControllerSystem *sys);
  ```

- [x] T038 Implement `src/native/engine/ai/controller_system.c`:
  - Manage array of active controller instances
  - Bulk update all controllers per frame
  - Broadcast signals to all (or filtered by type)

- [x] T039 Add controller test in `tests/native/phase1-engine-tests.c`:
  ```c
  void test_wildlife_controller() {
    ControllerSystem *sys = controller_system_create();

    // Spawn 3 wildlife controllers
    for (int i = 0; i < 3; i++) {
      controller_system_spawn(sys, "wildlife", i * 5.0f, 0, 0);
    }

    // Simulate 10 seconds (600 frames at 60 Hz)
    for (int frame = 0; frame < 600; frame++) {
      controller_system_update(sys, 1.0f / 60.0f);

      if (frame == 300) {
        // Send signal at frame 300
        controller_system_signal_all(sys, "player_nearby", NULL);
      }
    }

    // At least one controller should have changed state due to signal
    controller_system_destroy(sys);
  }
  ```

- [x] T040 Run test:
  ```bash
  make test_phase1_engine
  ```
  - **Success**: Controller test passes

---

## Phase 1d: WASM Game Loop (Week 2-3)

**Goal**: Main orchestrator that coordinates rendering, physics, and AI frame-by-frame
**Independent Test**: WASM module loads; game loop runs 60 frames; frame times logged
**Estimated Duration**: 2-3 days
**Blocker**: Requires rendering (1a), physics (1b), and controller (1c) to be ready

### Tasks

#### Subtask 1d.1: WASM Module Export Setup

- [x] T041 Create `src/typescript/api/wasm/game-loop.wasm.ts`:
  ```typescript
  // C function imports (via WASM imports)
  interface CEngine {
    memory: WebAssembly.Memory;
    render_init(width: number, height: number): void;
    render_frame(): void;
    render_get_frame_buffer(): number;  // Returns offset in linear memory
    physics_world_create(): number;  // Returns world handle
    physics_world_step(world: number, dt: number): void;
    controller_system_create(): number;
    controller_system_spawn(sys: number, type_ptr: number, x: number, y: number, z: number): number;
    controller_system_update(sys: number, dt: number): void;
  }

  export class GameLoop {
    private engine: CEngine;
    private frameCount: number = 0;
    private lastFrameTime: number = 0;
    private frameTimes: number[] = [];

    constructor(engine: CEngine) {
      this.engine = engine;
    }

    async tick(dt: number): Promise<void> {
      // Documented call sequence from plan.md
      this.engine.physics_world_step(dt);
      this.engine.controller_system_update(dt);
      this.engine.render_frame();

      this.frameCount++;
      if (this.frameCount % 60 === 0) {
        // Emit telemetry every second at 60 FPS
        this.emitTelemetry();
      }
    }

    private emitTelemetry(): void {
      // TODO: Send to API (phase 1e)
      console.log(`Frame ${this.frameCount}: avg ${this.getAverageFrameTime().toFixed(2)}ms`);
    }

    private getAverageFrameTime(): number {
      return this.frameTimes.reduce((a, b) => a + b, 0) / Math.max(1, this.frameTimes.length);
    }
  }
  ```
  - Reference: [WASM Orchestration spec](../../001-game-engine-architecture/spec.md#21-game-loop-game-loopwasmts)

- [x] T042 Create Emscripten build target in `src/native/engine/CMakeLists.txt`:
  - Build `banana_engine_core` as WASM module using Emscripten
  - Enable WASM exports via `EMSCRIPTEN_EXPORTED_FUNCTIONS`
  - Link with `-sWASM=1 -sALLOW_MEMORY_GROWTH=1`
  - Output: `banana_engine.wasm` to `dist/` directory
  - **Risk Mitigation**: Validate Emscripten SDK present; document version requirement

#### Subtask 1d.2: WASM Linking & Integration

- [x] T043 Create `src/typescript/api/wasm/engine-binding.ts`:
  ```typescript
  import wasmModule from './banana_engine.wasm';

  export async function initializeEngine(): Promise<CEngine> {
    const instance = await WebAssembly.instantiate(wasmModule);
    return instance.exports as CEngine;
  }
  ```

- [x] T044 Add WASM module to TypeScript API build:
  - Update `tsconfig.json` to include `.wasm` in `compilerOptions.resolveJsonModule`
  - Configure Vite to handle .wasm imports (if using Vite)
  - **Success**: WASM loads without import errors

#### Subtask 1d.3: Test Harness

- [x] T045 Create `tests/integration/phase1-wasm-harness.spec.ts`:
  ```typescript
  import { GameLoop, initializeEngine } from '@banana/api/wasm';

  describe('Phase 1 WASM Game Loop', () => {
    it('should initialize engine and run 60 frames', async () => {
      const engine = await initializeEngine();
      const gameLoop = new GameLoop(engine);

      const startTime = Date.now();
      for (let frame = 0; frame < 60; frame++) {
        await gameLoop.tick(1.0 / 60.0);
      }
      const elapsedMs = Date.now() - startTime;

      console.log(`60 frames in ${elapsedMs}ms (target: ~1000ms)`);
      expect(elapsedMs).toBeLessThan(5000);  // Generous limit for CI
    });
  });
  ```

- [x] T046 Run test:
  ```bash
  cd src/typescript/api
  npm test -- phase1-wasm-harness
  ```
  - **Success**: WASM harness runs 60 frames, frame times logged

---

## Phase 1e: Integration & Telemetry (Week 3)

**Goal**: End-to-end rendering + physics + AI; save/load cycle; telemetry to API
**Independent Test**: Full 5-minute runtime; no crashes; save/load produces identical replay
**Estimated Duration**: 3-4 days
**Blocker**: Requires 1a–1d complete

### Tasks

#### Subtask 1e.1: World State Serialization

- [x] T047 Create `src/typescript/api/wasm/game-state-bridge.ts`:
  ```typescript
  export interface WorldState {
    entities: EntitySnapshot[];
    controllers: ControllerSnapshot[];
    physics: PhysicsSnapshot;
    timestamp: number;
  }

  export interface EntitySnapshot {
    id: number;
    type: string;
    position: [number, number, number];
    rotation: [number, number, number, number];
  }

  export class GameStateBridge {
    serializeWorldState(engine: CEngine): WorldState {
      // Read entity positions from C world state
      // Return as JSON-serializable snapshot
    }

    deserializeWorldState(engine: CEngine, state: WorldState): void {
      // Write entity positions back to C world state
    }

    async syncToAPI(state: WorldState): Promise<void> {
      // POST to /api/game/save
    }
  }
  ```
  - Reference: [Game State Bridge spec](../../001-game-engine-architecture/spec.md#22-game-state-bridge-game-state-bridgets)

- [x] T048 Implement serialization logic:
  - Copy entity array from C linear memory
  - Convert floats to JSON-serializable format
  - Preserve controller FSM state
  - **Success**: State serializes and deserializes without data loss

#### Subtask 1e.2: API Endpoints (ASP.NET)

- [x] T049 Create new endpoints in `src/c-sharp/asp.net/Controllers/GameStateController.cs`:
  ```csharp
  [ApiController]
  [Route("api/game")]
  public class GameStateController : ControllerBase {
    [HttpPost("save")]
    public async Task<IActionResult> SaveGameState([FromBody] GameStateDto state) {
      // Store state in game_state_saves table
      // Return save_id
    }

    [HttpGet("load/{saveId}")]
    public async Task<IActionResult> LoadGameState(string saveId) {
      // Retrieve saved state from DB
      // Return WorldState
    }

    [HttpPost("telemetry")]
    public async Task<IActionResult> EmitTelemetry([FromBody] TelemetryEventDto telemetry) {
      // Store in game_state_telemetry table
    }
  }
  ```
  - Reference: [ASP.NET API spec](../../001-game-engine-architecture/spec.md#31-aspnet-api-unchanged)

- [x] T050 Create schema migration for `game_state_saves` and `game_state_telemetry` tables:
  - **game_state_saves**: id (PK), save_name, world_state (JSON), created_at, updated_at
  - **game_state_telemetry**: id (PK), frame_count, entity_count, avg_frame_ms, created_at
  - Migrate via Prisma (update `schema.prisma`)
  - **Success**: Tables created in Postgres

- [x] T051 Add persistence layer in `src/c-sharp/asp.net/Services/GameStateService.cs`:
  ```csharp
  public class GameStateService {
    public async Task<string> SaveState(GameStateDto state) { ... }
    public async Task<GameStateDto> LoadState(string saveId) { ... }
    public async Task RecordTelemetry(TelemetryEventDto telemetry) { ... }
  }
  ```

#### Subtask 1e.3: React Dashboard

- [x] T052 Create React component `src/typescript/react/src/components/GameWorldMap.tsx`:
  ```typescript
  export const GameWorldMap: React.FC<{ telemetry: TelemetryFeed }> = ({ telemetry }) => {
    return (
      <div className="world-map">
        {/* Render entity positions as dots on 2D top-down map */}
        {telemetry.entities.map(entity => (
          <div
            key={entity.id}
            className={`entity entity-${entity.type}`}
            style={{
              left: `${(entity.position[0] / WORLD_WIDTH) * 100}%`,
              top: `${(entity.position[2] / WORLD_DEPTH) * 100}%`
            }}
          >
            {entity.type}
          </div>
        ))}
      </div>
    );
  };
  ```

- [x] T053 Create telemetry display component `src/typescript/react/src/components/GameTelemetry.tsx`:
  - Show frame count, average frame time, entity count
  - Time-series graph of frame times (last 300 frames)
  - Controller states table

- [x] T054 Add game state page `src/typescript/react/src/pages/GameStatePage.tsx`:
  - Integrate GameWorldMap + GameTelemetry
  - Add "Save Game" / "Load Game" buttons (call API endpoints)
  - Real-time telemetry feed (poll API every 100ms or WebSocket)

#### Subtask 1e.4: End-to-End Testing

- [x] T055 Create comprehensive integration test `tests/integration/phase1-e2e.spec.ts`:
  ```typescript
  describe('Phase 1 End-to-End', () => {
    it('should run full game loop for 5 minutes without crash', async () => {
      // Initialize engine, game loop, controller system
      // Run 18000 frames (5 min at 60 FPS)
      // Verify frame times < 16ms (60 FPS)
      // Verify entity positions change
      // Verify controller states cycle through FSM
    });

    it('should deterministically replay after save/load', async () => {
      // Run game loop 100 frames
      // Serialize state at frame 100
      // Reset and deserialize
      // Run another 100 frames
      // Verify positions match between first run frame 100-200 and second run frame 1-100
    });

    it('should send telemetry to API', async () => {
      // Mock API endpoints
      // Run game loop with telemetry emission enabled
      // Verify telemetry POST requests received
    });
  });
  ```

- [x] T056 Run end-to-end test:
  ```bash
  npm test -- phase1-e2e
  ```
  - **Success**: All assertions pass

#### Subtask 1e.5: Performance Validation

- [x] T057 Create performance benchmark in `tests/native/phase1-perf.c`:
  - Run 1000 physics steps + controller updates + render calls
  - Measure time per frame
  - Target: < 16.7ms per frame (60 FPS)
  - **Success**: Frame time budget met

- [x] T058 Validate memory leaks:
  ```bash
  valgrind --leak-check=full --show-leak-kinds=all ./build/test_phase1_engine
  ```
  - **Success**: 0 leaked bytes

---

## Phase 1f: Polish & Validation (Final)

**Goal**: Documentation, final testing, handoff to Phase 2
**Estimated Duration**: 1-2 days

### Tasks

- [x] T059 Write `src/native/engine/README.md`:
  - Architecture overview
  - Building instructions (CMake)
  - Module descriptions (rendering, physics, AI)
  - WASM export contract

- [x] T060 Document C↔WASM interface in [ABI reference](../../../docs/c-wasm-abi-reference.md):
  - Function signatures
  - Memory layout (frame buffer offset, entity state layout)
  - Calling conventions
  - Example WASM → C call

- [x] T061 Update root [BUILD.md](../../../BUILD.md):
  - Phase 1 compilation steps
  - Dependency installation (GLFW, OpenGL, Emscripten)
  - Run tests via `make test`

- [x] T062 Create [Phase 1 Acceptance Checklist](./PHASE1-ACCEPTANCE.md):
  ```markdown
  - [x] C engine builds: `cmake --build build`
  - [x] Cube renders: frame buffer contains non-zero RGBA
  - [x] Physics deterministic: save/load produces identical replay
  - [x] 5 NPCs patrol: visible state transitions on signal
  - [x] WASM loop runs 60 frames: < 5 sec elapsed
  - [x] Save/load cycle: world state persisted + restored
  - [x] Telemetry flows to API: events stored in Postgres
  - [x] React dashboard displays: entity positions + frame times
  - [x] No crashes: 5-minute stress test passes
  - [x] 0 memory leaks: Valgrind clean
  ```

- [x] T063 Final integration test run:
  ```bash
  cd /c/Github/Banana
  cmake --build build
  make test
  npm run test:integration -- phase1-e2e
  ```
  - **Success**: All tests pass, documentation complete

---

## Dependency Graph

```
T001 (Setup: directories)
  ↓
T002 (CMakeLists.txt)
  ├→ T003 (GLFW/GL dependencies) [P] → T005-T017 (Rendering 1a)
  ├→ T004 (Test harness) [P] → T018-T026 (Physics 1b)
  └→ (Foundation complete)
       ↓
       Phase 1a (Rendering): T005-T017
         ├→ T005-T006 (Window)
         ├→ T007-T008 (Shader) [P]
         ├→ T009-T010 (Mesh) [P]
         └→ T011-T017 (Material/Camera/Renderer)

       Phase 1b (Physics): T018-T026 [can start after T002]
         ├→ T018-T019 (Body) [P]
         ├→ T020-T021 (Collider) [P]
         ├→ T022-T024 (Dynamics/World)
         └→ T025-T026 (Tests)

       Phase 1c (AI): T027-T040 [after 1a+1b complete]
         ├→ T027-T028 (Controller interface)
         ├→ T029-T030 (FSM) [P]
         ├→ T031-T032 (Navigation) [P]
         ├→ T033-T034 (Perception) [P]
         ├→ T035-T036 (Wildlife spec)
         └→ T037-T040 (System/Tests)

       Phase 1d (WASM): T041-T046 [after 1a+1b+1c complete]
         └→ T045-T046 (Tests)

       Phase 1e (Integration): T047-T058 [after 1a+1b+1c+1d complete]
         ├→ T047-T048 (Serialization)
         ├→ T049-T051 (API)
         ├→ T052-T054 (React)
         ├→ T055-T056 (E2E tests)
         └→ T057-T058 (Perf/leaks)

       Phase 1f (Polish): T059-T063 [after all previous phases]
```

---

## Parallel Execution Examples

### Scenario 1: Two Developers

**Developer A** (Tasks 1-28):
- T001-T004: Setup
- T005-T017: Rendering (1a)
- T041-T046: WASM Loop (1d) [start after 1a done]
- T059-T061: Documentation

**Developer B** (Tasks 18-40):
- T018-T026: Physics (1b)
- T027-T040: AI Controller (1c)
- T047-T058: Integration (1e)

**Timeline**: ~3 weeks with task overlap

### Scenario 2: Three Developers

**Developer A**: Rendering (T005-T017)
**Developer B**: Physics (T018-T026) + Navigation (T031-T032)
**Developer C**: AI Controller (T027-T030, T033-T036) + WASM (T041-T046)

**Timeline**: ~2 weeks with full parallelization

---

## Risk Mitigations Summary

| Risk | Task | Mitigation |
|------|------|-----------|
| WASM/C interop complexity | T041-T046 | Start with simple C→WASM calls; validate memory layout in T045 |
| Determinism loss (floating-point) | T026, T057 | Snapshot-based replay testing; run 100+ frame sequences |
| OpenGL context issues (headless CI) | T006, T015 | Use offscreen FBO; test on Linux (xvfb) + macOS + Windows |
| Physics inefficiency (O(n²) collisions) | T021, T057 | Cap body count to 50; add spatial hash in Phase 2 if needed |
| Controller state explosion | T030, T039 | Limit to 4 states (idle, patrol, investigate, return); add complexity Phase 2 |
| Memory leaks in C engine | T058 | Valgrind validation; add cleanup tests |
| Shader compilation errors | T008 | Log compile errors with line numbers; test on 3+ GPU drivers |
| Frame buffer export (render-to-texture) | T015, T043 | FBO offscreen rendering; validate RGBA data in tests |

---

## Success Criteria (Phase 1 Complete)

- [x] C engine builds with `cmake --build build` (Linux/Windows/macOS)
- [x] OpenGL renders rotating cube frame
- [x] Physics deterministic over 1000+ frame replay
- [x] 5 NPC instances patrol independently
- [x] WASM orchestrates physics + AI + rendering
- [x] Frame cycle completes in < 16.7ms (60 FPS target)
- [x] Save/load cycle preserves world state
- [x] Telemetry flows to ASP.NET API → Postgres
- [x] React dashboard displays live entity positions + frame times
- [x] All tests pass (native + integration + E2E)
- [x] 0 memory leaks (Valgrind clean)
- [x] Documentation complete (README, ABI reference, acceptance checklist)

---

## Handoff to Phase 2

**Deliverables**:
1. Fully functional C engine core (render, physics, AI framework)
2. WASM orchestration layer proven
3. API save/load + telemetry schema
4. React dashboard with world state visualization
5. Test suite (unit + integration + E2E)
6. Documentation (ABI contract, architecture, build instructions)

**Phase 2 will build on**:
- Multi-controller AI patterns (predator/prey dynamics)
- Advanced rendering (skeletal animation, lighting)
- Procedural world generation
- Performance optimization (spatial indexing, batch rendering)

