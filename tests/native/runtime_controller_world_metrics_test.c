#include "ai/controller.h"
#include "runtime/controller_runtime.h"
#include "runtime/world/world_metrics.h"
#include "world/world.h"

#include <stdio.h>
#include <string.h>

static int s_update_calls = 0;
static int s_signal_calls = 0;

static void test_update(ControllerInstance *self, float dt)
{
    (void)self;
    (void)dt;
    s_update_calls += 1;
}

static void test_signal(ControllerInstance *self, const char *signal, const void *data)
{
    (void)self;
    (void)signal;
    (void)data;
    s_signal_calls += 1;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    ControllerInstance controller_a;
    ControllerInstance controller_b;
    ControllerInstance *controllers[2] = {&controller_a, &controller_b};
    World *world = NULL;

    memset(&controller_a, 0, sizeof(controller_a));
    memset(&controller_b, 0, sizeof(controller_b));

    controller_a.id = 10;
    controller_a.update = test_update;
    controller_a.on_signal = test_signal;
    controller_b.id = 20;
    controller_b.update = test_update;
    controller_b.on_signal = test_signal;

    if (!expect_int("find by id", runtime_controller_find_by_id(controllers, 2, 20) == &controller_b ? 1 : 0, 1))
        return 1;
    if (!expect_int("update by id", runtime_controller_update_by_id(controllers, 2, 10, 1.0f / 60.0f), 1))
        return 1;
    if (!expect_int("signal by id", runtime_controller_signal_by_id(controllers, 2, 20, "ping", NULL), 1))
        return 1;
    if (!expect_int("update count", s_update_calls, 1))
        return 1;
    if (!expect_int("signal count", s_signal_calls, 1))
        return 1;
    if (!expect_int("missing update", runtime_controller_update_by_id(controllers, 2, 99, 0.1f), 0))
        return 1;

    world = world_create();
    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    {
        EntityId player_a = world_spawn_entity(world, ENTITY_TYPE_PLAYER, 0.f, 0.f, 0.f);
        EntityId player_b = world_spawn_entity(world, ENTITY_TYPE_PLAYER, 1.f, 0.f, 0.f);
        EntityId npc = world_spawn_entity(world, ENTITY_TYPE_NPC, 2.f, 0.f, 0.f);
        Entity *player_b_entity = world_get_entity(world, player_b);
        Entity *npc_entity = world_get_entity(world, npc);

        (void)player_a;

        if (!expect_int("spawned", player_b_entity != NULL && npc_entity != NULL, 1))
            return 1;

        player_b_entity->active = 0;
        npc_entity->controller_id = 20;

        if (!expect_int("active player count", runtime_world_active_player_count(world), 1))
            return 1;
        if (!expect_int("entity state", runtime_world_entity_state(world, 2, controllers, 2), 1))
            return 1;
        if (!expect_int("entity state missing", runtime_world_entity_state(world, 1, controllers, 2), 0))
            return 1;
    }

    world_destroy(world);
    return 0;
}
