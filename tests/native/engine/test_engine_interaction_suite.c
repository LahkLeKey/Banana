#include "test_engine_domain_suite_framework.h"

#include "../../../src/native/engine/engine.h"

static void test_interaction_before_init_is_inert(DomainSuiteStats *stats)
{
    int result = 0;

    SUITE_TEST("interaction bridge before init: returns no-target");
    result = engine_handle_right_click_normalized(0.5f, 0.5f);
    SUITE_ASSERT_INT_EQ(stats, result, 0);
    SUITE_PASS(stats);
done:
    return;
}

static void test_interaction_after_init_targets_actor(DomainSuiteStats *stats)
{
    int init_result = 0;
    int click_result = 0;
    int entities = 0;

    SUITE_TEST("interaction bridge after init: targets spawned actor");
    init_result = engine_init(800, 600);
    SUITE_ASSERT_INT_EQ(stats, init_result, 0);

    entities = engine_get_entity_count();
    SUITE_ASSERT_TRUE(stats, entities >= 5);

    click_result = engine_handle_right_click_normalized(0.5f, 0.5f);
    SUITE_ASSERT_INT_EQ(stats, click_result, 1);
    SUITE_PASS(stats);
done:
    engine_shutdown();
}

int run_engine_interaction_suite(void)
{
    DomainSuiteStats stats = {0, 0};

    printf("-- Interaction Suite --\n");
    test_interaction_before_init_is_inert(&stats);
    test_interaction_after_init_targets_actor(&stats);

    printf("Interaction: %d passed, %d failed\n\n", stats.pass, stats.fail);
    return stats.fail;
}
