#include "runtime/tick/tick_budget_policy.h"
#include "../support/test_support.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const g_policy_env_names[] = {
    "BANANA_TERRAIN_REBUILD_BUDGET",
    "BANANA_CONTROLLER_WAR_RADIUS",
    "BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK",
    "BANANA_CONTROLLER_WAR_POPULATION_CAP",
    "BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT",
    "BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS",
    "BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS",
    "BANANA_CONTROLLER_WAR_SKIRMISH_PCT",
    "BANANA_CONTROLLER_WAR_SIEGE_PCT",
    "BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS",
    "BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS",
    "BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION",
    "BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION",
    "BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH",
    "BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE",
    "BANANA_CONTROLLER_WAR_INTELLIGENCE_LEVEL_THRESHOLD_TICKS",
    "BANANA_CONTROLLER_WAR_INTELLIGENCE_MAX_STAGE",
    "BANANA_CONTROLLER_WAR_INTELLIGENCE_REINFORCEMENT_BONUS_PER_STAGE",
    "BANANA_CONTROLLER_WAR_INTELLIGENCE_FRONTIER_BONUS_PER_STAGE",
    "BANANA_CONTROLLER_WAR_OVERCROWD_PCT",
    "BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS",
    "BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE",
    "BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL",
    "BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX",
    "BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE",
    "BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK",
    "BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION",
    "BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_INTELLIGENCE_STAGE",
    "BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_REINFORCEMENTS",
};

static int policy_set_env(const char *name, const char *value)
{
#if defined(_WIN32)
    return _putenv_s(name, value) == 0;
#else
    return setenv(name, value, 1) == 0;
#endif
}

static int policy_clear_env(const char *name)
{
#if defined(_WIN32)
    return _putenv_s(name, "") == 0;
#else
    return unsetenv(name) == 0;
#endif
}

static void policy_reset_envs(void)
{
    size_t index;

    for (index = 0; index < sizeof(g_policy_env_names) / sizeof(g_policy_env_names[0]); ++index)
        BANANA_TEST_ASSERT_TRUE(policy_clear_env(g_policy_env_names[index]), "policy env reset must succeed");
}

static int policy_test_setup(void **state)
{
    (void)state;
    policy_reset_envs();
    return 0;
}

static int policy_test_teardown(void **state)
{
    (void)state;
    policy_reset_envs();
    return 0;
}

static void expect_policy_int_with_env(const char *name,
                                       const char *value,
                                       int (*fn)(void),
                                       int expected,
                                       const char *message)
{
    BANANA_TEST_ASSERT_TRUE(policy_set_env(name, value), "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(fn(), expected, message);
}

static void expect_policy_float_with_env(const char *name,
                                         const char *value,
                                         float (*fn)(void),
                                         float expected,
                                         const char *message)
{
    BANANA_TEST_ASSERT_TRUE(policy_set_env(name, value), "policy env set must succeed");
    BANANA_TEST_ASSERT_FLOAT_EQ(fn(), expected, 0.0001f, message);
}

static void test_tick_budget_policy_defaults(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_terrain_chunks_per_tick(), 2,
                              "terrain chunks per tick default must be 2");
    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_tick_budget_policy_controller_war_radius(), 8.0f, 0.0001f,
                                "war radius default must be 8.0");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_reinforcements_per_tick(), 2,
                              "reinforcements per tick default must be 2");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_population_cap(), 96,
                              "population cap default must be 96");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_terrain_pressure_pct(), 75,
                              "terrain pressure default must be 75");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks(), 3,
                              "terrain expand cooldown default must be 3");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_max_frontier_chunks(), 4,
                              "max frontier chunks default must be 4");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_skirmish_pct(), 50,
                              "skirmish pct default must be 50");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_pct(), 80,
                              "siege pct default must be 80");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus(), 1,
                              "skirmish reinforcement bonus default must be 1");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(), 2,
                              "siege reinforcement bonus default must be 2");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction(), 1,
                              "skirmish cooldown reduction default must be 1");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(), 2,
                              "siege cooldown reduction default must be 2");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_progress_skirmish(), 1,
                              "skirmish intelligence progress default must be 1");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_progress_siege(), 2,
                              "siege intelligence progress default must be 2");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_level_threshold_ticks(), 12,
                              "intelligence threshold default must be 12");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_max_stage(), 3,
                              "intelligence max stage default must be 3");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_reinforcement_bonus_per_stage(), 1,
                              "intelligence reinforcement bonus default must be 1");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_frontier_bonus_per_stage(), 1,
                              "intelligence frontier bonus default must be 1");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_overcrowd_pct(), 92,
                              "overcrowd pct default must be 92");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks(), 0,
                              "overcrowd expand bonus default must be 0");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage(), 0,
                              "overcrowd intelligence bonus default must be 0");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_life_tick_interval(), 1,
                              "life tick interval default must be 1");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max(), 2,
                              "life intelligence bonus max default must be 2");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_procgen_biome_variance(), 1,
                              "biome variance default must be 1");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_sentience_gain_per_tick(), 1,
                              "sentience gain default must be 1");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination(), 2,
                              "sentience comeback bonus default must be 2");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_negotiate_min_intelligence_stage(), 3,
                              "negotiate min intelligence stage default must be 3");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_negotiate_min_reinforcements(), 1,
                              "negotiate min reinforcements default must be 1");
}

