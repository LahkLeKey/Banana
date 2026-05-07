# Phase 1 Acceptance Checklist

Feature: **864 — Game Engine Pivot Baseline**
Branch: `feat/864-game-engine-pivot-baseline`
Status: Draft — **DO NOT MERGE** until WASM runs in-browser at target performance.

---

## C Engine

| # | Criterion | Status |
|---|-----------|--------|
| 1 | `src/native/engine/` directory exists with CMakeLists.txt | ✅ |
| 2 | `BANANA_ENABLE_ENGINE=OFF` default — existing CI unaffected | ✅ |
| 3 | Triple-path compilation: `__EMSCRIPTEN__` / `BANANA_ENGINE_HAS_GLFW` / headless | ✅ |
| 4 | Render pipeline: window, shader, mesh, material, camera, renderer | ✅ |
| 5 | Physics pipeline: body, collider, dynamics, world | ✅ |
| 6 | AI pipeline: controller registry, state machine, navigation (A*), perception | ✅ |
| 7 | Wildlife NPC: 4-state FSM (idle→patrol→investigate→return) | ✅ |
| 8 | ControllerSystem pool (256 slots), signal broadcast | ✅ |
| 9 | `wasm_main.c` spawns 5 NPCs and drives `emscripten_set_main_loop` | ✅ |
| 10 | All WASM ABI symbols in `engine.c` (not scattered across modules) | ✅ |

## Test Suite

| # | Criterion | Status |
|---|-----------|--------|
| 11 | `banana_test_engine_phase1` builds from `BANANA_ENABLE_ENGINE=ON` | ✅ |
| 12 | 26/26 tests passing, 0 failures | ✅ |
| 13 | Tests cover: renderer stub, mesh, physics determinism, FSM, A* nav, perception, wildlife NPC, controller system | ✅ |

## WASM Build

| # | Criterion | Status |
|---|-----------|--------|
| 14 | `scripts/build-engine-wasm.sh` produces `engine.js` + `engine.wasm` | ⬜ requires emcc |
| 15 | `engine.js` loads in Chrome/Firefox without errors | ⬜ manual |
| 16 | `_engine_init()` completes without GL errors | ⬜ manual |
| 17 | `_engine_tick()` renders ≥ 55 fps on a mid-range laptop | ⬜ manual |

## React Frontend

| # | Criterion | Status |
|---|-----------|--------|
| 18 | `GameEnginePage` reachable at `/game-engine` | ✅ |
| 19 | "Unavailable" state shown gracefully when `engine.js` absent | ✅ |
| 20 | Canvas renders when WASM loads | ⬜ requires WASM |
| 21 | `GameWorldMap` shows live entity positions as coloured dots | ✅ component ready |
| 22 | `GameTelemetry` shows frame-time graph and stats | ✅ component ready |
| 23 | Entity count in info card reflects live WASM value | ✅ wired up |

## ASP.NET API

| # | Criterion | Status |
|---|-----------|--------|
| 24 | `POST /api/game/save` returns `{ save_id }` | ✅ |
| 25 | `GET /api/game/load/{saveId}` returns saved snapshot or 404 | ✅ |
| 26 | `GET /api/game/saves` lists all save IDs | ✅ |
| 27 | `POST /api/game/telemetry` accepts frame stats (204) | ✅ |
| 28 | `GET /api/game/telemetry` returns last N events | ✅ |
| 29 | `GameStateService` wired as singleton in Program.cs | ✅ |

## TypeScript WASM Bindings

| # | Criterion | Status |
|---|-----------|--------|
| 30 | `game-loop.wasm.ts` exports `BananaEngineModule` interface | ✅ |
| 31 | `engine-binding.ts` exports `initializeEngine()` helper | ✅ |
| 32 | `isBananaEngineModule` guard validates ABI surface at runtime | ✅ |

## Documentation

| # | Criterion | Status |
|---|-----------|--------|
| 33 | `src/native/engine/README.md` — module map, build steps, adding controllers | ✅ |
| 34 | `docs/c-wasm-abi-reference.md` — full function signatures + memory notes | ✅ |
| 35 | `PHASE1-ACCEPTANCE.md` (this file) — checklist | ✅ |

---

## Merge gate

Items 14–17 and 20 require Emscripten to be installed and a manual browser smoke test.
The PR stays **DRAFT** until all five pass.

Run the smoke test:

```bash
# 1. Install Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git /tmp/emsdk
cd /tmp/emsdk && ./emsdk install latest && ./emsdk activate latest
source /tmp/emsdk/emsdk_env.sh

# 2. Build WASM
bash scripts/build-engine-wasm.sh

# 3. Start React dev server
cd src/typescript/react && bun run dev

# 4. Open http://localhost:5173/game-engine in Chrome or Firefox
#    Verify: canvas renders, world map shows dots, FPS ≥ 55
```
