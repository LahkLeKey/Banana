#include "runtime/controller/sync/controller_sync.h"

#include "runtime/support/test_support.h"

#include <string.h>

/* Stubs required by controller_sync.c and interlock.c */

static int g_update_calls = 0;
static int g_signal_calls = 0;

void controller_update(ControllerInstance *c, float dt)
{
    (void)dt;
    if (c)
        g_update_calls += 1;
}

void controller_signal(ControllerInstance *c, const char *signal, const void *data)
{
    (void)signal;
    (void)data;
    if (c)
        g_signal_calls += 1;
}

static ControllerInstance make_controller(uint32_t id, EntityId entity_id)
{
    ControllerInstance c;
    memset(&c, 0, sizeof(c));
    c.id = id;
    c.position[0] = 1.0f;
    c.position[1] = 0.0f;
    c.position[2] = 2.0f;
    return c;
}

static void test_sync_null_guards(void **state)
{
    (void)state;
    ControllerInstance *controllers[1] = {NULL};
    World world = {0};

    /* null world */
    runtime_sync_controller_positions(NULL, controllers, 1, 0.016f);

    /* null controllers */
    runtime_sync_controller_positions(&world, NULL, 1, 0.016f);

    /* zero count */
    runtime_sync_controller_positions(&world, controllers, 0, 0.016f);
}

static void test_sync_updates_positions(void **state)
{
    (void)state;
    ControllerInstance c0 = make_controller(1, 100);
    ControllerInstance c1 = make_controller(2, 101);
    ControllerInstance *controllers[2] = {&c0, &c1};
    Entity entity0 = { .id = 100, .active = 1,
                       .position = {5.0f, 0.0f, 7.0f}, .controller_id = 1 };
    Entity entity1 = { .id = 101, .active = 1,
                       .position = {3.0f, 0.0f, 4.0f}, .controller_id = 2 };
    World world = {0};
    world.entities[0] = &entity0;
    world.entities[1] = &entity1;
    world.entity_count = 2;

    g_update_calls = 0;

    runtime_sync_controller_positions(&world, controllers, 2, 0.016f);

    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_sync_parallel_available(),
                              1,
                              "parallel sync must report as available");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_sync_null_guards),
    BANANA_TEST_CASE(test_sync_updates_positions)
)
