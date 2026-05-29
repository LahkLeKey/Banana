#include "runtime_tick_orchestration.h"

#include "../../engine/gameplay_service.h"
#include "../../tick/tick_budget_policy.h"

#include <stdio.h>
#include <string.h>

static int runtime_tick_orchestration_count_active_war_npcs(const World *world)
{
    int count = 0;

    if (!world)
        return 0;

    for (int i = 0; i < world->entity_count; i++)
    {
        const Entity *entity = world->entities[i];

        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_NPC)
            continue;

        if (strcmp(entity->controller_kind, "combat") == 0 ||
            strcmp(entity->controller_kind, "wildlife") == 0)
        {
            count += 1;
        }
    }

    return count;
}

static int runtime_tick_orchestration_count_unlocked_biomes(unsigned int biome_unlock_mask)
{
    int count = 0;

    for (int i = 0; i < BANANA_ENGINE_TERRAIN_MAX_LAYERS; i++)
    {
        if ((biome_unlock_mask & (1u << i)) != 0u)
            count += 1;
    }

    return count;
}

static RuntimeWarEscalationTier runtime_tick_orchestration_war_escalation_tier_for_population(int active_war_npcs,
                                                                                                int population_cap)
{
    int skirmish_pct = 0;
    int siege_pct = 0;
    int occupancy_pct = 0;

    if (population_cap <= 0 || active_war_npcs <= 0)
        return RUNTIME_WAR_ESCALATION_PEACEFUL;

    skirmish_pct = runtime_tick_budget_policy_controller_war_skirmish_pct();
    siege_pct = runtime_tick_budget_policy_controller_war_siege_pct();
    occupancy_pct = (active_war_npcs * 100) / population_cap;

    if (occupancy_pct >= siege_pct)
        return RUNTIME_WAR_ESCALATION_SIEGE;
    if (occupancy_pct >= skirmish_pct)
        return RUNTIME_WAR_ESCALATION_SKIRMISH;

    return RUNTIME_WAR_ESCALATION_PEACEFUL;
}

static int runtime_tick_orchestration_effective_war_reinforcements(int base_reinforcements,
                                                                    RuntimeWarEscalationTier tier,
                                                                    int war_intelligence_stage)
{
    int effective = base_reinforcements;
    int stage_bonus_per_level =
        runtime_tick_budget_policy_controller_war_intelligence_reinforcement_bonus_per_stage();

    if (effective < 0)
        effective = 0;

    if (tier == RUNTIME_WAR_ESCALATION_SKIRMISH)
        effective += runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus();
    else if (tier == RUNTIME_WAR_ESCALATION_SIEGE)
        effective += runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus();

    if (war_intelligence_stage > 0 && stage_bonus_per_level > 0)
        effective += (war_intelligence_stage * stage_bonus_per_level);

    if (effective > 32)
        effective = 32;

    return effective;
}

static int runtime_tick_orchestration_effective_war_expand_cooldown(int base_cooldown,
                                                                     RuntimeWarEscalationTier tier)
{
    int reduction = 0;
    int effective = base_cooldown;

    if (tier == RUNTIME_WAR_ESCALATION_SKIRMISH)
        reduction = runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction();
    else if (tier == RUNTIME_WAR_ESCALATION_SIEGE)
        reduction = runtime_tick_budget_policy_controller_war_siege_cooldown_reduction();

    effective -= reduction;
    if (effective < 1)
        effective = 1;

    return effective;
}

static const char *runtime_tick_orchestration_war_escalation_label(RuntimeWarEscalationTier tier)
{
    switch (tier)
    {
        case RUNTIME_WAR_ESCALATION_SKIRMISH:
            return "skirmish";
        case RUNTIME_WAR_ESCALATION_SIEGE:
            return "siege";
        case RUNTIME_WAR_ESCALATION_PEACEFUL:
        default:
            return "peaceful";
    }
}

