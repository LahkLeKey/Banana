# Feature Specification: Unified Coherent Game World

**Feature Branch**: `031-unified-coherent-world`

**Created**: 2026-05-27

**Status**: Complete (2026-05-27)

**Input**: User description: "we have alot of slices, id like to combine them into one coherent game world"

## Purpose

Collapse the cohesion-lane treadmill (specs 015 through 030) into one canonical "coherent game world" surface owned by a single spec, a single test umbrella, and a single evidence path. Future cohesion work extends this spec instead of spawning a new numbered slice.

## Superseded Specs

This spec supersedes the following slice specs. They remain in `.specify/specs/` as historical evidence; new work MUST NOT extend them.

| Spec | Title | Disposition |
| --- | --- | --- |
| 015-coherent-game-world | Coherent Game World Integration | Folded into 031 baseline |
| 016-gameplay-api-continuity | Gameplay/API Persistence Continuity | Folded into 031 baseline |
| 017-coherent-world-unification | Slice Convergence | Folded into 031 baseline |
| 018-coherent-world-fusion | Fusion Continuation | Folded into 031 baseline |
| 019-coherent-world-synthesis | Synthesis Continuation | Folded into 031 baseline |
| 020-coherent-world-convergence | Convergence Continuation | Folded into 031 baseline |
| 021-coherent-world-cohesion | Cohesion Continuation | Folded into 031 baseline |
| 022-objective-completion-cohesion | Objective Completion | Folded into 031 baseline |
| 023-checkpoint-id-cohesion | Checkpoint Id | Folded into 031 baseline |
| 024-route-signature-cohesion | Route Signature | Folded into 031 baseline |
| 025-fusion-lane-cohesion | Fusion Lane | Folded into 031 baseline |
| 026-synthesis-pass-cohesion | Synthesis Pass | Folded into 031 baseline |
| 027-replay-phase-cohesion | Replay Phase | Folded into 031 baseline |
| 028-convergence-lane-cohesion | Convergence Lane | Folded into 031 baseline |
| 029-reconciliation-lane-cohesion | Reconciliation Lane | Folded into 031 baseline |
| 030-observation-lane-cohesion | Observation Lane | Folded into 031 baseline |

## User Scenarios *(mandatory)*

### US1 — One World, Many Variants (Priority: P1)

As a player, transitions between connected demo-scene browser variants behave as one persistent world: catalog state, gameplay placements, checkpoints, objectives, and route signatures stay coherent across round-trips.

**Independent Test**: Run the umbrella ctest filter `banana_(runtime_demo_scene_catalog|dx12|runtime_tick|runtime_camera_follow|runtime_move_target)_*` and confirm all coherent / continuity / transition / objective / lane tests pass.

### US2 — Single Owning Contract Path (Priority: P1)

As a developer, every cohesion concern (coherent profile, launch signature, failure path, transition continuity, transition drift, continuity roundtrip, mutation safety, disabled profile, objective completion, gameplay click lane, scene flow, projection policy, scene overlay) is owned by exactly one source path and verified by exactly one canonical test.

**Independent Test**: The catalog source `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.{c,h}` plus the `dx12` scene + objective + lane source files are the only owning paths. Catalog mutation safety + disabled diagnostics tests assert that no second owning path exists.

### US3 — Growth Path Without New Slices (Priority: P2)

As a developer, adding a new gameplay placement, checkpoint, route signature, or lane behavior MUST extend the single owning path and the existing umbrella ctest filter. New numbered specs for the same surface are prohibited.

**Independent Test**: Workflow doc in `artifacts/native/014-add-gameplay-assets/catalog-diagnostics.md` is the canonical growth runbook; this spec links to it instead of duplicating.

## Functional Requirements

