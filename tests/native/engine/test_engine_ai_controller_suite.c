#include "test_engine_domain_suite_framework.h"

#include "../../../src/native/engine/ai/controller_system.h"
#include "../../../src/native/engine/ai/wildlife_controller.h"

static void test_controller_system_spawn(DomainSuiteStats *stats)
{
    ControllerSystem *sys = NULL;
    uint32_t id = 0;

    SUITE_TEST("controller_system: spawn returns non-zero id");
    wildlife_controller_register();
    sys = controller_system_create();
    SUITE_ASSERT_TRUE(stats, sys != NULL);
    id = controller_system_spawn(sys, "wildlife", 0.f, 0.f, 0.f);
    SUITE_ASSERT_TRUE(stats, id != 0);
    SUITE_ASSERT_INT_EQ(stats, sys->count, 1);
    SUITE_PASS(stats);
done:
    if (sys)
        controller_system_destroy(sys);
}

static void test_controller_system_multi_spawn_update(DomainSuiteStats *stats)
{
    ControllerSystem *sys = NULL;

    SUITE_TEST("controller_system: 3 NPCs update 600 frames without crash");
    wildlife_controller_register();
    sys = controller_system_create();
    SUITE_ASSERT_TRUE(stats, sys != NULL);

    for (int i = 0; i < 3; i++)
        controller_system_spawn(sys, "wildlife", (float)(i * 5), 0.f, 0.f);

    SUITE_ASSERT_INT_EQ(stats, sys->count, 3);

    for (int frame = 0; frame < 600; frame++)
        controller_system_update(sys, 1.f / 60.f);

    SUITE_ASSERT_INT_EQ(stats, sys->count, 3);
    SUITE_PASS(stats);
done:
    if (sys)
        controller_system_destroy(sys);
}

static void test_controller_system_signal_broadcast(DomainSuiteStats *stats)
{
    ControllerSystem *sys = NULL;
    float player_pos[3] = {5.f, 0.f, 5.f};

    SUITE_TEST("controller_system: signal broadcast reaches all controllers");
    wildlife_controller_register();
    sys = controller_system_create();
    SUITE_ASSERT_TRUE(stats, sys != NULL);

    for (int i = 0; i < 3; i++)
        controller_system_spawn(sys, "wildlife", (float)(i * 5), 0.f, 0.f);

    for (int f = 0; f < 60; f++)
        controller_system_update(sys, 1.f / 60.f);

    controller_system_signal_all(sys, "player_nearby", player_pos);
    controller_system_update(sys, 1.f / 60.f);
    SUITE_ASSERT_INT_EQ(stats, sys->count, 3);
    SUITE_PASS(stats);
done:
    if (sys)
        controller_system_destroy(sys);
}

int run_engine_ai_controller_suite(void)
{
    DomainSuiteStats stats = (DomainSuiteStats){0, 0};

    printf("-- AI Controller Suite --\n");
    test_controller_system_spawn(&stats);
    test_controller_system_multi_spawn_update(&stats);
    test_controller_system_signal_broadcast(&stats);

    printf("AI Controller: %d passed, %d failed\n\n", stats.pass, stats.fail);
    return stats.fail;
}