static unsigned int runtime_tick_orchestration_hash_u32(unsigned int value)
{
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static int runtime_tick_orchestration_war_life_cell_is_alive(unsigned int cells, int x, int z)
{
    int bit_index = (z * BANANA_ENGINE_WAR_LIFE_GRID_DIM) + x;

    if (bit_index < 0 || bit_index >= BANANA_ENGINE_WAR_LIFE_CELL_COUNT)
        return 0;

    return ((cells >> bit_index) & 1u) != 0u ? 1 : 0;
}

static void runtime_tick_orchestration_war_life_set_cell(unsigned int *cells, int x, int z, int alive)
{
    int bit_index = (z * BANANA_ENGINE_WAR_LIFE_GRID_DIM) + x;
    unsigned int mask = 0u;

    if (!cells)
        return;
    if (bit_index < 0 || bit_index >= BANANA_ENGINE_WAR_LIFE_CELL_COUNT)
        return;

    mask = (1u << bit_index);
    if (alive)
        *cells |= mask;
    else
        *cells &= ~mask;
}

static void runtime_tick_orchestration_refresh_war_life_metrics(EngineRuntimeState *state)
{
    int alive = 0;
    int frontline = 0;

    if (!state)
        return;

    for (int z = 0; z < BANANA_ENGINE_WAR_LIFE_GRID_DIM; z++)
    {
        for (int x = 0; x < BANANA_ENGINE_WAR_LIFE_GRID_DIM; x++)
        {
            if (!runtime_tick_orchestration_war_life_cell_is_alive(state->war_life_cells, x, z))
                continue;

            alive += 1;
            if (x == 0 || z == 0 || x == (BANANA_ENGINE_WAR_LIFE_GRID_DIM - 1) || z == (BANANA_ENGINE_WAR_LIFE_GRID_DIM - 1))
                frontline += 1;
        }
    }

    state->war_life_alive_cells = alive;
    state->war_life_frontline_cells = frontline;
}

static void runtime_tick_orchestration_seed_war_life_cells(EngineRuntimeState *state,
                                                           int active_war_npcs,
                                                           RuntimeWarEscalationTier war_tier)
{
    unsigned int seed = 0u;
    unsigned int cells = 0u;

    if (!state)
        return;

    seed = runtime_tick_orchestration_hash_u32((unsigned int)(active_war_npcs + 1) * 2654435761u);
    seed ^= runtime_tick_orchestration_hash_u32((unsigned int)(state->war_frontier_chunks + 7) * 2246822519u);
    seed ^= runtime_tick_orchestration_hash_u32((unsigned int)(state->war_intelligence_stage + (int)war_tier + 3) * 3266489917u);

    for (int bit_index = 0; bit_index < BANANA_ENGINE_WAR_LIFE_CELL_COUNT; bit_index++)
    {
        seed = runtime_tick_orchestration_hash_u32(seed + (unsigned int)bit_index * 374761393u);
        if ((seed & 1u) != 0u)
            cells |= (1u << bit_index);
    }

    if (cells == 0u)
    {
        /* Stable 2x2 block keeps the lane deterministic if the initial hash degenerates. */
        cells = 0x0660u;
    }

    state->war_life_cells = cells;
    state->war_life_generation = 0;
    state->war_life_tick_counter = 0;
    runtime_tick_orchestration_refresh_war_life_metrics(state);
}

static int runtime_tick_orchestration_war_life_neighbor_count(unsigned int cells, int x, int z)
{
    int neighbor_count = 0;

    for (int dz = -1; dz <= 1; dz++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int nx = 0;
            int nz = 0;

            if (dx == 0 && dz == 0)
                continue;

            nx = x + dx;
            nz = z + dz;
            if (nx < 0 || nz < 0 || nx >= BANANA_ENGINE_WAR_LIFE_GRID_DIM || nz >= BANANA_ENGINE_WAR_LIFE_GRID_DIM)
                continue;

            neighbor_count += runtime_tick_orchestration_war_life_cell_is_alive(cells, nx, nz);
        }
    }

    return neighbor_count;
}

