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
- **FR-009**: The "coherent game world" surface in native is the full native engine surface. The canonical signal is the **full native ctest suite** (currently 120 tests) covering: launch gating (`runtime_launch_gate_*`, spec 008 surface), persistent terrain/chunks (`runtime_terrain_*`, `terrain_chunks_*`, `chunk_stream_packet`, specs 009/010 surface), player API/sync/motion/builds/resources/gateway/merchant-adapter, merchant economy (`*merchant*`), gameplay service + interactive collection/spawner + full-MMO demo, engine lifecycle/state/tick/host/aux/serialize, physics/collider/raycast, render/mesh/material/headless-frame, controller (system/dispatch/attach/kind/sync/metrics), input (abi/contract/binding/click-policy), native UI ABI + UI system, ABI/architecture guards (`engine_typed_abi_validation`, `runtime_compile_guard_abi`, `architecture_dependency_guard`, `application_service_ports`, `domain_adapters`), parallel/Amdahl, and the demo-scene catalog / dx12 cohesion-lane tests already enumerated in FR-001 through FR-006. Any new native test belongs to this surface by default; exclusions MUST be justified in the spec.
- **FR-010**: Demo-scene gameplay placements MUST carry an explicit team identity, including a Bean enemy team (`bean`) represented as a green banana model, and Banana-vs-Bean hostility MUST be encoded in the catalog contract (`banana` and `bean` are enemies; same-team pairings are not hostile).
- **FR-011**: Runtime gameplay ticks MUST support dynamic Banana-vs-Bean warfront expansion by spawning additional combat/wildlife reinforcements when hostile teams are actively engaged within policy-defined radius and budget limits.
- **FR-012**: Runtime gameplay/orchestration MUST expand terrain frontier chunk rings when Banana-vs-Bean crowd pressure exceeds policy threshold, with cooldown and max-frontier limits, and MUST cycle biome stage layers across expansions.
- **FR-013**: Gameplay model IDs MUST resolve through a deterministic catalog model-id -> generated-vector profile mapping that scaffolds mesh generation with existing vector mesh builders.
- **FR-014**: The war expansion flow is a repeating spec loop and MUST remain encoded in `.specify/specs/031-unified-coherent-world/spec.md`, `.specify/integrations/war-expansion-loop.md`, and `.specify/memory/war-expansion-loop.md`.
- **FR-015**: Runtime war simulation MUST apply escalation tiers (`peaceful`, `skirmish`, `siege`) based on active war population pressure and MUST use those tiers to adjust reinforcement throughput and terrain-expansion cadence.
- **FR-016**: Runtime war simulation MUST progress intelligence stages from sustained escalation pressure, MUST unlock biome layers across repeated terrain expansions until all configured biome layers are reachable, and MUST scaffold tier+biome-aware generated reinforcement model IDs as intelligence advances.
- **FR-017**: Native warfront verification MUST include a deterministic full-biome reinforcement cycle in a continuous run (tropical, glacier, urban, volcanic) at elevated intelligence so stage-upgraded Banana/Bean model families are both observed.
- **FR-018**: Runtime warfront reinforcement allocation MUST apply a natural Banana comeback bonus whenever active Bean controller pressure exceeds active Banana controller pressure, biasing per-tick spawn targets toward Bananas so Bean-heavy fronts naturally self-correct.
- **FR-019**: Runtime war intelligence stage growth MUST scaffold apex gameplay reinforcement features at high intelligence by switching siege-tier families to generated-vector apex model IDs (`banana-phalanx-<biome>`, `bean-colossus-<biome>`) while preserving deterministic biome cycle coverage.
- **FR-020**: Runtime terrain expansion MUST support overcrowd burst expansion under policy control (`BANANA_CONTROLLER_WAR_OVERCROWD_PCT`, `BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS`) so high-pressure war populations can expand multiple chunk rings per cadence tick while staying bounded by frontier caps and biome rotation rules.
- **FR-021**: Runtime HUD/API telemetry MUST expose war overcrowd policy + burst values, intelligence-overcrowd burst policy, current frontier chunk depth, current biome-stage index, and apex/mythic feature activation state so operators can observe overcrowd and high-intelligence behavior live during warfront runs.
- **FR-022**: Native warfront verification MUST include a long-run soak scenario that validates monotonic/bounded frontier growth, monotonic biome unlock progression to all configured biome layers, and intelligence-stage saturation under sustained siege/overcrowd pressure.
- **FR-023**: Runtime war intelligence stage growth MUST scaffold mythic generated-vector reinforcement families at stage 5+ by switching siege-tier families to `banana-archon-<biome>` and `bean-leviathan-<biome>` model IDs while preserving deterministic full-biome cycle coverage.
- **FR-024**: Runtime terrain overcrowd burst expansion MUST support intelligence-driven scaling via `BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE`, allowing high-intelligence wars to burst-expand additional chunk rings while remaining bounded by frontier caps and biome rotation rules.
- **FR-025**: Runtime telemetry MUST track apex and mythic reinforcement channels per biome for both teams so stage-transition generated-vector scaffolding can be validated directly (`banana-phalanx`, `bean-colossus`, `banana-archon`, `bean-leviathan`) instead of inferred from aggregate siege/warbrute counters.
- **FR-026**: Runtime telemetry MUST expose stage-indexed apex/mythic reinforcement channel rollups for both teams so operators and tests can attribute high-tier generated-vector bursts to the exact intelligence stage that emitted them.
- **FR-027**: Runtime war orchestration MUST execute deterministic Game-of-Life style lane ticks during active conflict and MUST allow those lane metrics to contribute bounded intelligence progression so AI pressure continues advancing under sustained war conditions.
- **FR-028**: Runtime terrain expansion MUST support policy-controlled procedural biome bias derived from war life-lane metrics while preserving bounded frontier growth and eventual all-biome unlock behavior.
- **FR-029**: Banana and Bean factions MUST be treated as sentient humanoid combatants (not plant actors) in runtime telemetry/scaffolding, including bounded humanoid index, coordination, empathy, deterministic per-faction behavior modes (`hold-line`, `flank`, `regroup`, `negotiate`), mode-dependent tactical reinforcement placement geometry (flank wraps around opponent-facing normal; regroup forms rear echelons), and coordination-driven comeback bonus hooks.
- **FR-030**: When both factions resolve to `negotiate` behavior, runtime warfront expansion MUST apply bounded consecutive-tick de-escalation trimming to reinforcement spawn pressure, MUST expose negotiation streak + trim telemetry, and MUST reset de-escalation state when negotiation no longer holds.
- **FR-031**: Runtime warfront reinforcement scaffolding MUST emit sentience-mode-specific skirmish gameplay asset model IDs (`banana-scout-flank/regroup/envoy-<biome>`, `bean-raider-flank/regroup/envoy-<biome>`), and HUD/API telemetry MUST expose per-faction sentience mode channel totals (hold/flank/regroup/negotiate) so DX12 clients can render deterministic in-game sentience UI components.

