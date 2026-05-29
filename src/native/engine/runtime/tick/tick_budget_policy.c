#include "tick_budget_policy.h"

#include <stdlib.h>

int runtime_tick_budget_policy_terrain_chunks_per_tick(void)
{
    const char *raw = getenv("BANANA_TERRAIN_REBUILD_BUDGET");
    long value = 2;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 1)
        return 1;
    if (value > 128)
        return 128;

    return (int)value;
}

float runtime_tick_budget_policy_controller_war_radius(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_RADIUS");
    float value = 8.0f;

    if (raw && raw[0])
        value = (float)strtod(raw, NULL);

    if (value < 1.0f)
        return 1.0f;
    if (value > 256.0f)
        return 256.0f;

    return value;
}

int runtime_tick_budget_policy_controller_war_reinforcements_per_tick(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK");
    long value = 2;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 0)
        return 0;
    if (value > 32)
        return 32;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_population_cap(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_POPULATION_CAP");
    long value = 96;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 8)
        return 8;
    if (value > 2048)
        return 2048;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_terrain_pressure_pct(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_TERRAIN_PRESSURE_PCT");
    long value = 75;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 25)
        return 25;
    if (value > 100)
        return 100;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_TERRAIN_EXPAND_COOLDOWN_TICKS");
    long value = 3;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 1)
        return 1;
    if (value > 300)
        return 300;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_max_frontier_chunks(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_MAX_FRONTIER_CHUNKS");
    long value = 4;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 1)
        return 1;
    if (value > 32)
        return 32;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_skirmish_pct(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_SKIRMISH_PCT");
    long value = 50;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 25)
        return 25;
    if (value > 95)
        return 95;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_siege_pct(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_SIEGE_PCT");
    long value = 80;
    int skirmish_pct = runtime_tick_budget_policy_controller_war_skirmish_pct();

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < (long)(skirmish_pct + 5))
        value = (long)(skirmish_pct + 5);
    if (value > 100)
        value = 100;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_SKIRMISH_REINFORCEMENT_BONUS");
    long value = 1;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 0)
        return 0;
    if (value > 16)
        return 16;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_SIEGE_REINFORCEMENT_BONUS");
    long value = 2;
    int skirmish_bonus = runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus();

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < skirmish_bonus)
        value = skirmish_bonus;
    if (value > 32)
        value = 32;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_SKIRMISH_COOLDOWN_REDUCTION");
    long value = 1;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 0)
        return 0;
    if (value > 16)
        return 16;

    return (int)value;
}

int runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(void)
{
    const char *raw = getenv("BANANA_CONTROLLER_WAR_SIEGE_COOLDOWN_REDUCTION");
    long value = 2;
    int skirmish_reduction = runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction();

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < skirmish_reduction)
        value = skirmish_reduction;
    if (value > 32)
        value = 32;

    return (int)value;
}
