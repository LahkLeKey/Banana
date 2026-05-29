#include "runtime_gameplay_test_factory.h"

#include <stdlib.h>

typedef int (*RuntimeWarScenarioFn)(void);

typedef struct RuntimeWarScenario
{
    const char *name;
    RuntimeWarScenarioFn run;
} RuntimeWarScenario;

static int run_floor_enabled_scenario(void)
{
    RuntimeGameplayTestFactoryContext context;
    ControllerInstance *controllers[128] = {0};
    int spawned_per_tick[3] = {0};
    int trim_per_tick[3] = {0};
    int result = 1;

    setenv("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_INTELLIGENCE_STAGE", "3", 1);
    setenv("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_REINFORCEMENTS", "1", 1);

    if (!runtime_gameplay_test_factory_init(&context,
                                            controllers,
                                            (int)(sizeof(controllers) / sizeof(controllers[0])),
                                            -12.0f,
                                            -12.0f))
    {
        fprintf(stderr, "floor-enabled scenario failed to initialize test factory\n");
        goto cleanup;
    }

    if (!runtime_gameplay_test_expect_true("floor-enabled banana anchor",
                                           runtime_gameplay_test_factory_spawn_team_controller(&context,
                                                                                               CONTROLLER_TEAM_BANANA,
                                                                                               "combat",
                                                                                               0.0f,
                                                                                               0.0f,
                                                                                               NULL)))
    {
        goto cleanup;
    }

    if (!runtime_gameplay_test_expect_true("floor-enabled bean anchor",
                                           runtime_gameplay_test_factory_spawn_team_controller(&context,
                                                                                               CONTROLLER_TEAM_BEAN,
                                                                                               "wildlife",
                                                                                               2.0f,
                                                                                               0.0f,
                                                                                               NULL)))
    {
        goto cleanup;
    }

    context.telemetry.war_sentience_humanoid_index = 14;
    context.telemetry.war_sentience_coordination_level = 8;
    context.telemetry.war_sentience_empathy_level = 8;

    for (int tick = 0; tick < 3; tick++)
    {
        int entities_before = context.world->entity_count;

        runtime_gameplay_test_factory_tick(&context,
                                           0.0f,
                                           1.0f,
                                           6.0f,
                                           2,
                                           128,
                                           1,
                                           4,
                                           0);

        spawned_per_tick[tick] = context.world->entity_count - entities_before;
        trim_per_tick[tick] = context.telemetry.war_sentience_negotiate_deescalation_trim_last_tick;

        if (!runtime_gameplay_test_expect_int("floor-enabled banana behavior negotiate",
                                              context.telemetry.war_sentience_behavior_banana,
                                              RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE))
        {
            goto cleanup;
        }

        if (!runtime_gameplay_test_expect_int("floor-enabled bean behavior negotiate",
                                              context.telemetry.war_sentience_behavior_bean,
                                              RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE))
        {
            goto cleanup;
        }

        if (!runtime_gameplay_test_expect_int("floor-enabled streak increments",
                                              context.telemetry.war_sentience_negotiate_streak_ticks,
                                              tick + 1))
        {
            goto cleanup;
        }

        if (!runtime_gameplay_test_expect_true("floor-enabled continuity floor keeps lane active",
                                               spawned_per_tick[tick] >= 1))
        {
            goto cleanup;
        }

        if (tick < 2)
        {
            if (!runtime_gameplay_test_expect_true("floor-enabled bean parity injection",
                                                   runtime_gameplay_test_factory_spawn_team_controller(&context,
                                                                                                       CONTROLLER_TEAM_BEAN,
                                                                                                       "wildlife",
                                                                                                       2.0f + (float)tick,
                                                                                                       0.25f,
                                                                                                       NULL)))
            {
                goto cleanup;
            }
        }
    }

    if (!runtime_gameplay_test_expect_int("floor-enabled trim tick1", trim_per_tick[0], 1))
        goto cleanup;
    if (!runtime_gameplay_test_expect_int("floor-enabled trim tick2", trim_per_tick[1], 1))
        goto cleanup;
    if (!runtime_gameplay_test_expect_int("floor-enabled trim tick3", trim_per_tick[2], 1))
        goto cleanup;
    if (!runtime_gameplay_test_expect_true("floor-enabled third tick still spawns", spawned_per_tick[2] >= 1))
        goto cleanup;

    result = 0;

cleanup:
    runtime_gameplay_test_factory_destroy(&context);
    unsetenv("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_INTELLIGENCE_STAGE");
    unsetenv("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_REINFORCEMENTS");
    return result;
}

