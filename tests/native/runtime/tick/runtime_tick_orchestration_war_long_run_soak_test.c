#include "runtime/orchestration/tick/runtime_tick_orchestration.h"

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <stdlib.h>
#define setenv(name, value, overwrite) _putenv_s((name), (value))
#define unsetenv(name) _putenv_s((name), "")
#endif

static int s_set_height_calls = 0;
static int s_mark_dirty_calls = 0;

static int fake_set_height(EngineRuntimeState *state, int x, int z, int elevation)
{
    (void)state;
    (void)x;
    (void)z;
    (void)elevation;
    s_set_height_calls += 1;
    return 1;
}

static void fake_mark_region_dirty(EngineRuntimeState *state,
                                   int min_x,
                                   int min_z,
                                   int max_x,
                                   int max_z)
{
    (void)state;
    (void)min_x;
    (void)min_z;
    (void)max_x;
    (void)max_z;
    s_mark_dirty_calls += 1;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static int count_unlocked_biomes(unsigned int unlock_mask)
{
    int count = 0;

    for (int i = 0; i < BANANA_ENGINE_TERRAIN_MAX_LAYERS; i++)
    {
        if ((unlock_mask & (1u << i)) != 0u)
            count += 1;
    }

    return count;
}

int main(void)
{
    EngineRuntimeState state;
    RuntimeApplicationServicePorts service_ports;
    RuntimeTickOrchestrationContext context;
    int previous_frontier = 0;
    int previous_unlock = 0;
    int max_frontier = BANANA_ENGINE_TERRAIN_CHUNK_COLS / 2;

    memset(&state, 0, sizeof(state));
    memset(&service_ports, 0, sizeof(service_ports));
    memset(&context, 0, sizeof(context));

    setenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_POPULATION_CAP", "16", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "50", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "2", 1);
    setenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "4", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "40", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_PCT", "80", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_LEVEL_THRESHOLD_TICKS", "2", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_MAX_STAGE", "5", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH", "1", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE", "2", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_FRONTIER_BONUS_PER_STAGE", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_PCT", "90", 1);
    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS", "2", 1);
    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE", "1", 1);

    state.world = world_create();
    state.player_id = world_spawn_entity(state.world, ENTITY_TYPE_PLAYER, 0.0f, 0.0f, 0.0f);
    state.war_frontier_chunks = 0;
    state.war_biome_stage_index = 0;

    for (int i = 0; i < 16; i++)
    {
        EntityId npc_id = world_spawn_entity(state.world,
                                             ENTITY_TYPE_NPC,
                                             (float)i,
                                             0.0f,
                                             (float)i);
        Entity *npc = world_get_entity(state.world, npc_id);
        if (npc)
        {
            strncpy(npc->controller_kind,
                    (i % 2 == 0) ? "combat" : "wildlife",
                    sizeof(npc->controller_kind) - 1);
            npc->controller_kind[sizeof(npc->controller_kind) - 1] = '\0';
        }
    }

    service_ports.terrain.set_height = fake_set_height;
    service_ports.terrain.mark_region_dirty = fake_mark_region_dirty;

    context.state = &state;
    context.service_ports = &service_ports;

    if (max_frontier < BANANA_ENGINE_TERRAIN_MAX_LAYERS)
        max_frontier = BANANA_ENGINE_TERRAIN_MAX_LAYERS;

    previous_frontier = state.war_frontier_chunks;
    previous_unlock = count_unlocked_biomes(state.war_biome_unlock_mask);

    for (int tick = 0; tick < 48; tick++)
    {
        int current_unlock = 0;

        runtime_tick_orchestration_gameplay(&context);

        current_unlock = count_unlocked_biomes(state.war_biome_unlock_mask);

        if (!expect_true("frontier stays monotonic", state.war_frontier_chunks >= previous_frontier))
            return 1;
        if (!expect_true("frontier stays within world cap", state.war_frontier_chunks <= max_frontier))
            return 1;
        if (!expect_true("biome unlock count stays monotonic", current_unlock >= previous_unlock))
            return 1;
        if (!expect_true("biome unlock count stays bounded",
                         current_unlock <= BANANA_ENGINE_TERRAIN_MAX_LAYERS))
            return 1;
        if (!expect_true("biome stage index stays in range",
                         state.war_biome_stage_index >= 0 &&
                             state.war_biome_stage_index < BANANA_ENGINE_TERRAIN_MAX_LAYERS))
            return 1;

        previous_frontier = state.war_frontier_chunks;
        previous_unlock = current_unlock;
    }

    if (!expect_int("frontier saturates at world cap", state.war_frontier_chunks, max_frontier))
        return 1;
    if (!expect_int("all biome layers unlock in soak",
                    count_unlocked_biomes(state.war_biome_unlock_mask),
                    BANANA_ENGINE_TERRAIN_MAX_LAYERS))
        return 1;
    if (!expect_int("intelligence stage saturates at configured max", state.war_intelligence_stage, 5))
        return 1;
    if (!expect_true("terrain dirty marks emitted", s_mark_dirty_calls > 0))
        return 1;
    if (!expect_true("terrain writes emitted", s_set_height_calls > 0))
        return 1;

    world_destroy(state.world);

    unsetenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK");
    unsetenv("BANANA_CONTROLLER_WAR_POPULATION_CAP");
    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS");
    unsetenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS");
    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION");
    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_LEVEL_THRESHOLD_TICKS");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_MAX_STAGE");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_FRONTIER_BONUS_PER_STAGE");
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS");
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE");

    return 0;
}
