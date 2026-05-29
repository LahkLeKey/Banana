# War Expansion Loop Integration Contract

This contract defines the repeating world-expansion loop for Banana vs Bean runtime evolution.

## Loop Cadence

Run this loop repeatedly while evolving AI behavior:

1. Scaffold generated-vector assets for newly introduced gameplay model IDs.
2. Validate model identity to vector profile mapping in catalog tests.
3. Run focused war pressure/intelligence tests (`runtime_tick_orchestration_war_terrain_expansion_test`, `runtime_tick_orchestration_war_escalation_test`, `runtime_tick_orchestration_war_intelligence_growth_test`, `runtime_tick_orchestration_war_game_of_life_sentience_test`, `runtime_tick_orchestration_war_overcrowd_expansion_test`, `runtime_tick_orchestration_war_long_run_soak_test`, `runtime_gameplay_warfront_expansion_test`, `runtime_gameplay_warfront_comeback_bonus_test`, `runtime_gameplay_warfront_negotiate_factory_test` (includes floor-enabled + threshold-disabled scenarios), `runtime_gameplay_warfront_intelligence_feature_unlock_test`, `runtime_full_mmo_demo_test`, war evidence suite).
4. Refresh coherent-world evidence artifacts.
5. Append the next iteration notes to `.specify/memory/war-expansion-loop.md`.

## Runtime Expansion Rule

When active Banana/Bean war population crosses pressure threshold, runtime expands the terrain frontier in chunk rings and cycles biome stage layers. Expansion is governed by env-backed policy knobs:

- `BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT`
- `BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS`
- `BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS`
- `BANANA_CONTROLLER_WAR_SKIRMISH_PCT`, `BANANA_CONTROLLER_WAR_SIEGE_PCT`
- `BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS`, `BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS`
- `BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION`, `BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION`
- `BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH`, `BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE`
- `BANANA_CONTROLLER_WAR_INTELLIGENCE_LEVEL_THRESHOLD_TICKS`, `BANANA_CONTROLLER_WAR_INTELLIGENCE_MAX_STAGE`
- `BANANA_CONTROLLER_WAR_INTELLIGENCE_REINFORCEMENT_BONUS_PER_STAGE`, `BANANA_CONTROLLER_WAR_INTELLIGENCE_FRONTIER_BONUS_PER_STAGE`
- `BANANA_CONTROLLER_WAR_OVERCROWD_PCT`, `BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS`
- `BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE`

Escalation + intelligence stage drive generated reinforcement model IDs so each biome stage can scaffold new vector assets while preserving deterministic catalog/runtime profile resolution. Keep deterministic full-biome cycle assertions (tropical, glacier, urban, volcanic) in warfront integration coverage.

At higher intelligence stages, keep apex generated-vector reinforcement scaffolding active (`banana-phalanx-*`, `bean-colossus-*`) and verify profile resolution through the existing vector profile pipeline.

At stage 5+, scaffold mythic generated-vector reinforcement families (`banana-archon-*`, `bean-leviathan-*`) so intelligence growth keeps introducing new deterministic model families across all biome layers.

Keep runtime observability synced with this loop by exposing overcrowd policy/burst values, frontier depth, biome-stage index, apex-active status, and mythic-active status through engine getters + HUD overlay text.

Track reinforcement telemetry channels for apex and mythic families explicitly (banana/bean) so generated-vector stage transitions are measurable without inferring from aggregate siege/warbrute totals.

Keep stage-indexed apex/mythic telemetry rollups available through runtime getters + HUD so operators can inspect which intelligence stage produced each high-tier channel burst.

Run deterministic Game-of-Life style war-lane ticks to keep AI simulation pressure moving forward and use those lane outputs to steer procedural biome bias for terrain expansion under policy control.

Treat Banana and Bean combatants as sentient humanoid factions (not plants): keep sentience coordination/empathy telemetry visible, scaffold deterministic per-faction behavior modes (`hold-line`, `flank`, `regroup`, `negotiate`), let those modes drive deterministic tactical reinforcement placement geometry (flank wraps around opponent-facing normal; regroup forms rear echelons), and allow humanoid coordination + behavior mode to bias comeback allocation under bounded policy controls.

When both factions remain in `negotiate` mode over consecutive ticks, apply bounded de-escalation trimming to reinforcement spawn pressure, expose streak/trim telemetry, and reset trim state immediately once either side leaves negotiation.

At or above the configured intelligence threshold, preserve a bounded negotiate-continuation minimum reinforcement floor so conflict telemetry channels remain active; below the threshold, allow full de-escalation trim to zero to preserve staged behavior separation.

Keep sentience-mode skirmish asset channels scaffolded for DX12 runtime visibility (`banana-scout-*` / `bean-raider-*` with `flank`, `regroup`, `envoy` variants) and expose per-faction mode channel counters (H/F/R/N) through engine getters + HUD overlay text so mode transitions are observable as in-game UI components.

Reserve scene-browser variant `6` (`banana-war-sentience-showcase`) as the deterministic preview slice that pre-seeds flank/regroup/envoy placements so operators can verify sentience assets immediately on launch.

When Bean team pressure exceeds Banana pressure, reinforcement allocation should bias toward Banana spawns as a natural comeback effect; keep this behavior under automated coverage with `runtime_gameplay_warfront_comeback_bonus_test`.

## Spec Ownership

All loop updates extend `.specify/specs/031-unified-coherent-world/spec.md` and its evidence paths.
