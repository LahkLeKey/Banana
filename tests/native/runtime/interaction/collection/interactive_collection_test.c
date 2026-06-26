#include "runtime/interaction/collection/interactive_collection.h"

#include "runtime/support/test_support.h"

static void test_init_register_collect_lifecycle(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_init(),
                              0,
                              "init must succeed");

    /* validation guards */
    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_register(-1, RESOURCE_WOOD, 5, 3, 1000),
                              -1,
                              "negative object id must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_register(1, RESOURCE_COUNT, 5, 3, 1000),
                              -1,
                              "invalid resource type must be rejected");

    /* register two objects */
    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_register(10, RESOURCE_WOOD, 5, 3, 500),
                              0,
                              "valid register must succeed");
    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_register(10, RESOURCE_WOOD, 5, 3, 500),
                              -1,
                              "duplicate object_id must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_register(11, RESOURCE_ORE, 8, 2, 300),
                              0,
                              "second object registration must succeed");

    /* collect from valid object */
    int result = interactive_collection_collect(10, "player-a");
    BANANA_TEST_ASSERT_INT_EQ(result,
                              COLLECTION_OK,
                              "first collect on fresh object must succeed");

    /* advance time so cooldown clears, then null player path */
    interactive_collection_tick(2000);
    result = interactive_collection_collect(10, NULL);
    BANANA_TEST_ASSERT_TRUE(result == COLLECTION_INVALID_PLAYER || result == COLLECTION_OBJECT_COOLDOWN,
                            "null player_guid must return invalid-player or cooldown code");

    /* unknown object */
    result = interactive_collection_collect(9999, "player-a");
    BANANA_TEST_ASSERT_INT_EQ(result,
                              COLLECTION_OBJECT_NOT_FOUND,
                              "unknown object must return not found code");

    /* exhaust hits to trigger depleted state; advance tick between each collect to clear cooldown */
    interactive_collection_tick(1000);
    interactive_collection_collect(11, "player-b");
    interactive_collection_tick(1000);
    interactive_collection_collect(11, "player-b");   /* max_hits == 2 */
    result = interactive_collection_collect(11, "player-b");
    BANANA_TEST_ASSERT_INT_EQ(result,
                              COLLECTION_OBJECT_DEPLETED,
                              "exhausted object must return depleted code");

    /* respawn restores hits */
    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_respawn(11),
                              0,
                              "respawn must succeed for known object");
    result = interactive_collection_collect(11, "player-b");
    BANANA_TEST_ASSERT_INT_EQ(result,
                              COLLECTION_OK,
                              "collection after respawn must succeed");

    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_respawn(9999),
                              -1,
                              "respawn for unknown object must fail");

    /* get state */
    const CollectibleObject *obj = interactive_collection_get(10);
    BANANA_TEST_ASSERT_TRUE(obj != NULL,
                            "get for known object must return non-null");
    BANANA_TEST_ASSERT_TRUE(interactive_collection_get(9999) == NULL,
                            "get for unknown object must return NULL");

    interactive_collection_cleanup();
}

static void test_cooldown_path(void **state)
{
    (void)state;
    interactive_collection_init();
    interactive_collection_register(20, RESOURCE_WOOD, 3, 10, 1000);

    /* First collect succeeds */
    BANANA_TEST_ASSERT_INT_EQ(interactive_collection_collect(20, "player-c"),
                              COLLECTION_OK,
                              "first collect must succeed");

    /* Immediate second collect triggers cooldown (tick_accumulator hasn't advanced) */
    int result = interactive_collection_collect(20, "player-c");
    BANANA_TEST_ASSERT_INT_EQ(result,
                              COLLECTION_OBJECT_COOLDOWN,
                              "immediate second collect must be on cooldown");

    /* Advance time past cooldown */
    interactive_collection_tick(2000);
    result = interactive_collection_collect(20, "player-c");
    BANANA_TEST_ASSERT_INT_EQ(result,
                              COLLECTION_OK,
                              "collect after cooldown elapsed must succeed");

    interactive_collection_cleanup();
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_init_register_collect_lifecycle),
    BANANA_TEST_CASE(test_cooldown_path)
)
