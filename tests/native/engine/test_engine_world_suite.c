#include "test_engine_domain_suite_framework.h"

#include "../../../src/native/engine/engine.h"
#include "../../../src/native/engine/world/entity.h"
#include "../../../src/native/engine/world/signals.h"
#include "../../../src/native/engine/world/world.h"

static void test_world_spawn_entity(DomainSuiteStats *stats)
{
    World *w = NULL;
    EntityId id = 0;
    Entity *e = NULL;

    SUITE_TEST("world_spawn_entity: returns non-zero EntityId");
    w = world_create();
    SUITE_ASSERT_TRUE(stats, w != NULL);
    id = world_spawn_entity(w, ENTITY_TYPE_NPC, 1.f, 0.f, 1.f);
    SUITE_ASSERT_TRUE(stats, id != 0);
    e = world_get_entity(w, id);
    SUITE_ASSERT_TRUE(stats, e != NULL);
    SUITE_ASSERT_NEAR(stats, e->position[0], 1.f, 0.001f);
    SUITE_PASS(stats);
done:
    if (w)
        world_destroy(w);
}

static void test_world_despawn_entity(DomainSuiteStats *stats)
{
    World *w = NULL;
    EntityId id = 0;
    Entity *e = NULL;

    SUITE_TEST("world_despawn_entity: entity no longer retrievable");
    w = world_create();
    SUITE_ASSERT_TRUE(stats, w != NULL);
    id = world_spawn_entity(w, ENTITY_TYPE_STATIC, 0, 0, 0);
    world_despawn_entity(w, id);
    e = world_get_entity(w, id);
    SUITE_ASSERT_TRUE(stats, e == NULL);
    SUITE_PASS(stats);
done:
    if (w)
        world_destroy(w);
}

static void test_world_query_nearby(DomainSuiteStats *stats)
{
    World *w = NULL;
    EntityId id1 = 0;
    EntityId found[8];
    float center[3] = {0, 0, 0};
    int count = 0;

    SUITE_TEST("world_query_nearby: finds entities within radius");
    w = world_create();
    SUITE_ASSERT_TRUE(stats, w != NULL);
    id1 = world_spawn_entity(w, ENTITY_TYPE_NPC, 1.f, 0, 1.f);
    (void)world_spawn_entity(w, ENTITY_TYPE_NPC, 100.f, 0, 100.f);

    count = world_query_nearby(w, center, 5.f, found, 8);
    SUITE_ASSERT_INT_EQ(stats, count, 1);
    SUITE_ASSERT_INT_EQ(stats, (int)found[0], (int)id1);
    SUITE_PASS(stats);
done:
    if (w)
        world_destroy(w);
}

static void test_signals_send_and_flush(DomainSuiteStats *stats)
{
    World *w = NULL;
    SignalQueue *q = NULL;
    int before = 0;

    SUITE_TEST("signals_send: queue count increases and flush clears");
    w = world_create();
    SUITE_ASSERT_TRUE(stats, w != NULL);
    (void)world_spawn_entity(w, ENTITY_TYPE_NPC, 0, 0, 0);
    q = signals_get_queue();
    SUITE_ASSERT_TRUE(stats, q != NULL);

    before = q->count;
    signals_send(1, "player_nearby", NULL);
    SUITE_ASSERT_INT_EQ(stats, q->count, before + 1);
    signals_flush(w);
    SUITE_ASSERT_INT_EQ(stats, q->count, 0);
    SUITE_PASS(stats);
done:
    if (w)
        world_destroy(w);
}

static void test_engine_scene_spawns_non_player_actors(DomainSuiteStats *stats)
{
    int init_result = 0;
    int entity_count = 0;
    int off_center_actors = 0;

    SUITE_TEST("engine_init: scene spawns off-center non-player actors");
    init_result = engine_init(800, 600);
    SUITE_ASSERT_INT_EQ(stats, init_result, 0);

    entity_count = engine_get_entity_count();
    SUITE_ASSERT_TRUE(stats, entity_count >= 5);

    for (int i = 0; i < entity_count; i++)
    {
        float x = engine_get_entity_x(i);
        float z = engine_get_entity_z(i);
        if (fabsf(x) >= 2.0f || fabsf(z) >= 2.0f)
            off_center_actors++;
    }

    SUITE_ASSERT_TRUE(stats, off_center_actors >= 2);
    SUITE_PASS(stats);
done:
    engine_shutdown();
}

int run_engine_world_suite(void)
{
    DomainSuiteStats stats = {0, 0};

    printf("-- World + Signals Suite --\n");
    test_world_spawn_entity(&stats);
    test_world_despawn_entity(&stats);
    test_world_query_nearby(&stats);
    test_signals_send_and_flush(&stats);
    test_engine_scene_spawns_non_player_actors(&stats);

    printf("World+Signals: %d passed, %d failed\n\n", stats.pass, stats.fail);
    return stats.fail;
}
