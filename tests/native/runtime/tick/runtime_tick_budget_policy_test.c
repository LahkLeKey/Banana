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
    return 0;
}