- **FR-001**: All coherent-world catalog state MUST be expressed via `BananaPocDemoSceneCatalog*` and `BananaPocDemoSceneGameplayPlacement` in the single owning path.
- **FR-002**: All transition continuity, drift, and roundtrip checks MUST live under tests #36, #37, #38 (transition_continuity, transition_drift, continuity_roundtrip).
- **FR-003**: Coherent profile, launch signature, failure path, mutation safety, and disabled profile MUST live under tests #33, #34, #35, #39, #40.
- **FR-004**: Objective completion MUST be owned by `dx12_objective_policy_test` (#50).
- **FR-005**: Gameplay click lane, gameplay lane, scene flow, scene overlay, projection policy MUST be owned by tests #48, #49, #51, #52, #53.
- **FR-006**: Tick phasing (input, post, budget) MUST be owned by tests #57–#60.
- **FR-007**: New cohesion work MUST extend FR-001 through FR-006 in place and MUST NOT create a new numbered spec for the same surface.
- **FR-008**: The end-to-end "is this a real MMO?" demo MUST be owned by `banana_runtime_full_mmo_demo_test` and MUST exercise the real engine + runtime ABIs (not registry math) in one process: `engine_init` through the real launch gate (env supplied by CMake), multi-player upsert via `runtime_player_api_upsert`, click-to-move driven through `runtime_player_motion_tick`, the merchant economy via `engine_npc_merchant_get_price` / `_trade_buy` / `_trade_sell`, pickup pipeline via `runtime_gameplay_service_tick`, multiplayer presence via `runtime_player_sync_abi_*` (mark-seen / staleness / deactivate / rejoin), and coherent-world catalog transitions with deterministic + self-loop signatures.
- **FR-009**: The "coherent game world" surface in native is the full native engine surface. The canonical signal is the **full native ctest suite** (currently 107 tests) covering: launch gating (`runtime_launch_gate_*`, spec 008 surface), persistent terrain/chunks (`runtime_terrain_*`, `terrain_chunks_*`, `chunk_stream_packet`, specs 009/010 surface), player API/sync/motion/builds/resources/gateway/merchant-adapter, merchant economy (`*merchant*`), gameplay service + interactive collection/spawner + full-MMO demo, engine lifecycle/state/tick/host/aux/serialize, physics/collider/raycast, render/mesh/material/headless-frame, controller (system/dispatch/attach/kind/sync/metrics), input (abi/contract/binding/click-policy), native UI ABI + UI system, ABI/architecture guards (`engine_typed_abi_validation`, `runtime_compile_guard_abi`, `architecture_dependency_guard`, `application_service_ports`, `domain_adapters`), parallel/Amdahl, and the demo-scene catalog / dx12 cohesion-lane tests already enumerated in FR-001 through FR-006. Any new native test belongs to this surface by default; exclusions MUST be justified in the spec.
- **FR-010**: Demo-scene gameplay placements MUST carry an explicit team identity, including a Bean enemy team (`bean`) represented as a green banana model, and Banana-vs-Bean hostility MUST be encoded in the catalog contract (`banana` and `bean` are enemies; same-team pairings are not hostile).
- **FR-011**: Runtime gameplay ticks MUST support dynamic Banana-vs-Bean warfront expansion by spawning additional combat/wildlife reinforcements when hostile teams are actively engaged within policy-defined radius and budget limits.
- **FR-012**: Runtime gameplay/orchestration MUST expand terrain frontier chunk rings when Banana-vs-Bean crowd pressure exceeds policy threshold, with cooldown and max-frontier limits, and MUST cycle biome stage layers across expansions.
- **FR-013**: Gameplay model IDs MUST resolve through a deterministic catalog model-id -> generated-vector profile mapping that scaffolds mesh generation with existing vector mesh builders.
- **FR-014**: The war expansion flow is a repeating spec loop and MUST remain encoded in `.specify/specs/031-unified-coherent-world/spec.md`, `.specify/integrations/war-expansion-loop.md`, and `.specify/memory/war-expansion-loop.md`.
- **FR-015**: Runtime war simulation MUST apply escalation tiers (`peaceful`, `skirmish`, `siege`) based on active war population pressure and MUST use those tiers to adjust reinforcement throughput and terrain-expansion cadence.

## Full Engine Baseline (canonical)

```bash
ctest --test-dir out/v3-native -C Debug --output-on-failure
```

Baseline (2026-05-27): **107/107 passed** in 14.77s. Evidence: `artifacts/native/031-unified-coherent-world/full-engine-baseline.txt`.

The narrow umbrella filter below is the cohesion-lane subset retained as a fast smoke signal for catalog/dx12/tick/camera/move-target work; it is a strict subset of the full engine baseline.

## Umbrella Test Filter (cohesion-lane subset)

```bash
ctest --test-dir out/v3-native -C Debug --output-on-failure -R \
  "banana_(runtime_full_mmo_demo|runtime_demo_scene_catalog_.+|dx12_(objective_policy|scene_flow|scene_overlay_frame|projection_policy|gameplay_lane|gameplay_click_lane)|runtime_tick_(input_phase|post_phase|budget_policy)|runtime_engine_tick|runtime_camera_follow_policy|runtime_move_target_domain)_test"
```

Baseline (2026-05-27): **29/29 passed** in 4.60s (28 cohesion-lane tests + `banana_runtime_full_mmo_demo_test`).

## Evidence

- Full engine baseline: `artifacts/native/031-unified-coherent-world/full-engine-baseline.txt` (107/107, 14.77s).
- Cohesion-lane umbrella baseline: `artifacts/native/031-unified-coherent-world/umbrella-baseline.txt` (29/29, 4.60s).
- Full-MMO demo evidence: `artifacts/native/031-unified-coherent-world/full-mmo-demo.txt`.
- Combat vs wildlife war evidence: `artifacts/native/031-unified-coherent-world/combat-wildlife-war-evidence.txt`.
- War terrain expansion loop evidence: `artifacts/native/031-unified-coherent-world/combat-wildlife-war-evidence.txt` (includes runtime terrain expansion, escalation-tier orchestration, and vector profile tests).


## API Unit Test Baseline (cross-domain verification)

The unified coherent world is verified across native plus TypeScript domains. In addition to the full native ctest baseline, the Banana TypeScript API, React shell, and shared package tests all pass as of 2026-05-27.

**Evidence:**

- API unit baseline: `artifacts/typescript/api/coherent-world-units.log` (14 pass, 0 fail, stable auth/context/route subset)
- API integration baseline: `artifacts/typescript/api/coherent-world-integration.log` (45 pass, 0 fail, 14 files)
- React shell baseline: `artifacts/typescript/react/coherent-world-baseline.log` (7 pass, 0 fail, 3 files)
- Shared resilience baseline: `artifacts/typescript/shared/resilience/coherent-world-baseline.log` (12 pass, 0 fail, 3 files)
- Shared UI baseline: `artifacts/typescript/shared/ui/coherent-world-baseline.log` (30 pass, 0 fail, 11 files)

All new cross-domain verification should be tracked here, not in superseded slices (for example 016-gameplay-api-continuity).

---
## Out of Scope

- Steam release readiness (owned by `001-steamworks-release-readiness`).
- Native runtime/render modularization (owned by `005-native-runtime-render-modularization`).
- API MMO recenter (owned by `007-api-mmo-recenter`).
- Gameplay asset growth workflow (owned by `014-add-gameplay-assets`).
