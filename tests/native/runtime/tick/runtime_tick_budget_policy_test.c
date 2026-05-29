#include "runtime/tick/tick_budget_policy.h"

#include <stdio.h>

#if defined(_WIN32)
#include <stdlib.h>
#define setenv(name, value, overwrite) _putenv_s((name), (value))
#define unsetenv(name) _putenv_s((name), "")
#endif

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    unsetenv("BANANA_TERRAIN_REBUILD_BUDGET");
    if (!expect_int("default budget", runtime_tick_budget_policy_terrain_chunks_per_tick(), 2))
        return 1;

    setenv("BANANA_TERRAIN_REBUILD_BUDGET", "8", 1);
    if (!expect_int("configured budget", runtime_tick_budget_policy_terrain_chunks_per_tick(), 8))
        return 1;

    setenv("BANANA_TERRAIN_REBUILD_BUDGET", "0", 1);
    if (!expect_int("minimum clamp", runtime_tick_budget_policy_terrain_chunks_per_tick(), 1))
        return 1;

    setenv("BANANA_TERRAIN_REBUILD_BUDGET", "9999", 1);
    if (!expect_int("maximum clamp", runtime_tick_budget_policy_terrain_chunks_per_tick(), 128))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_RADIUS");
    if (!expect_int("default controller war radius", (int)runtime_tick_budget_policy_controller_war_radius(), 8))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_RADIUS", "12", 1);
    if (!expect_int("configured controller war radius", (int)runtime_tick_budget_policy_controller_war_radius(), 12))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_RADIUS", "0", 1);
    if (!expect_int("minimum controller war radius clamp", (int)runtime_tick_budget_policy_controller_war_radius(), 1))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_RADIUS", "9999", 1);
    if (!expect_int("maximum controller war radius clamp", (int)runtime_tick_budget_policy_controller_war_radius(), 256))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK");
    if (!expect_int("default controller war reinforcements", runtime_tick_budget_policy_controller_war_reinforcements_per_tick(), 2))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "6", 1);
    if (!expect_int("configured controller war reinforcements", runtime_tick_budget_policy_controller_war_reinforcements_per_tick(), 6))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "-4", 1);
    if (!expect_int("minimum controller war reinforcements clamp", runtime_tick_budget_policy_controller_war_reinforcements_per_tick(), 0))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "1000", 1);
    if (!expect_int("maximum controller war reinforcements clamp", runtime_tick_budget_policy_controller_war_reinforcements_per_tick(), 32))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_POPULATION_CAP");
    if (!expect_int("default controller war population cap", runtime_tick_budget_policy_controller_war_population_cap(), 96))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_POPULATION_CAP", "256", 1);
    if (!expect_int("configured controller war population cap", runtime_tick_budget_policy_controller_war_population_cap(), 256))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_POPULATION_CAP", "2", 1);
    if (!expect_int("minimum controller war population cap clamp", runtime_tick_budget_policy_controller_war_population_cap(), 8))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_POPULATION_CAP", "99999", 1);
    if (!expect_int("maximum controller war population cap clamp", runtime_tick_budget_policy_controller_war_population_cap(), 2048))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT");
    if (!expect_int("default terrain pressure pct", runtime_tick_budget_policy_controller_war_terrain_pressure_pct(), 75))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "90", 1);
    if (!expect_int("configured terrain pressure pct", runtime_tick_budget_policy_controller_war_terrain_pressure_pct(), 90))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "1", 1);
    if (!expect_int("minimum terrain pressure pct clamp", runtime_tick_budget_policy_controller_war_terrain_pressure_pct(), 25))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "999", 1);
    if (!expect_int("maximum terrain pressure pct clamp", runtime_tick_budget_policy_controller_war_terrain_pressure_pct(), 100))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS");
    if (!expect_int("default terrain expand cooldown", runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks(), 3))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "5", 1);
    if (!expect_int("configured terrain expand cooldown", runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks(), 5))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "0", 1);
    if (!expect_int("minimum terrain expand cooldown clamp", runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks(), 1))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "999", 1);
    if (!expect_int("maximum terrain expand cooldown clamp", runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks(), 300))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS");
    if (!expect_int("default max frontier chunks", runtime_tick_budget_policy_controller_war_max_frontier_chunks(), 4))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "7", 1);
    if (!expect_int("configured max frontier chunks", runtime_tick_budget_policy_controller_war_max_frontier_chunks(), 7))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "0", 1);
    if (!expect_int("minimum max frontier chunks clamp", runtime_tick_budget_policy_controller_war_max_frontier_chunks(), 1))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "99", 1);
    if (!expect_int("maximum max frontier chunks clamp", runtime_tick_budget_policy_controller_war_max_frontier_chunks(), 32))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT");
    if (!expect_int("default war skirmish pct", runtime_tick_budget_policy_controller_war_skirmish_pct(), 50))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "10", 1);
    if (!expect_int("minimum war skirmish pct clamp", runtime_tick_budget_policy_controller_war_skirmish_pct(), 25))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "150", 1);
    if (!expect_int("maximum war skirmish pct clamp", runtime_tick_budget_policy_controller_war_skirmish_pct(), 95))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_PCT");
    if (!expect_int("default war siege pct", runtime_tick_budget_policy_controller_war_siege_pct(), 80))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "70", 1);
    setenv("BANANA_CONTROLLER_WAR_SIEGE_PCT", "50", 1);
    if (!expect_int("siege pct tracks skirmish floor", runtime_tick_budget_policy_controller_war_siege_pct(), 75))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SIEGE_PCT", "999", 1);
    if (!expect_int("siege pct ceiling", runtime_tick_budget_policy_controller_war_siege_pct(), 100))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS");
    if (!expect_int("default skirmish reinforcement bonus", runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus(), 1))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "-1", 1);
    if (!expect_int("minimum skirmish reinforcement bonus clamp", runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus(), 0))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "999", 1);
    if (!expect_int("maximum skirmish reinforcement bonus clamp", runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus(), 16))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS");
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "3", 1);
    if (!expect_int("default siege reinforcement bonus", runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(), 3))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS", "2", 1);
    if (!expect_int("siege reinforcement bonus floor", runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(), 3))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS", "999", 1);
    if (!expect_int("siege reinforcement bonus ceiling", runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(), 32))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION");
    if (!expect_int("default skirmish cooldown reduction", runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction(), 1))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "-4", 1);
    if (!expect_int("minimum skirmish cooldown reduction clamp", runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction(), 0))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "999", 1);
    if (!expect_int("maximum skirmish cooldown reduction clamp", runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction(), 16))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION");
    setenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "3", 1);
    if (!expect_int("default siege cooldown reduction", runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(), 3))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION", "2", 1);
    if (!expect_int("siege cooldown reduction floor", runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(), 3))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION", "999", 1);
    if (!expect_int("siege cooldown reduction ceiling", runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(), 32))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_PCT");
    if (!expect_int("default overcrowd pct", runtime_tick_budget_policy_controller_war_overcrowd_pct(), 92))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_PCT", "40", 1);
    if (!expect_int("minimum overcrowd pct clamp", runtime_tick_budget_policy_controller_war_overcrowd_pct(), 60))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_PCT", "140", 1);
    if (!expect_int("maximum overcrowd pct clamp", runtime_tick_budget_policy_controller_war_overcrowd_pct(), 100))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS");
    if (!expect_int("default overcrowd expand bonus", runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks(), 0))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS", "2", 1);
    if (!expect_int("configured overcrowd expand bonus", runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks(), 2))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS", "-1", 1);
    if (!expect_int("minimum overcrowd expand bonus clamp", runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks(), 0))
        return 1;

    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS", "999", 1);
    if (!expect_int("maximum overcrowd expand bonus clamp", runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks(), 4))
        return 1;

    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE");
    if (!expect_int("default overcrowd intelligence bonus per stage",
                    runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage(),
                    0))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE", "1", 1);
    if (!expect_int("configured overcrowd intelligence bonus per stage",
                    runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage(),
                    1))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE", "-1", 1);
    if (!expect_int("minimum overcrowd intelligence bonus per stage clamp",
                    runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage(),
                    0))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE", "999", 1);
    if (!expect_int("maximum overcrowd intelligence bonus per stage clamp",
                    runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage(),
                    2))
    {
        return 1;
    }

    unsetenv("BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL");
    if (!expect_int("default life tick interval",
                    runtime_tick_budget_policy_controller_war_life_tick_interval(),
                    1))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL", "0", 1);
    if (!expect_int("minimum life tick interval clamp",
                    runtime_tick_budget_policy_controller_war_life_tick_interval(),
                    1))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL", "999", 1);
    if (!expect_int("maximum life tick interval clamp",
                    runtime_tick_budget_policy_controller_war_life_tick_interval(),
                    32))
    {
        return 1;
    }

    unsetenv("BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX");
    if (!expect_int("default life intelligence bonus max",
                    runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max(),
                    2))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX", "-1", 1);
    if (!expect_int("minimum life intelligence bonus max clamp",
                    runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max(),
                    0))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX", "999", 1);
    if (!expect_int("maximum life intelligence bonus max clamp",
                    runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max(),
                    8))
    {
        return 1;
    }

    unsetenv("BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE");
    if (!expect_int("default procgen biome variance",
                    runtime_tick_budget_policy_controller_war_procgen_biome_variance(),
                    1))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE", "-1", 1);
    if (!expect_int("minimum procgen biome variance clamp",
                    runtime_tick_budget_policy_controller_war_procgen_biome_variance(),
                    0))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE", "999", 1);
    if (!expect_int("maximum procgen biome variance clamp",
                    runtime_tick_budget_policy_controller_war_procgen_biome_variance(),
                    3))
    {
        return 1;
    }

    unsetenv("BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK");
    if (!expect_int("default sentience gain per tick",
                    runtime_tick_budget_policy_controller_war_sentience_gain_per_tick(),
                    1))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK", "-1", 1);
    if (!expect_int("minimum sentience gain per tick clamp",
                    runtime_tick_budget_policy_controller_war_sentience_gain_per_tick(),
                    0))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK", "999", 1);
    if (!expect_int("maximum sentience gain per tick clamp",
                    runtime_tick_budget_policy_controller_war_sentience_gain_per_tick(),
                    8))
    {
        return 1;
    }

    unsetenv("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION");
    if (!expect_int("default sentience comeback bonus per coordination",
                    runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination(),
                    2))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION", "-1", 1);
    if (!expect_int("minimum sentience comeback bonus per coordination clamp",
                    runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination(),
                    0))
    {
        return 1;
    }

    setenv("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION", "999", 1);
    if (!expect_int("maximum sentience comeback bonus per coordination clamp",
                    runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination(),
                    8))
    {
        return 1;
    }

    unsetenv("BANANA_TERRAIN_REBUILD_BUDGET");
    unsetenv("BANANA_CONTROLLER_WAR_RADIUS");
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
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_PCT");
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS");
    unsetenv("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE");
    unsetenv("BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL");
    unsetenv("BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX");
    unsetenv("BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE");
    unsetenv("BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK");
    unsetenv("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION");
    return 0;
}
