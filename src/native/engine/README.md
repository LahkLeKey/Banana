# Banana Engine — Native C → WASM

This directory contains the Phase 1 game engine: a pure-C runtime compiled to
WebAssembly via Emscripten and consumed by the React frontend.

## Module map

```
engine/
├── engine.h / engine.c        ← singleton lifecycle + WASM ABI entry points
├── wasm_main.c                ← Emscripten entry point, spawns initial NPCs
├── CMakeLists.txt             ← dual-target: native test binary + WASM bundle
│
├── render/
│   ├── window.h / window.c    ← context creation (triple-path: WASM / GLFW / headless)
│   ├── shader.h / shader.c    ← GLSL compile + link
│   ├── mesh.h / mesh.c        ← vertex buffer builders (cube, sphere)
│   ├── material.h             ← Material struct (color[3])
│   ├── camera.h / camera.c    ← perspective + view/projection matrices
│   └── renderer.h / renderer.c← FBO render loop, MVP transforms
│
├── physics/
│   ├── body.h / body.c        ← RigidBody struct, velocity integration
│   ├── collider.h / collider.c← AABB collision detection
│   ├── dynamics.h / dynamics.c← force accumulation, constraint solving
│   └── world.h / world.c      ← physics world: step, add/remove bodies
│
└── ai/
    ├── controller.h / controller.c          ← controller registry (name → factory)
    ├── controller_system.h / controller_system.c ← 256-slot pool, signal broadcast
    ├── state_machine.h / state_machine.c    ← generic FSM (16 states, 32 transitions)
    ├── navigation.h / navigation.c          ← A* on a 32×32 NavGrid
    ├── perception.h / perception.c          ← raycast + proximity queries
    └── wildlife_controller.h / wildlife_controller.c ← 4-state NPC FSM
```

## Build

### Prerequisites

| Target | Requirement |
|--------|-------------|
| Native tests | CMake ≥ 3.21, C compiler (GCC/Clang/MSVC) |
| WASM bundle  | [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) (`emcc` in `PATH`) |

### Native test binary

```bash
cmake -B build -DBANANA_ENABLE_ENGINE=ON
cmake --build build --target banana_test_engine_phase1
./build/tests/native/engine/banana_test_engine_phase1
```

### WASM bundle (requires emcc)

```bash
bash scripts/build-engine-wasm.sh
# Outputs:
#   out/wasm/engine.js   (Emscripten JS glue)
#   out/wasm/engine.wasm (binary module)
#   src/typescript/react/public/wasm/engine.js   (copied for dev server)
#   src/typescript/react/public/wasm/engine.wasm (copied for dev server)
```

## AI controller architecture

Each open-world mechanic is an independent **controller** registered under a
type name:

```c
void wildlife_controller_register(void);   // registers "wildlife"
```

Controllers are instantiated by the engine when an entity is spawned and
destroyed when the entity is removed.  The `ControllerSystem` manages the
256-slot pool and supports broadcast signals:

```c
controller_system_signal_all(sys, "player_nearby", &player_pos);
controller_system_signal_type(sys, "wildlife", "death", NULL);
```

### Adding a new controller

1. Create `ai/<name>_controller.h` + `.c` with a factory `<name>_factory(x, y, z)`.
2. Call `controller_register("<name>", <name>_factory)` inside `<name>_controller_register()`.
3. Call `<name>_controller_register()` from `engine_init()` in `engine.c`.
4. Add the `.c` file to `ENGINE_AI_SOURCES` in `CMakeLists.txt`.

## Runtime path

```
React (GameEnginePage.tsx)
  └─ loads /wasm/engine.js via <script>
       └─ BananaEngine module (MODULARIZE=1)
            ├─ _engine_init()      ← called once
            └─ _engine_tick(dt)    ← called each rAF frame
                 ├─ physics_step()
                 ├─ engine_world_tick()
                 ├─ controller_system_update()
                 └─ render all entities
```

The React page reads entity positions each frame through:

```ts
const count = mod._engine_get_entity_count();
const x     = mod._engine_get_entity_x(i);
const z     = mod._engine_get_entity_z(i);
const state = mod._engine_get_entity_state(i);
```

These are passed to `<GameWorldMap>` for the live minimap and `<GameTelemetry>`
for the frame-time chart.

## Testing

Tests live in `tests/native/engine/test_engine_phase1.c` and are compiled into
`banana_test_engine_phase1`.  All tests run in headless mode (no GL context)
via stub implementations.

Current status: **26/26 passing**.
