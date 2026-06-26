#include "runtime/world/world_metrics.h"
#include "runtime/controller/runtime/controller_runtime.h"

#include "runtime/support/test_support.h"

ControllerInstance *runtime_controller_find_by_id(ControllerInstance **controllers,
                                                  int controller_count,
                                                  uint32_t controller_id)
{
    int index = 0;

    if (!controllers || controller_count <= 0 || controller_id == 0)
        return NULL;

    for (index = 0; index < controller_count; index++)
    {
        ControllerInstance *controller = controllers[index];
        if (controller && controller->id == controller_id)
            return controller;
    }

    return NULL;
}

static void test_world_active_player_count_paths(void **state)
{
    (void)state;
    World world = {0};
    Entity player_active = { .id = 1, .type = ENTITY_TYPE_PLAYER, .active = 1 };
    Entity player_inactive = { .id = 2, .type = ENTITY_TYPE_PLAYER, .active = 0 };
    Entity npc_active = { .id = 3, .type = ENTITY_TYPE_NPC, .active = 1 };

    BANANA_TEST_ASSERT_INT_EQ(runtime_world_active_player_count(NULL),
                              0,
                              "null worlds must report zero active players");

    world.entities[0] = &player_active;
    world.entities[1] = &player_inactive;
    world.entities[2] = &npc_active;
    world.entity_count = 3;

    BANANA_TEST_ASSERT_INT_EQ(runtime_world_active_player_count(&world),
                              1,
                              "only active player entities should be counted");
}

static void test_world_entity_state_paths(void **state)
{
    (void)state;
    World world = {0};
    Entity entity = { .id = 11, .type = ENTITY_TYPE_NPC, .active = 1, .controller_id = 22 };
    ControllerInstance controller = { .id = 22 };
    ControllerInstance *controllers[1] = { &controller };

    BANANA_TEST_ASSERT_INT_EQ(runtime_world_entity_state(NULL, 0, controllers, 1),
                              0,
                              "null worlds must report inactive state");

    world.entities[0] = &entity;
    world.entity_count = 1;

    BANANA_TEST_ASSERT_INT_EQ(runtime_world_entity_state(&world, -1, controllers, 1),
                              0,
                              "negative entity indices must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_world_entity_state(&world, 3, controllers, 1),
                              0,
                              "out-of-range entity indices must be rejected");

    entity.controller_id = 0;
    BANANA_TEST_ASSERT_INT_EQ(runtime_world_entity_state(&world, 0, controllers, 1),
                              0,
                              "entities without controller ids must report inactive state");

    entity.controller_id = 22;
    BANANA_TEST_ASSERT_INT_EQ(runtime_world_entity_state(&world, 0, NULL, 1),
                              0,
                              "missing controller registry must report inactive state");

    BANANA_TEST_ASSERT_INT_EQ(runtime_world_entity_state(&world, 0, controllers, 1),
                              1,
                              "matching controller entries must report active state");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_world_active_player_count_paths),
    BANANA_TEST_CASE(test_world_entity_state_paths)
)
