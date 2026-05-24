#include "runtime/controller/sync/controller_sync.h"

#include <stdio.h>
#include <string.h>

static void sync_update(ControllerInstance *self, float dt)
{
    (void)dt;
    self->position[0] = 5.0f;
    self->position[1] = 6.0f;
    self->position[2] = 7.0f;
}

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    World *world = world_create();
    ControllerInstance controller;
    ControllerInstance *controllers[1] = {&controller};
    EntityId entity_id = 0;

    memset(&controller, 0, sizeof(controller));
    controller.id = 900;
    controller.update = sync_update;

    if (!expect_true("world", world != NULL))
        return 1;

    entity_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 0.f, 0.f, 0.f);
    {
        Entity *entity = world_get_entity(world, entity_id);
        if (!expect_true("entity", entity != NULL))
            return 1;
        entity->controller_id = controller.id;
    }

    runtime_sync_controller_positions(world, controllers, 1, 1.0f / 60.0f);

    {
        Entity *entity = world_get_entity(world, entity_id);
        if (!expect_true("x synced", entity->position[0] == 5.0f))
            return 1;
        if (!expect_true("y synced", entity->position[1] == 6.0f))
            return 1;
        if (!expect_true("z synced", entity->position[2] == 7.0f))
            return 1;
    }

    world_destroy(world);
    return 0;
}
