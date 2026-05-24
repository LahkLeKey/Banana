# Banana Engine — Native C Runtime

This directory contains the domain-contract game engine implemented in pure C.
Current delivery focus is the Steam Windows desktop client (DirectX12 runtime
integration path), with web usage limited to marketing/account surfaces.

## Module map

```
engine/
├── engine.h / engine.c        ← singleton lifecycle + runtime ABI entry points
├── wasm_main.c                ← legacy Emscripten entry point (not an active deliverable)
├── CMakeLists.txt             ← native targets; legacy Emscripten target retained for history
│
├── render/
│   ├── backend.h / backend.c  ← backend selection (`glfw`, `dx12`, `headless`, `wasm`)
│   ├── backend_dx12.h / .c    ← DX12 probe + runtime (swapchain, depth, fences, telemetry)
│   ├── window.h / window.c    ← window/event abstraction (Win32 + GLFW + headless)
│   ├── shader.h / shader.c    ← GLSL compile + link (legacy path while DX12 draw migration completes)
│   ├── mesh.h / mesh.c        ← vertex buffer builders (banana-derived profiles, terrain data)
│   ├── material.h             ← Material struct (color[3])
│   ├── camera.h / camera.c    ← perspective + view/projection matrices
│   └── renderer.h / renderer.c← render lifecycle + backend runtime bridge
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

### Native test binary

```bash
cmake -B build -DBANANA_ENABLE_ENGINE=ON
cmake --build build --target banana_test_engine_domain_contracts
./build/tests/native/engine/banana_test_engine_domain_contracts
```

### Delivery note

WASM/WebAssembly assets are not part of the active Steam DX12 release path.
Treat any remaining Emscripten sources/targets as legacy compatibility material.

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

## Runtime path (desktop-focused)

```
Desktop shell (Steam Windows client)
  └─ initializes native runtime
       ├─ engine_init()
       └─ engine_tick(dt)
            ├─ physics_step()
            ├─ engine_world_tick()
            ├─ controller_system_update()
            └─ render + present
```

## Domain boundaries (DDD-oriented)

Runtime responsibilities are split by domain service modules rather than one
monolithic `engine.c` control block:

- `runtime/engine/engine_lifecycle.*`: bootstrap/teardown orchestration.
- `runtime/engine/engine_tick.*`: tick pipeline execution and phase ordering.
- `runtime/input/input_contract.*`: click/move intent contract and sanitization.
- `runtime/input/move_target_domain.*`: strongly-typed click-target state and steering decisions.
- `runtime/render/render_submit.*` + `RendererDrawCommand`: typed scene command
  submission seam so domain render intent is backend-agnostic.
- `runtime/player/player_*.*`, `runtime/terrain/terrain_*.*`, `runtime/world/world_*.*`: bounded
  contexts for player, terrain, and world behaviors.

This keeps high-level orchestration in `engine.c` while moving domain rules into
testable, single-purpose modules.

## DX12 runtime contract (current)

- Windows default backend is `dx12` (see `BANANA_RENDER_BACKEND` in CMake).
- Runtime initializes: factory, adapter, device, queue, swapchain, RTV heap,
  DSV heap, depth target, command allocator/list, fence/event.
- Frame flow:
  1. `banana_dx12_runtime_begin_frame`: reset allocator/list, transition backbuffer,
     bind RTV/DSV, clear color + depth.
  2. Scene submit (currently minimal while GL draw migration is in progress).
  3. `banana_dx12_runtime_end_frame`: transition to present, execute command list,
     `Present(interval)`, fence wait, advance frame index.
- Present mode is configurable with environment variables:
  - `BANANA_DX12_PRESENT_INTERVAL` (`0` immediate, `1` vsync)
  - `BANANA_DX12_VSYNC` fallback (`0`/`1`)
- Device-loss handling attempts swapchain recreation on
  `DXGI_ERROR_DEVICE_REMOVED`/`DXGI_ERROR_DEVICE_RESET` during resize/present.
- Telemetry surface:
  - `banana_dx12_runtime_telemetry()`
  - `banana_dx12_runtime_frames_presented()`

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

Native tests live under `tests/native/*.c` and are registered as individual CTest
targets. Key new coverage includes:

- `banana_engine_dx12_runtime_smoke_test`
- `banana_runtime_move_target_domain_test`
- `banana_engine_click_to_move_target_test`
- `banana_engine_input_contract_transition_test`
