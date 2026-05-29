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
                                                                    RuntimeWarEscalationTier tier)
{
    int effective = base_reinforcements;

    if (effective < 0)
        effective = 0;

    if (tier == RUNTIME_WAR_ESCALATION_SKIRMISH)
        effective += runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus();
    else if (tier == RUNTIME_WAR_ESCALATION_SIEGE)
        effective += runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus();

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

static void runtime_tick_orchestration_expand_terrain_for_war(RuntimeTickOrchestrationContext *orchestration,
                                                              int active_war_npcs,
                                                              int population_cap,
                                                              RuntimeWarEscalationTier war_tier)
{
    EngineRuntimeState *state = NULL;
    int pressure_pct = 0;
    int cooldown_ticks = 0;
    int effective_cooldown_ticks = 0;
    int max_frontier_chunks = 0;
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
    effective_cooldown_ticks = runtime_tick_orchestration_effective_war_expand_cooldown(cooldown_ticks,
                                                                                         war_tier);
    max_frontier_chunks = runtime_tick_budget_policy_controller_war_max_frontier_chunks();

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

    {
        int next_frontier = state->war_frontier_chunks + 1;
        int center = BANANA_ENGINE_TERRAIN_SIZE / 2;
        int radius_tiles = next_frontier * BANANA_ENGINE_TERRAIN_CHUNK_SIZE;
        int min_x = center - radius_tiles;
        int max_x = center + radius_tiles;
        int min_z = center - radius_tiles;
        int max_z = center + radius_tiles;
        int biome_layer = state->war_biome_stage_index % BANANA_ENGINE_TERRAIN_MAX_LAYERS;

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
        state->war_biome_stage_index = (state->war_biome_stage_index + 1) % BANANA_ENGINE_TERRAIN_MAX_LAYERS;

        fprintf(stdout,
            "[runtime] war-terrain-expansion frontier=%d/%d biome-stage=%d tier=%s active-war-npcs=%d threshold=%d cooldown=%d\n",
                state->war_frontier_chunks,
                max_frontier_chunks,
                state->war_biome_stage_index,
            runtime_tick_orchestration_war_escalation_label(war_tier),
                active_war_npcs,
            threshold_population,
            effective_cooldown_ticks);
    }
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
    effective_reinforcements = runtime_tick_orchestration_effective_war_reinforcements(base_reinforcements,
                                                                                        war_tier);

    orchestration->state->war_escalation_tier = war_tier;
    orchestration->state->war_effective_reinforcements_per_tick = effective_reinforcements;

    runtime_gameplay_service_tick(orchestration->state->world,
                                  orchestration->state->controllers,
                                  (int)(sizeof(orchestration->state->controllers) / sizeof(orchestration->state->controllers[0])),
                                  &orchestration->state->controller_count,
                                  orchestration->state->player_id,
                                  &orchestration->state->pbj_pickup_collected,
                                  6.0f,
                                  1.55f,
                                  runtime_tick_budget_policy_controller_war_radius(),
                                  effective_reinforcements,
                                  population_cap);

    runtime_tick_orchestration_expand_terrain_for_war(orchestration,
                                                      active_war_npcs,
                                                      population_cap,
                                                      war_tier);
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
