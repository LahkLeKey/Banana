#include "runtime/controller/controller_attach.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_destroy(ControllerInstance *self)
{
    if (!self)
        return;
    if (self->state)
        free(self->state);
    self->state = NULL;
}

static ControllerInstance *test_factory(float x, float y, float z)
{
    ControllerInstance *instance = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    if (!instance)
        return NULL;

    strncpy(instance->type_name, "attach_test", sizeof(instance->type_name) - 1);
    instance->position[0] = x;
    instance->position[1] = y;
    instance->position[2] = z;
    instance->destroy = test_destroy;
    instance->state = malloc(8);
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
    ControllerInstance *controllers[4] = {0};
    int controller_count = 0;
    World *world = world_create();
    EntityId entity_id = 0;
    uint32_t controller_id = 0;

    controller_register("attach_test", test_factory);

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    controller_id = runtime_controller_create_and_register(controllers,
                                                           4,
                                                           &controller_count,
                                                           "attach_test",
                                                           1.0f,
                                                           2.0f,
                                                           3.0f);
    if (!expect_int("created controller", controller_id != 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("controller count", controller_count, 1))
        return 1;

    entity_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 4.0f, 5.0f, 6.0f);
    controller_id = runtime_controller_attach_to_entity(world,
                                                        controllers,
                                                        4,
                                                        &controller_count,
                                                        entity_id,
                                                        "attach_test");
    if (!expect_int("attached controller", controller_id != 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("controller count 2", controller_count, 2))
        return 1;

    {
        Entity *entity = world_get_entity(world, entity_id);
        if (!expect_int("entity exists", entity != NULL ? 1 : 0, 1))
            return 1;
        if (!expect_int("entity controller id", (int)entity->controller_id, (int)controller_id))
            return 1;
    }

    for (int i = 0; i < controller_count; i++)
        controller_destroy(controllers[i]);

    world_destroy(world);
    return 0;
}
