#include "runtime/controller/attach/controller_attach.h"

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

static ControllerInstance *combat_factory(float x, float y, float z)
{
    ControllerInstance *instance = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    if (!instance)
        return NULL;

    strncpy(instance->type_name, "combat", sizeof(instance->type_name) - 1);
    instance->position[0] = x;
    instance->position[1] = y;
    instance->position[2] = z;
    instance->destroy = test_destroy;
    instance->state = malloc(8);
    return instance;
}

static ControllerInstance *wildlife_factory(float x, float y, float z)
{
    ControllerInstance *instance = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    if (!instance)
        return NULL;

    strncpy(instance->type_name, "wildlife", sizeof(instance->type_name) - 1);
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
    EntityId team_entity_id = 0;
    uint32_t controller_id = 0;
    uint32_t team_controller_id = 0;

    controller_register("attach_test", test_factory);
    controller_register("combat", combat_factory);
    controller_register("wildlife", wildlife_factory);

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

    team_entity_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 7.0f, 8.0f, 9.0f);
    team_controller_id = runtime_controller_attach_to_entity_with_team(world,
                                                                       controllers,
                                                                       4,
                                                                       &controller_count,
                                                                       team_entity_id,
                                                                       "attach_test",
                                                                       CONTROLLER_TEAM_BEAN);
    if (!expect_int("attached team controller", team_controller_id != 0 ? 1 : 0, 1))
        return 1;

    if (!expect_int("controller count 3", controller_count, 3))
        return 1;

    {
        Entity *entity = world_get_entity(world, entity_id);
        if (!expect_int("entity exists", entity != NULL ? 1 : 0, 1))
            return 1;
        if (!expect_int("entity controller id", (int)entity->controller_id, (int)controller_id))
            return 1;
    }

    {
        ControllerInstance *team_controller = controllers[2];
        if (!expect_int("team controller exists", team_controller != NULL ? 1 : 0, 1))
            return 1;
        if (!expect_int("team controller is bean", (int)team_controller->team, (int)CONTROLLER_TEAM_BEAN))
            return 1;
    }

    {
        uint32_t combat_id = runtime_controller_create_and_register(controllers,
                                                                    4,
                                                                    &controller_count,
                                                                    "combat",
                                                                    -2.0f,
                                                                    0.0f,
                                                                    1.0f);
        if (!expect_int("created combat controller", combat_id != 0 ? 1 : 0, 1))
            return 1;
        if (!expect_int("controller count 4", controller_count, 4))
            return 1;
        if (!expect_int("combat defaults to banana team",
                        (int)controllers[3]->team,
                        (int)CONTROLLER_TEAM_BANANA))
        {
            return 1;
        }
    }

    for (int i = 0; i < controller_count; i++)
        controller_destroy(controllers[i]);

    world_destroy(world);
    return 0;
}
