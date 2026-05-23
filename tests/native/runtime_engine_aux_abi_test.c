#include "runtime/engine_aux_abi.h"

#include <stdio.h>
#include <stdlib.h>
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

static void test_destroy(ControllerInstance *self)
{
    if (!self)
        return;
    self->update = NULL;
    self->on_signal = NULL;
    self->destroy = NULL;
}

static ControllerInstance *test_factory(float x, float y, float z)
{
    ControllerInstance *instance = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    if (!instance)
        return NULL;

    strncpy(instance->type_name, "aux_test", sizeof(instance->type_name) - 1);
    instance->position[0] = x;
    instance->position[1] = y;
    instance->position[2] = z;
    instance->update = test_update;
    instance->on_signal = test_signal;
    instance->destroy = test_destroy;
    return instance;
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
        printf("runtime_engine_aux_abi_test: start\n");

    ControllerInstance *controllers[8] = {0};
    int controller_count = 0;
    RuntimeEngineAuxContext context;
    World *world = world_create();
    EntityId npc_id = 0;
    uint32_t created_id = 0;
    uint32_t attached_id = 0;

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    controller_register("aux_test", test_factory);

    created_id = runtime_engine_aux_controller_create(controllers,
                                                      8,
                                                      &controller_count,
                                                      "aux_test",
                                                      1.f,
                                                      2.f,
                                                      3.f);
    if (!expect_int("controller created", created_id != 0 ? 1 : 0, 1))
        return 1;

    npc_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 4.f, 0.f, 5.f);

    context.world = world;
    context.controllers = controllers;
    context.controller_count = controller_count;

    attached_id = runtime_engine_aux_controller_attach(context,
                                                       8,
                                                       &controller_count,
                                                       npc_id,
                                                       "aux_test");
    if (!expect_int("controller attached", attached_id != 0 ? 1 : 0, 1))
        return 1;

    context.controller_count = controller_count;

    if (!expect_int("update controller", runtime_engine_aux_controller_update(context, created_id, 1.f / 60.f), 1))
        return 1;
    if (!expect_int("signal controller", runtime_engine_aux_controller_signal(context, attached_id, "ping"), 1))
        return 1;
    if (!expect_int("missing update rejected", runtime_engine_aux_controller_update(context, 999u, 0.1f), 0))
        return 1;

    if (!expect_int("update called", s_update_calls, 1))
        return 1;
    if (!expect_int("signal called", s_signal_calls, 1))
        return 1;

    if (!expect_int("entity count", runtime_engine_aux_entity_count(world), 1))
        return 1;

    if (!expect_int("entity x telemetry", runtime_engine_aux_entity_x(world, 0) == 4.f ? 1 : 0, 1))
        return 1;
    if (!expect_int("entity z telemetry", runtime_engine_aux_entity_z(world, 0) == 5.f ? 1 : 0, 1))
        return 1;

    if (!expect_int("entity state has controller", runtime_engine_aux_entity_state(context, 0), 1))
        return 1;

    if (!expect_int("active player count", runtime_engine_aux_active_player_count(world), 0))
        return 1;

    if (!expect_int("parallel sync availability bool",
                    runtime_engine_aux_parallel_sync_available() == 0 || runtime_engine_aux_parallel_sync_available() == 1,
                    1))
    {
        return 1;
    }

    /* Keep teardown minimal in this seam test to avoid CRT ownership noise. */
    world_destroy(world);
    printf("runtime_engine_aux_abi_test: pass\n");
    return 0;
}
