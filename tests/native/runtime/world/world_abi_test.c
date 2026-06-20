#include "runtime/world/world_abi.h"
#include "world/world.h"

#include "../support/test_support.h"

static void test_world_abi_guards_null_world(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ((int)runtime_world_abi_spawn(NULL, ENTITY_TYPE_DYNAMIC, 1.0f, 2.0f, 3.0f),
                              0,
                              "world abi spawn must fail closed when world is null");

    runtime_world_abi_tick(NULL, 0.016f);

    BANANA_TEST_ASSERT_TRUE(1,
                            "world abi tick must tolerate null world input");
}

static void test_world_abi_forwards_spawn_and_tick(void **state)
{
    (void)state;
    World *world = world_create();
    EntityId entity_id = 0;
    Entity *entity = NULL;

    BANANA_TEST_ASSERT_TRUE(world != NULL,
                            "world fixture must allocate successfully");

    entity_id = runtime_world_abi_spawn(world, ENTITY_TYPE_DYNAMIC, 4.0f, 5.0f, 6.0f);
    BANANA_TEST_ASSERT_TRUE(entity_id != 0,
                            "world abi spawn must return created entity id");

    entity = world_get_entity(world, entity_id);
    BANANA_TEST_ASSERT_TRUE(entity != NULL,
                            "world abi spawn must insert entity into world");
    BANANA_TEST_ASSERT_INT_EQ(entity->type,
                              ENTITY_TYPE_DYNAMIC,
                              "world abi spawn must preserve entity type");

    runtime_world_abi_tick(world, 0.016f);

    BANANA_TEST_ASSERT_TRUE(world_get_entity(world, entity_id) != NULL,
                            "world abi tick must keep active entities accessible");

    world_destroy(world);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_world_abi_guards_null_world),
    BANANA_TEST_CASE(test_world_abi_forwards_spawn_and_tick)
)
