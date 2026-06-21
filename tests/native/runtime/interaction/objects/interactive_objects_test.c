#include "runtime/interaction/objects/interactive_objects.h"

#include "../../support/test_support.h"

static void test_init_and_clear(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_init(),
                              0,
                              "init must succeed");
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_count(),
                              0,
                              "count must be zero after init");
    interactive_objects_clear();
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_count(),
                              0,
                              "count must remain zero after clear");
}

static void test_register_click_state(void **state)
{
    (void)state;
    int object_id = 0;
    InteractiveObjectState obj_state;
    InteractiveObject out_obj;
    InteractiveObject snap[4];

    interactive_objects_init();

    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_register(NULL),
                              -1,
                              "null object must be rejected");

    InteractiveObject tree = {
        .type = INTERACTIVE_TYPE_TREE,
        .x = 10.0f,
        .z = 20.0f,
        .click_radius = 2.0f,
        .resource_yield = 5,
        .resource_type = "wood",
    };
    int id1 = interactive_objects_register(&tree);
    BANANA_TEST_ASSERT_TRUE(id1 > 0,
                            "registered object must receive a positive id");
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_count(),
                              1,
                              "count must be one after single registration");

    InteractiveObject ore = {
        .type = INTERACTIVE_TYPE_ORE_DEPOSIT,
        .x = 30.0f,
        .z = 40.0f,
        .click_radius = 3.0f,
        .resource_yield = 10,
        .resource_type = "ore",
    };
    int id2 = interactive_objects_register(&ore);
    BANANA_TEST_ASSERT_TRUE(id2 > id1,
                            "second registered id must be greater than first");
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_count(),
                              2,
                              "count must be two after two registrations");

    /* Click miss */
    int yield = interactive_objects_click_at(99.0f, 99.0f, 1.0f, &object_id);
    BANANA_TEST_ASSERT_INT_EQ(yield, 0,
                              "click far from all objects must return zero yield");

    /* Click hit */
    yield = interactive_objects_click_at(10.5f, 20.5f, 3.0f, &object_id);
    BANANA_TEST_ASSERT_INT_EQ(yield, 5,
                              "click within tree radius must return resource yield");
    BANANA_TEST_ASSERT_INT_EQ(object_id, id1,
                              "click must report the correct object id");

    /* State after click should be depleted */
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_get_state(id1, &obj_state),
                              0,
                              "get_state must succeed for valid id");
    BANANA_TEST_ASSERT_INT_EQ((int)obj_state,
                              (int)INTERACTIVE_STATE_DEPLETED,
                              "clicked object must be in depleted state");

    /* Cannot click depleted object */
    yield = interactive_objects_click_at(10.5f, 20.5f, 3.0f, &object_id);
    BANANA_TEST_ASSERT_INT_EQ(yield, 0,
                              "depleted objects must not yield on second click");

    /* set_state path */
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_set_state(id1,
                                                             INTERACTIVE_STATE_ACTIVE,
                                                             1000),
                              0,
                              "set_state must succeed for valid id");
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_get_state(id1, &obj_state),
                              0,
                              "get_state must return updated state");
    BANANA_TEST_ASSERT_INT_EQ((int)obj_state,
                              (int)INTERACTIVE_STATE_ACTIVE,
                              "state must be active after set");

    /* get by id */
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_get(id1, &out_obj),
                              0,
                              "get by valid id must succeed");
    BANANA_TEST_ASSERT_INT_EQ(out_obj.resource_yield,
                              5,
                              "retrieved object must preserve resource yield");
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_get(9999, &out_obj),
                              -1,
                              "get by unknown id must fail");

    /* snapshot */
    int n = interactive_objects_snapshot(4, snap);
    BANANA_TEST_ASSERT_INT_EQ(n, 2,
                              "snapshot must return all registered objects");

    /* tick paths */
    interactive_objects_set_state(id1, INTERACTIVE_STATE_DEPLETED, 0);
    interactive_objects_tick(5000, 3000);   /* cooldown expired */
    interactive_objects_tick(100, 3000);    /* cooldown not expired */

    /* get_state null guard */
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_get_state(id1, NULL),
                              -1,
                              "get_state with null out pointer must fail");
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_get_state(9999, &obj_state),
                              -1,
                              "get_state with unknown id must fail");
    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_set_state(9999,
                                                             INTERACTIVE_STATE_ACTIVE,
                                                             0),
                              -1,
                              "set_state with unknown id must fail");

    BANANA_TEST_ASSERT_INT_EQ(interactive_objects_click_at(0.0f, 0.0f, 1.0f, NULL),
                              -1,
                              "click with null out_object_id must fail");

    interactive_objects_shutdown();
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_init_and_clear),
    BANANA_TEST_CASE(test_register_click_state)
)
