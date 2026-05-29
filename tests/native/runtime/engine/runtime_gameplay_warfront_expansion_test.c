#include "runtime/engine/gameplay_service.h"
#include "runtime/controller/attach/controller_attach.h"
#include "ai/wildlife_controller.h"
#include "ai/combat_controller.h"

#include <stdio.h>
#include <string.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int count_team(ControllerInstance **controllers, int controller_count, ControllerTeam team)
{
    int count = 0;

    for (int i = 0; i < controller_count; i++)
    {
        if (!controllers[i])
            continue;
        if (controllers[i]->team == team)
            count += 1;
    }

    return count;
}

int main(void)
{
    World *world = world_create();
    ControllerInstance *controllers[16] = {0};
    int controller_count = 0;
    int pickup_collected = 1;
    EntityId player_id = 0;
    EntityId banana_id = 0;
    EntityId bean_id = 0;
    uint32_t banana_controller = 0;
    uint32_t bean_controller = 0;
    int base_entities = 0;

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    wildlife_controller_register();
    combat_controller_register();

    player_id = world_spawn_entity(world, ENTITY_TYPE_PLAYER, -16.0f, 0.0f, -16.0f);
    banana_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 0.0f, 0.0f, 0.0f);
    bean_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 2.0f, 0.0f, 0.0f);

    banana_controller = runtime_controller_attach_to_entity_with_team(world,
                                                                      controllers,
                                                                      16,
                                                                      &controller_count,
                                                                      banana_id,
                                                                      "combat",
                                                                      CONTROLLER_TEAM_BANANA);
    bean_controller = runtime_controller_attach_to_entity_with_team(world,
                                                                    controllers,
                                                                    16,
                                                                    &controller_count,
                                                                    bean_id,
                                                                    "wildlife",
                                                                    CONTROLLER_TEAM_BEAN);

    if (!expect_int("banana controller attached", banana_controller != 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("bean controller attached", bean_controller != 0 ? 1 : 0, 1))
        return 1;

    {
        Entity *banana = world_get_entity(world, banana_id);
        Entity *bean = world_get_entity(world, bean_id);
        if (banana)
            strncpy(banana->controller_kind, "combat", sizeof(banana->controller_kind) - 1);
        if (bean)
            strncpy(bean->controller_kind, "wildlife", sizeof(bean->controller_kind) - 1);
    }

    base_entities = world->entity_count;

    runtime_gameplay_service_tick(world,
                                  controllers,
                                  16,
                                  &controller_count,
                                  player_id,
                                  &pickup_collected,
                                  0.0f,
                                  1.0f,
                                  5.0f,
                                  2,
                                  12);

    if (!expect_int("world expanded by reinforcements", world->entity_count, base_entities + 2))
        return 1;
    if (!expect_int("controllers expanded by reinforcements", controller_count, 4))
        return 1;
    if (!expect_int("banana team count", count_team(controllers, controller_count, CONTROLLER_TEAM_BANANA), 2))
        return 1;
    if (!expect_int("bean team count", count_team(controllers, controller_count, CONTROLLER_TEAM_BEAN), 2))
        return 1;

    base_entities = world->entity_count;
    runtime_gameplay_service_tick(world,
                                  controllers,
                                  16,
                                  &controller_count,
                                  player_id,
                                  &pickup_collected,
                                  0.0f,
                                  1.0f,
                                  5.0f,
                                  2,
                                  4);

    if (!expect_int("population cap blocks additional spawn", world->entity_count, base_entities))
        return 1;

    world_destroy(world);
    for (int i = 0; i < controller_count; i++)
        controller_destroy(controllers[i]);

    return 0;
}