static void test_tick_budget_policy_lower_bounds(void **state)
{
    (void)state;

    expect_policy_int_with_env("BANANA_TERRAIN_REBUILD_BUDGET", "0", runtime_tick_budget_policy_terrain_chunks_per_tick, 1,
                               "terrain chunks per tick must clamp to 1");
    expect_policy_float_with_env("BANANA_CONTROLLER_WAR_RADIUS", "0.1", runtime_tick_budget_policy_controller_war_radius, 1.0f,
                                 "war radius must clamp to 1.0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "-9",
                               runtime_tick_budget_policy_controller_war_reinforcements_per_tick, 0,
                               "reinforcements per tick must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_POPULATION_CAP", "0", runtime_tick_budget_policy_controller_war_population_cap,
                               8, "population cap must clamp to 8");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "1",
                               runtime_tick_budget_policy_controller_war_terrain_pressure_pct, 25,
                               "terrain pressure must clamp to 25");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "0",
                               runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks, 1,
                               "terrain expand cooldown must clamp to 1");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "0", runtime_tick_budget_policy_controller_war_max_frontier_chunks,
                               1, "max frontier chunks must clamp to 1");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "0", runtime_tick_budget_policy_controller_war_skirmish_pct,
                               25, "skirmish pct must clamp to 25");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "0"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SIEGE_PCT", "0"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_pct(), 30,
                              "siege pct must respect the skirmish floor");

    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "-1",
                               runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus, 0,
                               "skirmish reinforcement bonus must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS", "-1",
                               runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus, 0,
                               "siege reinforcement bonus must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "-1",
                               runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction, 0,
                               "skirmish cooldown reduction must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION", "-1",
                               runtime_tick_budget_policy_controller_war_siege_cooldown_reduction, 0,
                               "siege cooldown reduction must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH", "-1",
                               runtime_tick_budget_policy_controller_war_intelligence_progress_skirmish, 0,
                               "skirmish intelligence progress must clamp to 0");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH", "0"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE", "-1"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_progress_siege(), 0,
                              "siege intelligence progress must clamp to the skirmish floor");

    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_LEVEL_THRESHOLD_TICKS", "0",
                               runtime_tick_budget_policy_controller_war_intelligence_level_threshold_ticks, 1,
                               "intelligence threshold must clamp to 1");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_MAX_STAGE", "-1",
                               runtime_tick_budget_policy_controller_war_intelligence_max_stage, 0,
                               "intelligence max stage must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_REINFORCEMENT_BONUS_PER_STAGE", "-1",
                               runtime_tick_budget_policy_controller_war_intelligence_reinforcement_bonus_per_stage, 0,
                               "intelligence reinforcement bonus must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_FRONTIER_BONUS_PER_STAGE", "-1",
                               runtime_tick_budget_policy_controller_war_intelligence_frontier_bonus_per_stage, 0,
                               "intelligence frontier bonus must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_OVERCROWD_PCT", "0", runtime_tick_budget_policy_controller_war_overcrowd_pct,
                               60, "overcrowd pct must clamp to 60");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS", "-1",
                               runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks, 0,
                               "overcrowd expand bonus chunks must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE", "-1",
                               runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage, 0,
                               "overcrowd intelligence bonus must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL", "0", runtime_tick_budget_policy_controller_war_life_tick_interval,
                               1, "life tick interval must clamp to 1");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX", "-1",
                               runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max, 0,
                               "life intelligence bonus max must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE", "-1",
                               runtime_tick_budget_policy_controller_war_procgen_biome_variance, 0,
                               "biome variance must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK", "-1",
                               runtime_tick_budget_policy_controller_war_sentience_gain_per_tick, 0,
                               "sentience gain must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION", "-1",
                               runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination, 0,
                               "sentience comeback bonus must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_INTELLIGENCE_STAGE", "-1",
                               runtime_tick_budget_policy_controller_war_negotiate_min_intelligence_stage, 0,
                               "negotiate min intelligence stage must clamp to 0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_REINFORCEMENTS", "-1",
                               runtime_tick_budget_policy_controller_war_negotiate_min_reinforcements, 0,
                               "negotiate min reinforcements must clamp to 0");
}

