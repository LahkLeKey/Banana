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

    memset(&state, 0, sizeof(state));
    memset(&service_ports, 0, sizeof(service_ports));
    memset(&context, 0, sizeof(context));

    setenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_POPULATION_CAP", "8", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "50", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "1", 1);
    setenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "4", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "40", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_PCT", "80", 1);
    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_PCT", "90", 1);
    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE", "1", 1);

    state.world = world_create();
    state.player_id = world_spawn_entity(state.world, ENTITY_TYPE_PLAYER, 0.0f, 0.0f, 0.0f);
    state.war_frontier_chunks = 0;
    state.war_biome_stage_index = 0;
    state.war_intelligence_stage = 3;

    for (int i = 0; i < 8; i++)
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

    runtime_tick_orchestration_gameplay(&context);

    if (!expect_int("intelligence overcrowd burst expands to max frontier", state.war_frontier_chunks, 4))
        return 1;
    if (!expect_int("biome stage rotates across all biome rings", state.war_biome_stage_index, 0))
        return 1;
    if (!expect_int("multiple dirty calls emitted for burst", s_mark_dirty_calls, 4))
        return 1;
    if (!expect_int("terrain writes emitted during burst", s_set_height_calls > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("all biomes unlock during intelligence burst", count_unlocked_biomes(state.war_biome_unlock_mask), 4))
        return 1;

    world_destroy(state.world);

    unsetenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK");
    unsetenv("BANANA_CONTROLLER_WAR_POPULATION_CAP");
    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS");
    unsetenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS");
    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS");
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE");

    return 0;
}