static int run_threshold_disabled_scenario(void)
{
    RuntimeGameplayTestFactoryContext context;
    ControllerInstance *controllers[128] = {0};
    int spawned_per_tick[3] = {0};
    int trim_per_tick[3] = {0};
    int result = 1;

    setenv("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_INTELLIGENCE_STAGE", "5", 1);
    setenv("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_REINFORCEMENTS", "1", 1);

    if (!runtime_gameplay_test_factory_init(&context,
                                            controllers,
                                            (int)(sizeof(controllers) / sizeof(controllers[0])),
                                            -12.0f,
                                            -12.0f))
    {
        fprintf(stderr, "threshold-disabled scenario failed to initialize test factory\n");
        goto cleanup;
    }

    if (!runtime_gameplay_test_expect_true("threshold-disabled banana anchor",
                                           runtime_gameplay_test_factory_spawn_team_controller(&context,
                                                                                               CONTROLLER_TEAM_BANANA,
                                                                                               "combat",
                                                                                               0.0f,
                                                                                               0.0f,
                                                                                               NULL)))
    {
        goto cleanup;
    }

    if (!runtime_gameplay_test_expect_true("threshold-disabled bean anchor",
                                           runtime_gameplay_test_factory_spawn_team_controller(&context,
                                                                                               CONTROLLER_TEAM_BEAN,
                                                                                               "wildlife",
                                                                                               2.0f,
                                                                                               0.0f,
                                                                                               NULL)))
    {
        goto cleanup;
    }

    context.telemetry.war_sentience_humanoid_index = 14;
    context.telemetry.war_sentience_coordination_level = 8;
    context.telemetry.war_sentience_empathy_level = 8;

    for (int tick = 0; tick < 3; tick++)
    {
        int entities_before = context.world->entity_count;

        runtime_gameplay_test_factory_tick(&context,
                                           0.0f,
                                           1.0f,
                                           6.0f,
                                           2,
                                           128,
                                           1,
                                           4,
                                           0);

        spawned_per_tick[tick] = context.world->entity_count - entities_before;
        trim_per_tick[tick] = context.telemetry.war_sentience_negotiate_deescalation_trim_last_tick;

        if (!runtime_gameplay_test_expect_int("threshold-disabled banana behavior negotiate",
                                              context.telemetry.war_sentience_behavior_banana,
                                              RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE))
        {
            goto cleanup;
        }

        if (!runtime_gameplay_test_expect_int("threshold-disabled bean behavior negotiate",
                                              context.telemetry.war_sentience_behavior_bean,
                                              RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE))
        {
            goto cleanup;
        }

        if (!runtime_gameplay_test_expect_int("threshold-disabled streak increments",
                                              context.telemetry.war_sentience_negotiate_streak_ticks,
                                              tick + 1))
        {
            goto cleanup;
        }

        if (tick < 2)
        {
            if (!runtime_gameplay_test_expect_true("threshold-disabled bean parity injection",
                                                   runtime_gameplay_test_factory_spawn_team_controller(&context,
                                                                                                       CONTROLLER_TEAM_BEAN,
                                                                                                       "wildlife",
                                                                                                       2.0f + (float)tick,
                                                                                                       0.25f,
                                                                                                       NULL)))
            {
                goto cleanup;
            }
        }
    }

    if (!runtime_gameplay_test_expect_int("threshold-disabled trim tick1", trim_per_tick[0], 1))
        goto cleanup;
    if (!runtime_gameplay_test_expect_int("threshold-disabled trim tick2", trim_per_tick[1], 1))
        goto cleanup;
    if (!runtime_gameplay_test_expect_int("threshold-disabled trim tick3", trim_per_tick[2], 2))
        goto cleanup;

    if (!runtime_gameplay_test_expect_int("threshold-disabled third tick can deescalate to zero",
                                          spawned_per_tick[2],
                                          0))
    {
        goto cleanup;
    }

    result = 0;

cleanup:
    runtime_gameplay_test_factory_destroy(&context);
    unsetenv("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_INTELLIGENCE_STAGE");
    unsetenv("BANANA_CONTROLLER_WAR_NEGOTIATE_MIN_REINFORCEMENTS");
    return result;
}

int main(void)
{
    const RuntimeWarScenario scenarios[] = {
        {"floor-enabled", run_floor_enabled_scenario},
        {"threshold-disabled", run_threshold_disabled_scenario},
    };

    for (int i = 0; i < (int)(sizeof(scenarios) / sizeof(scenarios[0])); i++)
    {
        if (scenarios[i].run() != 0)
        {
            fprintf(stderr, "negotiate factory scenario failed: %s\n", scenarios[i].name);
            return 1;
        }
    }

    return 0;
}