static void test_tick_budget_policy_upper_bounds(void **state)
{
    (void)state;

    expect_policy_int_with_env("BANANA_TERRAIN_REBUILD_BUDGET", "999", runtime_tick_budget_policy_terrain_chunks_per_tick, 128,
                               "terrain chunks per tick must clamp to 128");
    expect_policy_float_with_env("BANANA_CONTROLLER_WAR_RADIUS", "999.0", runtime_tick_budget_policy_controller_war_radius, 256.0f,
                                 "war radius must clamp to 256.0");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK", "999",
                               runtime_tick_budget_policy_controller_war_reinforcements_per_tick, 32,
                               "reinforcements per tick must clamp to 32");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_POPULATION_CAP", "9999", runtime_tick_budget_policy_controller_war_population_cap,
                               2048, "population cap must clamp to 2048");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT", "999",
                               runtime_tick_budget_policy_controller_war_terrain_pressure_pct, 100,
                               "terrain pressure must clamp to 100");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS", "999",
                               runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks, 300,
                               "terrain expand cooldown must clamp to 300");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS", "999", runtime_tick_budget_policy_controller_war_max_frontier_chunks,
                               32, "max frontier chunks must clamp to 32");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "999", runtime_tick_budget_policy_controller_war_skirmish_pct,
                               95, "skirmish pct must clamp to 95");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "95"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SIEGE_PCT", "999"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_pct(), 100,
                              "siege pct must clamp to 100");

    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "999",
                               runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus, 16,
                               "skirmish reinforcement bonus must clamp to 16");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "16"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS", "999"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(), 32,
                              "siege reinforcement bonus must clamp to 32");

    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "999",
                               runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction, 16,
                               "skirmish cooldown reduction must clamp to 16");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "16"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION", "999"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(), 32,
                              "siege cooldown reduction must clamp to 32");

    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH", "999",
                               runtime_tick_budget_policy_controller_war_intelligence_progress_skirmish, 8,
                               "skirmish intelligence progress must clamp to 8");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH", "8"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE", "999"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_progress_siege(), 12,
                              "siege intelligence progress must clamp to 12");

    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_LEVEL_THRESHOLD_TICKS", "999",
                               runtime_tick_budget_policy_controller_war_intelligence_level_threshold_ticks, 600,
                               "intelligence threshold must clamp to 600");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_MAX_STAGE", "999",
                               runtime_tick_budget_policy_controller_war_intelligence_max_stage, 8,
                               "intelligence max stage must clamp to 8");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_REINFORCEMENT_BONUS_PER_STAGE", "999",
                               runtime_tick_budget_policy_controller_war_intelligence_reinforcement_bonus_per_stage, 4,
                               "intelligence reinforcement bonus must clamp to 4");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_FRONTIER_BONUS_PER_STAGE", "999",
                               runtime_tick_budget_policy_controller_war_intelligence_frontier_bonus_per_stage, 4,
                               "intelligence frontier bonus must clamp to 4");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_OVERCROWD_PCT", "999", runtime_tick_budget_policy_controller_war_overcrowd_pct,
                               100, "overcrowd pct must clamp to 100");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_OVERCROWD_EXPAND_BONUS_CHUNKS", "999",
                               runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks, 4,
                               "overcrowd expand bonus chunks must clamp to 4");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE", "999",
                               runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage, 2,
                               "overcrowd intelligence bonus must clamp to 2");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_LIFE_TICK_INTERVAL", "999",
                               runtime_tick_budget_policy_controller_war_life_tick_interval, 32,
                               "life tick interval must clamp to 32");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_LIFE_INTELLIGENCE_BONUS_MAX", "999",
                               runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max, 8,
                               "life intelligence bonus max must clamp to 8");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_PROCGEN_BIOME_VARIANCE", "999",
                               runtime_tick_budget_policy_controller_war_procgen_biome_variance, 3,
                               "biome variance must clamp to 3");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SENTIENCE_GAIN_PER_TICK", "999",
                               runtime_tick_budget_policy_controller_war_sentience_gain_per_tick, 8,
                               "sentience gain must clamp to 8");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION", "999",
                               runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination, 8,
                               "sentience comeback bonus must clamp to 8");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_INTELLIGENCE_STAGE", "999",
                               runtime_tick_budget_policy_controller_war_negotiate_min_intelligence_stage, 8,
                               "negotiate min intelligence stage must clamp to 8");
    expect_policy_int_with_env("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_REINFORCEMENTS", "999",
                               runtime_tick_budget_policy_controller_war_negotiate_min_reinforcements, 8,
                               "negotiate min reinforcements must clamp to 8");
}

static void test_tick_budget_policy_dependency_floors(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SKIRMISH_PCT", "61"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SIEGE_PCT", "62"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_pct(), 66,
                              "siege pct must stay at least five points above skirmish pct");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS", "4"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS", "3"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(), 4,
                              "siege reinforcement bonus must not fall below skirmish bonus");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION", "4"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION", "3"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(), 4,
                              "siege cooldown reduction must not fall below skirmish reduction");

    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SKIRMISH", "6"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_TRUE(policy_set_env("BANANA_CONTROLLER_WAR_INTELLIGENCE_PROGRESS_SIEGE", "5"),
                            "policy env set must succeed");
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_budget_policy_controller_war_intelligence_progress_siege(), 6,
                              "siege intelligence progress must not fall below skirmish progress");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_tick_budget_policy_defaults, policy_test_setup, policy_test_teardown),
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_tick_budget_policy_lower_bounds, policy_test_setup, policy_test_teardown),
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_tick_budget_policy_upper_bounds, policy_test_setup, policy_test_teardown),
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_tick_budget_policy_dependency_floors, policy_test_setup, policy_test_teardown)
)