static void runtime_tick_orchestration_step_war_life(EngineRuntimeState *state)
{
    unsigned int next_cells = 0u;

    if (!state)
        return;

    for (int z = 0; z < BANANA_ENGINE_WAR_LIFE_GRID_DIM; z++)
    {
        for (int x = 0; x < BANANA_ENGINE_WAR_LIFE_GRID_DIM; x++)
        {
            int alive = runtime_tick_orchestration_war_life_cell_is_alive(state->war_life_cells, x, z);
            int neighbors = runtime_tick_orchestration_war_life_neighbor_count(state->war_life_cells, x, z);
            int next_alive = 0;

            if (alive)
                next_alive = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            else
                next_alive = (neighbors == 3) ? 1 : 0;

            runtime_tick_orchestration_war_life_set_cell(&next_cells, x, z, next_alive);
        }
    }

    if (next_cells == 0u)
        next_cells = 0x0660u;

    state->war_life_cells = next_cells;
    state->war_life_generation += 1;
    runtime_tick_orchestration_refresh_war_life_metrics(state);
}

static void runtime_tick_orchestration_update_war_sentience_metrics(EngineRuntimeState *state,
                                                                    RuntimeWarEscalationTier war_tier)
{
    int variance = 0;
    int sentience_gain = 0;
    int coordination = 0;
    int empathy = 0;

    if (!state)
        return;

    variance = runtime_tick_budget_policy_controller_war_procgen_biome_variance();
    if (variance > 0)
    {
        state->war_procgen_biome_bias =
            ((state->war_life_frontline_cells + state->war_life_generation) * variance) % BANANA_ENGINE_TERRAIN_MAX_LAYERS;
    }
    else
    {
        state->war_procgen_biome_bias = 0;
    }

    sentience_gain = runtime_tick_budget_policy_controller_war_sentience_gain_per_tick();
    if (sentience_gain > 0)
    {
        state->war_sentience_humanoid_index += sentience_gain;
        state->war_sentience_humanoid_index += (state->war_life_alive_cells / 4);
        if (war_tier == RUNTIME_WAR_ESCALATION_SIEGE)
            state->war_sentience_humanoid_index += 1;
    }

    if (state->war_sentience_humanoid_index < 0)
        state->war_sentience_humanoid_index = 0;
    if (state->war_sentience_humanoid_index > 999)
        state->war_sentience_humanoid_index = 999;

    coordination = (state->war_sentience_humanoid_index / 25) + (state->war_life_frontline_cells / 4);
    if (coordination > 12)
        coordination = 12;

    empathy = state->war_life_alive_cells - state->war_life_frontline_cells;
    if (empathy < 0)
        empathy = 0;
    empathy = 1 + (empathy / 2);
    if (war_tier == RUNTIME_WAR_ESCALATION_PEACEFUL && empathy > 0)
        empathy -= 1;
    if (empathy > 12)
        empathy = 12;

    state->war_sentience_coordination_level = coordination;
    state->war_sentience_empathy_level = empathy;
}

static void runtime_tick_orchestration_update_war_life(EngineRuntimeState *state,
                                                       int active_war_npcs,
                                                       RuntimeWarEscalationTier war_tier)
{
    int tick_interval = 0;

    if (!state)
        return;

    if (active_war_npcs <= 0 || war_tier == RUNTIME_WAR_ESCALATION_PEACEFUL)
    {
        if (state->war_sentience_humanoid_index > 0)
            state->war_sentience_humanoid_index -= 1;
        state->war_sentience_coordination_level = state->war_sentience_humanoid_index / 25;
        state->war_sentience_empathy_level = 0;
        return;
    }

    if (state->war_life_cells == 0u)
        runtime_tick_orchestration_seed_war_life_cells(state, active_war_npcs, war_tier);

    tick_interval = runtime_tick_budget_policy_controller_war_life_tick_interval();
    state->war_life_tick_counter += 1;
    if (state->war_life_tick_counter >= tick_interval)
    {
        state->war_life_tick_counter = 0;
        runtime_tick_orchestration_step_war_life(state);
    }
    else
    {
        runtime_tick_orchestration_refresh_war_life_metrics(state);
    }

    runtime_tick_orchestration_update_war_sentience_metrics(state, war_tier);
}

