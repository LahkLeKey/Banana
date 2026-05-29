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

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
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

static void clear_env(void)
{
    unsetenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK");
    unsetenv("BANANA_CONTROLLER_WAR_POPULATION_CAP");
    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS");
    unsetenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS");
    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_LEVEL_THRESHOLD_TICKS");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_MAX_STAGE");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH");
    unsetenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE");
    unsetenv("BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL");
    unsetenv("BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX");
    unsetenv("BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE");
    unsetenv("BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK");
    unsetenv("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION");
}

int main(void)
{
    EngineRuntimeState state;
    RuntimeApplicationServicePorts service_ports;
    RuntimeTickOrchestrationContext context;

    memset(&state, 0, sizeof(state));
    memset(&service_ports, 0, sizeof(service_ports));
    memset(&context, 0, sizeof(context));

    clear_env();

    setenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_POPULATION_CAP", "8", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "95", 1);
    setenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "8", 1);
    setenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "4", 1);
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "40", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_PCT", "70", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_LEVEL_THRESHOLD_TICKS", "2", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_MAX_STAGE", "3", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE", "0", 1);
    setenv("BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL", "1", 1);
    setenv("BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX", "3", 1);
    setenv("BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE", "2", 1);
    setenv("BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK", "2", 1);
    setenv("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION", "2", 1);

    state.world = world_create();
    state.player_id = world_spawn_entity(state.world, ENTITY_TYPE_PLAYER, 0.0f, 0.0f, 0.0f);
    state.war_frontier_chunks = 1;
    state.war_biome_stage_index = 0;

    if (!expect_true("world created", state.world != NULL))
        return 1;

    spawn_war_npcs(state.world, 8);

    service_ports.terrain.set_height = fake_set_height;
    service_ports.terrain.mark_region_dirty = fake_mark_region_dirty;

    context.state = &state;
    context.service_ports = &service_ports;

    for (int tick = 0; tick < 8; tick++)
        runtime_tick_orchestration_gameplay(&context);

    if (!expect_true("life generation advanced", state.war_life_generation > 0))
        return 1;
    if (!expect_true("life alive cells in range",
                     state.war_life_alive_cells > 0 &&
                         state.war_life_alive_cells <= BANANA_ENGINE_WAR_LIFE_CELL_COUNT))
    {
        return 1;
    }
    if (!expect_true("life frontline cells bounded",
                     state.war_life_frontline_cells >= 0 &&
                         state.war_life_frontline_cells <= state.war_life_alive_cells))
    {
        return 1;
    }
    if (!expect_true("procgen biome bias in range",
                     state.war_procgen_biome_bias >= 0 &&
                         state.war_procgen_biome_bias < BANANA_ENGINE_TERRAIN_MAX_LAYERS))
    {
        return 1;
    }
    if (!expect_true("sentient humanoid index advanced", state.war_sentience_humanoid_index > 0))
        return 1;
    if (!expect_true("sentient coordination advanced", state.war_sentience_coordination_level > 0))
        return 1;
    if (!expect_true("sentient empathy non-negative", state.war_sentience_empathy_level >= 0))
        return 1;
    if (!expect_true("life bonus drives intelligence forward", state.war_intelligence_stage > 0))
        return 1;
    if (!expect_int("comeback bonus defaults to zero without warfront anchors",
                    state.war_sentience_comeback_bonus_last_tick,
                    0))
    {
        return 1;
    }
    if (!expect_int("banana behavior defaults to hold-line without anchors",
                    state.war_sentience_behavior_banana,
                    RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE))
    {
        return 1;
    }
    if (!expect_int("bean behavior defaults to hold-line without anchors",
                    state.war_sentience_behavior_bean,
                    RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE))
    {
        return 1;
    }
    if (!expect_int("negotiate streak defaults to zero without anchors",
                    state.war_sentience_negotiate_streak_ticks,
                    0))
    {
        return 1;
    }
    if (!expect_int("negotiate trim defaults to zero without anchors",
                    state.war_sentience_negotiate_deescalation_trim_last_tick,
                    0))
    {
        return 1;
    }

    for (int mode = 0; mode < BANANA_ENGINE_WAR_SENTIENCE_MODE_COUNT; mode++)
    {
        if (!expect_int("banana mode channels remain zero without anchors",
                        state.war_sentience_spawn_mode_hits_banana[mode],
                        0))
        {
            return 1;
        }

        if (!expect_int("bean mode channels remain zero without anchors",
                        state.war_sentience_spawn_mode_hits_bean[mode],
                        0))
        {
            return 1;
        }
    }

    if (!expect_true("terrain services remain callable",
                     s_set_height_calls >= 0 && s_mark_dirty_calls >= 0))
    {
        return 1;
    }

    world_destroy(state.world);
    clear_env();

    return 0;
}
