# Specification: Unified Gameplay Authority + Native Domain Bake-In + WASM Orchestration (Spec 051)

**Version**: 2.0 | **Date**: 2026-05-15 | **Status**: Active Unified Baseline
**Supersedes Local Notes**: ad-hoc implementation transcript notes for AI loop wiring
**Scope Authority**: multiplayer authority baseline + native engine loop + native domain integration contract + web viewport WASM orchestration
**Absorbs**: Spec 050 multiplayer server-authoritative baseline (articles, evidence posture, anti-cheat authority)

---

## Purpose

Deliver a complete playable AI loop in the web viewport by enforcing three contracts together:

1. **Engine AI Loop Contract**: ML-driven controllers must update every frame and visibly drive actors.
2. **Native Domain Bake-In Contract**: `banana_native` must be built from current migrated domains (not deleted legacy `core` files).
3. **WASM Orchestration Contract**: web runtime must consume current engine artifacts through a reproducible build-and-publish path.

The previous partial state showed isolated progress in engine/ML code but broken production runtime flow due to root native build drift and non-orchestrated WASM artifact lifecycle.

This unified spec also carries forward the multiplayer constitutional baseline from Spec 050 so authoritative server-state, prediction, replication, and anti-cheat governance remain in the same active execution surface.

---

## Current Execution Reality (Captured Context)

### What is already working

- Engine-side ML controller movement path is partially wired in native engine code.
- Engine CMake target for `banana_engine_core` compiles locally.
- React web viewport is capable of loading `/wasm/engine.js` and `/wasm/engine.wasm` when present.

### What is currently broken

- Runtime compose pipeline fails in `native-build` because root `CMakeLists.txt` still references deleted legacy files such as `src/native/core/banana_status.c`.
- Wrapper integration still assumes removed `banana_core_*` and `banana_dal_*` functions.
- Domain migration is incomplete/inconsistent across batch, truck, ripeness, status, and classify services.
- WASM artifacts are built by standalone script but not consistently orchestrated as part of the same reproducible runtime channel.

---

## In Scope (Mandatory)

- Preserve Spec 050 multiplayer authority contracts as governing constraints for gameplay execution.
- Bake all active native domains into root `banana_native` build contract.
- Replace broken legacy wrapper dependencies with current domain service calls and compatibility adapters.
- Ensure anti-cheat shared contract types are available in migrated domain service surfaces.
- Complete AI controller on-screen loop for non-player actors in engine tick path.
- Preserve histogram telemetry updates for AI actions and player input.
- Add/lock WASM orchestration so web viewport loads current engine artifacts without manual drift.
- Enforce WASM parallel worldgen runtime contract (pthread + worker pool + OpenMP) with CI hard gate.
- Ensure compose/runtime path no longer fails on deleted legacy source references.

---

## Out of Scope (This Spec)

- Advanced training pipeline changes (new model architectures, tuning infrastructure).
- Multiplayer authority expansion beyond the absorbed baseline scope.
- Full anti-cheat hardening roadmap already tracked in Spec 050.

---

## Imported Baseline from Spec 050

The following authority is merged into this spec and remains binding:

- Server-authoritative state as single source of truth.
- Deterministic tick and sequence invariants.
- Prediction and rollback constraints.
- Replication and session lifecycle contracts.
- Native anti-cheat telemetry/scoring and heartbeat layers.
- Existing Spec 050 completion evidence and non-blocking parity follow-up item.

---

## Architecture Decisions (Record)

1. `banana_engine_core` and `banana_native` are related but distinct outputs:
	- `banana_engine_core` handles runtime gameplay engine loop behavior.
	- `banana_native` remains the interop/shared library contract consumed by API/runtime channels.

2. ML movement orchestration remains deterministic and frame-driven in engine tick.

3. Native domain migration requires compatibility wrappers during transition; deleting legacy references without replacing call sites is disallowed.

4. Web viewport contract is artifact-based:
	- required files: `engine.js`, `engine.wasm`, `engine.version.json`
	- artifact publishing path must be explicit and reproducible for local runtime channels.

---

## Acceptance Criteria

- [ ] `docker compose` runtime profile can build and start without missing legacy native source errors.
- [ ] Root `banana_native` target compiles using current migrated domain sources.
- [ ] Wrapper exports remain ABI-stable and mapped to active domain services.
- [ ] AI-controlled actors are visibly moving on-screen in the web viewport.
- [ ] ML histogram events include both AI action and player input channels.
- [ ] WASM web artifacts are produced and consumed through orchestrated runtime workflow, not manual one-off steps.
- [ ] CI fails when WASM parallel worldgen budgets are not satisfied (`USE_PTHREADS`, `PTHREAD_POOL_SIZE`, parallel gate script).
- [ ] Spec, plan, tasks, and heartbeat artifacts document execution state and next actions.

---

## Risks and Constraints

- Wrapper ABI stability must be preserved while implementation is rerouted from deleted legacy core.
- Stub domain services may satisfy compile-time integration but can mask behavioral regressions.
- Emscripten toolchain availability varies by host/runtime channel; orchestration must fail loudly with actionable guidance.
- Browser threading support requires cross-origin isolation (`COOP`/`COEP`); runtime contract must fail clearly when pthread prerequisites are unavailable.

---

## Evidence Requirements

- Native build evidence for root `banana_native` target.
- Runtime compose evidence for profile startup success.
- Web viewport evidence that AI-controlled actors are moving.
- WASM artifact presence/version evidence in web public artifact location.
