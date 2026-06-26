#include "world/world.h"
#include "runtime/support/test_support.h"

static void test_world_spawn_and_lookup(void **state)
{
    (void)state;
    World *world = world_create();
    EntityId id = 0;
    Entity *entity = NULL;

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must allocate a usable world");

    id = world_spawn_entity(world, ENTITY_TYPE_DYNAMIC, 1.0f, 2.0f, 3.0f);
    BANANA_TEST_ASSERT_TRUE(id != 0, "world_spawn_entity must return non-zero id");

    entity = world_get_entity(world, id);
    BANANA_TEST_ASSERT_TRUE(entity != NULL, "world_get_entity must return spawned entity");
    BANANA_TEST_ASSERT_INT_EQ(entity->type, ENTITY_TYPE_DYNAMIC,
                              "spawned entity type must be preserved");
    BANANA_TEST_ASSERT_FLOAT_EQ(entity->position[0], 1.0f, 0.0001f,
                                "spawned entity position x must be preserved");
    BANANA_TEST_ASSERT_FLOAT_EQ(entity->position[1], 2.0f, 0.0001f,
                                "spawned entity position y must be preserved");
    BANANA_TEST_ASSERT_FLOAT_EQ(entity->position[2], 3.0f, 0.0001f,
                                "spawned entity position z must be preserved");

    world_tick(world, 0.016f);
    world_despawn_entity(world, id);
    BANANA_TEST_ASSERT_TRUE(world_get_entity(world, id) == NULL,
                            "world_despawn_entity must remove entity from world");

    world_destroy(world);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_world_spawn_and_lookup)
)
