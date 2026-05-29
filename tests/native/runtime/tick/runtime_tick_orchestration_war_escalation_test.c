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

static void reset_counters(void)
{
    s_set_height_calls = 0;
    s_mark_dirty_calls = 0;
}

static void clear_war_env(void)
{
    unsetenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK");
    unsetenv("BANANA_CONTROLLER_WAR_POPULATION_CAP");
    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS");
    unsetenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS");
    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS");
    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS");
    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION");
    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION");
}

static void spawn_war_npcs(World *world, int count)
{
    for (int i = 0; i < count; i++)
    {
        EntityId npc_id = world_spawn_entity(world,
                                             ENTITY_TYPE_NPC,
                                             (float)i,
                                             0.0f,
                                             (float)i);
        Entity *npc = world_get_entity(world, npc_id);
        if (!npc)
            continue;

        strncpy(npc->controller_kind,
                (i % 2 == 0) ? "combat" : "wildlife",
                sizeof(npc->controller_kind) - 1);
        npc->controller_kind[sizeof(npc->controller_kind) - 1] = '\0';
    }
}

static int run_skirmish_tier_test(void)
{
    EngineRuntimeState state;
    RuntimeApplicationServicePorts service_ports;
    RuntimeTickOrchestrationContext context;

    memset(&state, 0, sizeof(state));
    memset(&service_ports, 0, sizeof(service_ports));
    memset(&context, 0, sizeof(context));

    reset_counters();

    setenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_POPULATION_CAP", "10", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "95", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "3", 1);
    setenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "3", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "40", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_PCT", "80", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "1", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS", "2", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "1", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION", "2", 1);

    state.world = world_create();
    state.player_id = world_spawn_entity(state.world, ENTITY_TYPE_PLAYER, 0.0f, 0.0f, 0.0f);
    state.war_frontier_chunks = 1;
    state.war_biome_stage_index = 0;
    spawn_war_npcs(state.world, 5);

    service_ports.terrain.set_height = fake_set_height;
    service_ports.terrain.mark_region_dirty = fake_mark_region_dirty;

    context.state = &state;
    context.service_ports = &service_ports;

    runtime_tick_orchestration_gameplay(&context);

    if (!expect_int("skirmish tier selected", (int)state.war_escalation_tier, (int)RUNTIME_WAR_ESCALATION_SKIRMISH))
        return 0;
    if (!expect_int("frontier unchanged below pressure threshold", state.war_frontier_chunks, 1))
        return 0;
    if (!expect_int("no terrain dirty write when below threshold", s_mark_dirty_calls, 0))
        return 0;

    world_destroy(state.world);
    clear_war_env();
    return 1;
}

static int run_siege_cooldown_reduction_test(void)
{
    EngineRuntimeState state;
    RuntimeApplicationServicePorts service_ports;
    RuntimeTickOrchestrationContext context;

    memset(&state, 0, sizeof(state));
    memset(&service_ports, 0, sizeof(service_ports));
    memset(&context, 0, sizeof(context));

    reset_counters();

    setenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_POPULATION_CAP", "8", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "50", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "3", 1);
    setenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "3", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "40", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_PCT", "80", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "1", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS", "3", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "1", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION", "2", 1);

    state.world = world_create();
    state.player_id = world_spawn_entity(state.world, ENTITY_TYPE_PLAYER, 0.0f, 0.0f, 0.0f);
    state.war_frontier_chunks = 1;
    state.war_biome_stage_index = 0;
    spawn_war_npcs(state.world, 8);

    service_ports.terrain.set_height = fake_set_height;
    service_ports.terrain.mark_region_dirty = fake_mark_region_dirty;

    context.state = &state;
    context.service_ports = &service_ports;

    runtime_tick_orchestration_gameplay(&context);

    if (!expect_int("siege tier selected", (int)state.war_escalation_tier, (int)RUNTIME_WAR_ESCALATION_SIEGE))
        return 0;
    if (!expect_int("frontier expanded under siege cooldown reduction", state.war_frontier_chunks, 2))
        return 0;
    if (!expect_int("terrain marked dirty once", s_mark_dirty_calls, 1))
        return 0;
    if (!expect_int("terrain write executed", s_set_height_calls > 0 ? 1 : 0, 1))
        return 0;

    world_destroy(state.world);
    clear_war_env();
    return 1;
}

int main(void)
{
    if (!run_skirmish_tier_test())
        return 1;

    if (!run_siege_cooldown_reduction_test())
        return 1;

    return 0;
}
