# Tasks: 001 Game Engine Architecture

**Feature**: 001-game-engine-architecture
**Branch**: `feat/001-game-engine-architecture`
**Spec**: `.specify/specs/001-game-engine-architecture/spec.md`
**Plan**: `.specify/specs/001-game-engine-architecture/plan.md`

---

## Phase 1: Rendering Foundation

- [x] T001 Implement OpenGL window management and context setup (`src/native/engine/render/window.{c,h}`)
- [x] T002 Implement camera projection and view transforms (`src/native/engine/render/camera.{c,h}`)
- [x] T003 Implement mesh + material primitives (`src/native/engine/render/mesh.{c,h}`, `material.{c,h}`)
- [x] T004 Implement renderer and shader pipeline with stub mode for headless CI (`src/native/engine/render/renderer.{c,h}`, `shader.{c,h}`)
- [x] T005 Implement frame buffer export contract for WASM consumer

---

## Phase 2: Physics Integration

- [x] T006 Implement rigid body representation and dynamics integration (`src/native/engine/physics/body.{c,h}`, `dynamics.{c,h}`)
- [x] T007 Implement AABB/sphere collider and collision detection (`src/native/engine/physics/collider.{c,h}`)
- [x] T008 Implement physics world step with fixed-timestep integration (`src/native/engine/physics/world.{c,h}`)

---

## Phase 3: AI Controllers

- [x] T009 Implement finite state machine core (`src/native/engine/ai/state_machine.{c,h}`)
- [x] T010 Implement grid-based navigation and pathfinding (`src/native/engine/ai/navigation.{c,h}`)
- [x] T011 Implement spatial perception (proximity + signal detection) (`src/native/engine/ai/perception.{c,h}`)
- [x] T012 Implement wildlife NPC controller (patrol/idle/flee states) (`src/native/engine/ai/wildlife_controller.{c,h}`)
- [x] T013 Implement generic controller interface and instance registry (`src/native/engine/ai/controller.{c,h}`, `controller_system.{c,h}`)

---

## Phase 4: World + Entity + WASM Orchestration

- [x] T014 Implement entity type registry and world entity management (`src/native/engine/world/entity.{c,h}`, `world.{c,h}`)
- [x] T015 Implement inter-controller signals queue (`src/native/engine/world/signals.{c,h}`)
- [x] T016 Implement top-level engine tick and render dispatcher (`src/native/engine/engine.{c,h}`)
- [x] T017 Implement WASM export layer with all required `WASM_EXPORT` functions (`src/native/engine/wasm_main.c`)

---

## Phase 5: Tests + CMake

- [x] T018 Add CMake target for engine library and WASM module (`src/native/engine/CMakeLists.txt`)
- [x] T019 Add Phase 1 C test harness covering physics, AI, world, and renderer stub (`tests/native/engine/test_engine_phase1.c`)

---

## Phase 6: Spec Validation

- [x] T020 All spec 001 success criteria satisfied: engine compiles, WASM links, physics deterministic, NPC controller operational, full tick cycle completes, telemetry flows to API
