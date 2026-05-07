# C → WASM ABI Reference

This document describes every function exported from `engine.c` through
Emscripten to the JavaScript/TypeScript host layer.

All symbols are prefixed with `_` in the Emscripten module object.

---

## Lifecycle

### `_engine_init() → void`

Initialise all engine subsystems: renderer, physics world, AI controller table,
and spawn the initial NPC set.  Must be called **once** after the WebGL 2 canvas
context exists.

### `_engine_tick(dt: f32) → void`

Advance the simulation by `dt` seconds.  Call from `requestAnimationFrame` or a
fixed-step loop.  Internally:

1. `physics_step(dt)`
2. `engine_world_tick(dt)`
3. `controller_system_update(dt)` — AI FSM transitions
4. Copy controller positions back into entity table
5. Render all entities

---

## Entity query

### `_engine_get_entity_count() → i32`

Return the number of active entities.

### `_engine_get_entity_x(idx: i32) → f32`

World-space X coordinate of entity at slot `idx`.

### `_engine_get_entity_z(idx: i32) → f32`

World-space Z coordinate of entity at slot `idx`.

### `_engine_get_entity_state(idx: i32) → i32`

Controller state integer for entity at slot `idx`.

| Value | Meaning |
|-------|---------|
| 0 | inactive / not yet assigned a controller |
| 1 | patrol (moving along A* path) |
| 2 | investigate (heading toward detected stimulus) |
| 3 | return-to-base |

---

## Controller ABI

### `_engine_controller_create(typeName: string) → u32`

Create a new controller instance of the named type.  Returns an opaque handle
(`uint32_t`), or `0` on failure.

Registered types at Phase 1:

| Name | Behaviour |
|------|-----------|
| `"wildlife"` | 4-state FSM NPC with A* patrol and proximity detection |

### `_engine_controller_attach(entityId: u32, typeName: string) → u32`

Attach an existing registered controller type to entity `entityId`.  Returns
non-zero on success.

### `_engine_controller_update(handle: u32, dt: f32) → void`

Advance a single controller by `dt` seconds.  Normally called internally by
`engine_tick`; exposed for fine-grained testing.

### `_engine_controller_signal(handle: u32, signal: string, data: f32) → void`

Deliver a named signal to a controller.  `data` is a scalar float; for 3-vector
signals the convention is to call three times with `"signal_x"`, `"signal_y"`,
`"signal_z"`.

Built-in signals:

| Signal | Effect |
|--------|--------|
| `"player_nearby"` | Switch to investigate state |
| `"death"` | Force back to idle/inactive |

---

## Physics ABI

### `_physics_step(dt: f32) → void`

Integrate all rigid bodies by `dt` seconds.

### `_physics_add_body(x: f32, y: f32, z: f32, mass: f32) → i32`

Register a new rigid body at `(x, y, z)` with the given `mass`.  Returns an
integer body ID (`-1` on failure).

### `_physics_update_body(id: i32, x: f32, y: f32, z: f32) → void`

Overwrite the position of body `id`.  Used to sync AI-driven entities back into
the physics world.

---

## World / spawn ABI

### `_engine_world_spawn(x: f32, y: f32, z: f32, type: i32) → u32`

Spawn an entity at world position `(x, y, z)`.  `type` maps to `EntityType`:

| Value | Entity type |
|-------|-------------|
| 0 | static object |
| 1 | dynamic object |
| 2 | NPC (AI-controlled) |
| 3 | player |

Returns the new entity ID, or `0` on failure (entity table full).

### `_engine_world_tick(dt: f32) → void`

Advance the world simulation (region loading, event queue) by `dt`.

---

## Memory layout notes

- Emscripten is configured with `ALLOW_MEMORY_GROWTH=1`; the heap starts at 16 MiB and grows on demand.
- String arguments (`typeName`, `signal`) are passed as null-terminated C strings via Emscripten's
  automatic UTF-8 JS→C conversion when using `ccall` / `cwrap`.  Direct `_` prefix calls from the
  WASM module object also work when the string is a JS `string` literal thanks to the Emscripten JS
  glue.
- Scalar numeric arguments and return values map directly: `float` ↔ `number`, `int`/`uint32_t` ↔
  `number`.

---

## TypeScript types

See [`src/typescript/api/wasm/game-loop.wasm.ts`](../../src/typescript/api/wasm/game-loop.wasm.ts) for
the full `BananaEngineModule` interface and the `isBananaEngineModule` guard.

The [`src/typescript/api/wasm/engine-binding.ts`](../../src/typescript/api/wasm/engine-binding.ts)
`initializeEngine()` helper handles script injection, module readiness polling, and ABI validation.