static int runtime_tick_orchestration_war_life_intelligence_bonus(const EngineRuntimeState *state)
{
    int bonus = 0;
    int max_bonus = 0;

    if (!state)
        return 0;

    max_bonus = runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max();
    if (max_bonus <= 0)
        return 0;

    bonus = state->war_life_alive_cells / 5;
    if (state->war_life_frontline_cells >= 4)
        bonus += 1;
    if (state->war_sentience_coordination_level >= 4)
        bonus += 1;

    if (bonus > max_bonus)
        bonus = max_bonus;
    if (bonus < 0)
        bonus = 0;

    return bonus;
}

static void runtime_tick_orchestration_update_war_intelligence(EngineRuntimeState *state,
                                                                RuntimeWarEscalationTier war_tier)
{
    int threshold_ticks = 0;
    int max_stage = 0;
    int progress_delta = 0;

    if (!state)
        return;

    threshold_ticks = runtime_tick_budget_policy_controller_war_intelligence_level_threshold_ticks();
    max_stage = runtime_tick_budget_policy_controller_war_intelligence_max_stage();

    if (war_tier == RUNTIME_WAR_ESCALATION_SIEGE)
        progress_delta = runtime_tick_budget_policy_controller_war_intelligence_progress_siege();
    else if (war_tier == RUNTIME_WAR_ESCALATION_SKIRMISH)
        progress_delta = runtime_tick_budget_policy_controller_war_intelligence_progress_skirmish();
    else if (state->war_intelligence_progress_ticks > 0)
        state->war_intelligence_progress_ticks -= 1;

    if (war_tier != RUNTIME_WAR_ESCALATION_PEACEFUL)
        progress_delta += runtime_tick_orchestration_war_life_intelligence_bonus(state);

    state->war_intelligence_progress_ticks += progress_delta;
    if (state->war_intelligence_progress_ticks < 0)
        state->war_intelligence_progress_ticks = 0;

    while (state->war_intelligence_stage < max_stage &&
           state->war_intelligence_progress_ticks >= threshold_ticks)
    {
        state->war_intelligence_stage += 1;
        state->war_intelligence_progress_ticks -= threshold_ticks;
        fprintf(stdout,
                "[runtime] war-intelligence-stage-up new-stage=%d tier=%s\n",
                state->war_intelligence_stage,
                runtime_tick_orchestration_war_escalation_label(war_tier));
    }
}