## Full Engine Baseline (canonical)

```bash
ctest --test-dir out/v3-native -C Debug --output-on-failure
```

Baseline (2026-05-29): **122/122 passed** in 21.49s. Evidence: `artifacts/native/031-unified-coherent-world/full-engine-baseline.txt`.

The narrow umbrella filter below is the cohesion-lane subset retained as a fast smoke signal for catalog/dx12/tick/camera/move-target work; it is a strict subset of the full engine baseline.

## Umbrella Test Filter (cohesion-lane subset)

```bash
ctest --test-dir out/v3-native -C Debug --output-on-failure -R \
  "banana_(runtime_full_mmo_demo|runtime_demo_scene_catalog_.+|dx12_(objective_policy|scene_flow|scene_overlay_frame|projection_policy|gameplay_lane|gameplay_click_lane)|runtime_tick_(input_phase|post_phase|budget_policy)|runtime_engine_tick|runtime_camera_follow_policy|runtime_move_target_domain)_test"
```

Baseline (2026-05-29): **31/31 passed** in 5.42s (30 cohesion-lane tests + `banana_runtime_full_mmo_demo_test`).

## Evidence

- Full engine baseline: `artifacts/native/031-unified-coherent-world/full-engine-baseline.txt` (122/122, 21.49s).
- Cohesion-lane umbrella baseline: `artifacts/native/031-unified-coherent-world/umbrella-baseline.txt` (31/31, 5.42s).
- Full-MMO demo evidence: `artifacts/native/031-unified-coherent-world/full-mmo-demo.txt`.
- Combat vs wildlife war evidence: `artifacts/native/031-unified-coherent-world/combat-wildlife-war-evidence.txt`.
- War terrain expansion loop evidence: `artifacts/native/031-unified-coherent-world/combat-wildlife-war-evidence.txt` (includes runtime terrain expansion, escalation-tier orchestration, intelligence growth/all-biome unlock, deterministic full-biome reinforcement cycle coverage, stage-5 mythic reinforcement scaffolding, intelligence-overcrowd burst scaling, explicit apex/mythic telemetry channels, stage-indexed channel rollups, Game-of-Life life-lane intelligence drive, procedural biome bias steering, sentient humanoid comeback scaffolding with deterministic behavior modes and mode-dependent tactical placement geometry (flank-wrap + rear-echelon regroup), sentience-mode skirmish asset channels (`scout/raider` flank/regroup/envoy), negotiate-mode consecutive-tick de-escalation trim telemetry, HUD mode-channel counters, and vector profile tests).


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
