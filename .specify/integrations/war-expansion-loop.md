# War Expansion Loop Integration Contract

This contract defines the repeating world-expansion loop for Banana vs Bean runtime evolution.

## Loop Cadence

Run this loop repeatedly while evolving AI behavior:

1. Scaffold generated-vector assets for newly introduced gameplay model IDs.
2. Validate model identity to vector profile mapping in catalog tests.
3. Run focused war pressure tests (`runtime_tick_orchestration_war_terrain_expansion_test`, `runtime_tick_orchestration_war_escalation_test`, war evidence suite).
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

## Spec Ownership

All loop updates extend `.specify/specs/031-unified-coherent-world/spec.md` and its evidence paths.