static void runtime_tick_orchestration_expand_terrain_for_war(RuntimeTickOrchestrationContext *orchestration,
                                                              int active_war_npcs,
                                                              int population_cap,
                                                              RuntimeWarEscalationTier war_tier,
                                                              int war_intelligence_stage)
{
    EngineRuntimeState *state = NULL;
    int pressure_pct = 0;
    int cooldown_ticks = 0;
    int effective_cooldown_ticks = 0;
    int max_frontier_chunks = 0;
    int overcrowd_pct = 0;
    int overcrowd_expand_bonus_chunks = 0;
    int overcrowd_intelligence_bonus_per_stage = 0;
    int occupancy_pct = 0;
    int expansion_attempts = 1;
    int frontier_bonus_per_stage =
        runtime_tick_budget_policy_controller_war_intelligence_frontier_bonus_per_stage();
    int procgen_biome_variance =
        runtime_tick_budget_policy_controller_war_procgen_biome_variance();
    int threshold_population = 0;

    if (!orchestration || !orchestration->state || !orchestration->service_ports)
        return;

    state = orchestration->state;

    if (!orchestration->service_ports->terrain.set_height ||
        !orchestration->service_ports->terrain.mark_region_dirty)
    {
        return;
    }

    population_cap = runtime_tick_budget_policy_controller_war_population_cap();
    pressure_pct = runtime_tick_budget_policy_controller_war_terrain_pressure_pct();
    cooldown_ticks = runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks();
    overcrowd_pct = runtime_tick_budget_policy_controller_war_overcrowd_pct();
    overcrowd_expand_bonus_chunks =
        runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks();
    overcrowd_intelligence_bonus_per_stage =
        runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage();
    effective_cooldown_ticks = runtime_tick_orchestration_effective_war_expand_cooldown(cooldown_ticks,
                                                                                         war_tier);
    max_frontier_chunks = runtime_tick_budget_policy_controller_war_max_frontier_chunks();

    if (war_intelligence_stage > 0 && frontier_bonus_per_stage > 0)
        max_frontier_chunks += (war_intelligence_stage * frontier_bonus_per_stage);

    if (max_frontier_chunks < BANANA_ENGINE_TERRAIN_MAX_LAYERS)
        max_frontier_chunks = BANANA_ENGINE_TERRAIN_MAX_LAYERS;

    if (population_cap <= 0 || pressure_pct <= 0 || effective_cooldown_ticks <= 0)
        return;

    if (max_frontier_chunks > (BANANA_ENGINE_TERRAIN_CHUNK_COLS / 2))
        max_frontier_chunks = (BANANA_ENGINE_TERRAIN_CHUNK_COLS / 2);
    if (max_frontier_chunks < 1)
        max_frontier_chunks = 1;

    threshold_population = (population_cap * pressure_pct + 99) / 100;

    if (active_war_npcs < threshold_population)
    {
        state->war_terrain_expand_tick_counter = 0;
        return;
    }

    state->war_terrain_expand_tick_counter += 1;
    if (state->war_terrain_expand_tick_counter < effective_cooldown_ticks)
        return;

    state->war_terrain_expand_tick_counter = 0;

    if (state->war_frontier_chunks >= max_frontier_chunks)
        return;

    occupancy_pct = (active_war_npcs * 100) / population_cap;
    if (occupancy_pct >= overcrowd_pct)
    {
        if (overcrowd_expand_bonus_chunks > 0)
        {
            expansion_attempts += overcrowd_expand_bonus_chunks;

            if (war_tier == RUNTIME_WAR_ESCALATION_SIEGE)
                expansion_attempts += 1;
        }

        if (war_intelligence_stage > 0 && overcrowd_intelligence_bonus_per_stage > 0)
        {
            expansion_attempts += (war_intelligence_stage * overcrowd_intelligence_bonus_per_stage);
        }
    }

    if (expansion_attempts < 1)
        expansion_attempts = 1;
    if (expansion_attempts > 8)
        expansion_attempts = 8;

    for (int burst_index = 0; burst_index < expansion_attempts; burst_index++)
    {
        int next_frontier = 0;
        int center = 0;
        int radius_tiles = 0;
        int min_x = 0;
        int max_x = 0;
        int min_z = 0;
        int max_z = 0;
        int biome_layer = 0;

        if (state->war_frontier_chunks >= max_frontier_chunks)
            break;

        next_frontier = state->war_frontier_chunks + 1;
        center = BANANA_ENGINE_TERRAIN_SIZE / 2;
        radius_tiles = next_frontier * BANANA_ENGINE_TERRAIN_CHUNK_SIZE;
        min_x = center - radius_tiles;
        max_x = center + radius_tiles;
        min_z = center - radius_tiles;
        max_z = center + radius_tiles;
        biome_layer = state->war_biome_stage_index % BANANA_ENGINE_TERRAIN_MAX_LAYERS;
        if (procgen_biome_variance > 0)
        {
            biome_layer = (biome_layer + state->war_procgen_biome_bias) % BANANA_ENGINE_TERRAIN_MAX_LAYERS;
        }

        if (min_x < 0)
            min_x = 0;
        if (min_z < 0)
            min_z = 0;
        if (max_x >= BANANA_ENGINE_TERRAIN_SIZE)
            max_x = BANANA_ENGINE_TERRAIN_SIZE - 1;
        if (max_z >= BANANA_ENGINE_TERRAIN_SIZE)
            max_z = BANANA_ENGINE_TERRAIN_SIZE - 1;

        for (int z = min_z; z <= max_z; z++)
        {
            for (int x = min_x; x <= max_x; x++)
            {
                if (!(x == min_x || x == max_x || z == min_z || z == max_z))
                    continue;

                orchestration->service_ports->terrain.set_height(state,
                                                                 x,
                                                                 z,
                                                                 biome_layer);
            }
        }

        orchestration->service_ports->terrain.mark_region_dirty(state,
                                                                min_x,
                                                                min_z,
                                                                max_x,
                                                                max_z);

        state->war_frontier_chunks = next_frontier;
        state->war_biome_unlock_mask |= (1u << biome_layer);
        state->war_biome_stage_index = (state->war_biome_stage_index + 1) % BANANA_ENGINE_TERRAIN_MAX_LAYERS;
    }

    fprintf(stdout,
            "[runtime] war-terrain-expansion frontier=%d/%d biome-stage=%d tier=%s intelligence=%d biomes=%d/%d active-war-npcs=%d occupancy-pct=%d threshold=%d cooldown=%d burst=%d intel-burst-per-stage=%d procgen-bias=%d life=%d/%d\n",
            state->war_frontier_chunks,
            max_frontier_chunks,
            state->war_biome_stage_index,
            runtime_tick_orchestration_war_escalation_label(war_tier),
            war_intelligence_stage,
            runtime_tick_orchestration_count_unlocked_biomes(state->war_biome_unlock_mask),
            BANANA_ENGINE_TERRAIN_MAX_LAYERS,
            active_war_npcs,
            occupancy_pct,
            threshold_population,
            effective_cooldown_ticks,
            expansion_attempts,
            overcrowd_intelligence_bonus_per_stage,
            state->war_procgen_biome_bias,
            state->war_life_alive_cells,
            state->war_life_frontline_cells);
}

