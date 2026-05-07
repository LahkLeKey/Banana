# Implementation Plan: 001 Game Engine Architecture

**Branch**: `feat/001-game-engine-architecture` | **Date**: 2026-05-07 | **Spec**: `.specify/specs/001-game-engine-architecture/spec.md`
**Input**: Feature specification from `.specify/specs/001-game-engine-architecture/spec.md`

## Summary

Implement a C-native game engine architecture with rendering, physics, AI controller, WASM orchestration, and entity/world systems. All Phase 1 deliverables are implemented and validated under `src/native/engine/` and `tests/native/engine/`.

## Technical Context

**Language/Version**: C (C11), CMake 3.20+, wasm32-unknown-unknown
**Primary Dependencies**: CMake, Emscripten (WASM export), standard C math
**Storage**: In-memory entity/world state; serialization via engine save/load hooks
**Testing**: C test harness (no GPU required on CI — renderer runs in stub mode)
**Target Platform**: Native (Linux/macOS/Windows), WASM via Emscripten
**Performance Goals**: Deterministic physics tick; frame buffer available at WASM boundary
**Constraints**: No high-level C++ abstractions; OpenGL/native graphics only; no Three.js
**Scale/Scope**: Phase 1 — rendering foundation, physics, NPC controller, WASM loop, integration

## Constitution Check

- Preserve established banana-classification API contracts (engine is a separate layer).
- Scope changes to `src/native/engine/`, `tests/native/engine/`, and `CMakeLists.txt`.
- Keep CI behavior deterministic with stub rendering on headless runners.

## Project Structure

- `src/native/engine/engine.{c,h}` — engine tick + render dispatcher
- `src/native/engine/render/` — renderer, mesh, material, camera, shader, window
- `src/native/engine/physics/` — body, collider, dynamics, world
- `src/native/engine/ai/` — controller, controller_system, state_machine, navigation, perception, wildlife_controller
- `src/native/engine/world/` — entity, world, signals
- `src/native/engine/wasm_main.c` — WASM export layer
- `tests/native/engine/test_engine_phase1.c` — Phase 1 test harness

## Phases

### Phase 1: Rendering Foundation (COMPLETE)
- OpenGL window + camera + renderer stub
- Mesh + material primitives
- Frame buffer export to WASM

### Phase 2: Physics Integration (COMPLETE)
- Rigid body dynamics, collider, world step
- Deterministic integration with fixed timestep

### Phase 3: AI Controllers (COMPLETE)
- State machine, navigation grid, perception, wildlife controller
- Controller instances managed by controller_system

### Phase 4: WASM Orchestration + Integration (COMPLETE)
- `wasm_main.c` exports: `engine_tick`, `engine_render_frame`, `physics_world_step`, `controller_create`, `controller_update`, `world_spawn_entity`, `world_tick`
- Entity/world/signal wiring
- Phase 1 test harness in `tests/native/engine/`