void runtime_tick_orchestration_gameplay(void *context)
{
    RuntimeTickOrchestrationContext *orchestration =
        (RuntimeTickOrchestrationContext *)context;
    int population_cap = 0;
    int active_war_npcs = 0;
    int base_reinforcements = 0;
    int effective_reinforcements = 0;
    RuntimeWarEscalationTier war_tier = RUNTIME_WAR_ESCALATION_PEACEFUL;

    if (!orchestration || !orchestration->state)
        return;

    population_cap = runtime_tick_budget_policy_controller_war_population_cap();
    active_war_npcs = runtime_tick_orchestration_count_active_war_npcs(orchestration->state->world);
    base_reinforcements = runtime_tick_budget_policy_controller_war_reinforcements_per_tick();
    war_tier = runtime_tick_orchestration_war_escalation_tier_for_population(active_war_npcs,
                                                                              population_cap);

    if (orchestration->state->war_biome_unlock_mask == 0u)
    {
        int initial_biome = orchestration->state->war_biome_stage_index % BANANA_ENGINE_TERRAIN_MAX_LAYERS;
        if (initial_biome < 0)
            initial_biome = 0;
        orchestration->state->war_biome_unlock_mask = (1u << initial_biome);
    }

    runtime_tick_orchestration_update_war_life(orchestration->state,
                                               active_war_npcs,
                                               war_tier);

    runtime_tick_orchestration_update_war_intelligence(orchestration->state, war_tier);

    effective_reinforcements = runtime_tick_orchestration_effective_war_reinforcements(
        base_reinforcements,
        war_tier,
        orchestration->state->war_intelligence_stage);

    orchestration->state->war_escalation_tier = war_tier;
    orchestration->state->war_effective_reinforcements_per_tick = effective_reinforcements;

    runtime_gameplay_service_tick(orchestration->state->world,
                                  orchestration->state,
                                  orchestration->state->controllers,
                                  (int)(sizeof(orchestration->state->controllers) / sizeof(orchestration->state->controllers[0])),
                                  &orchestration->state->controller_count,
                                  orchestration->state->player_id,
                                  &orchestration->state->pbj_pickup_collected,
                                  6.0f,
                                  1.55f,
                                  runtime_tick_budget_policy_controller_war_radius(),
                                  effective_reinforcements,
                                  population_cap,
                                  (int)war_tier,
                                  orchestration->state->war_intelligence_stage,
                                  orchestration->state->war_biome_stage_index);

    runtime_tick_orchestration_expand_terrain_for_war(orchestration,
                                                      active_war_npcs,
                                                      population_cap,
                                                      war_tier,
                                                      orchestration->state->war_intelligence_stage);
}

void runtime_tick_orchestration_render_scene(void *context)
{
    RuntimeTickOrchestrationContext *orchestration =
        (RuntimeTickOrchestrationContext *)context;

    if (!orchestration || !orchestration->state || !orchestration->service_ports ||
        !orchestration->service_ports->render.submit_scene)
        return;

    orchestration->service_ports->render.submit_scene(orchestration->state);
}
