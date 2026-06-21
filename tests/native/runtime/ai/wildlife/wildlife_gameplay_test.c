#include "runtime/wildlife/wildlife_gameplay.h"
#include "../../support/test_support.h"

#include <string.h>

static int g_signal_count = 0;

static void wildlife_signal_callback(ControllerInstance *self, const char *signal, const void *data)
{
    (void)self;

    if (!signal || !data || strcmp(signal, "player_nearby") != 0)
        return;

    g_signal_count += 1;
}

static void reset_wildlife_test_state(void)
{
    g_signal_count = 0;
}

static void test_wildlife_signal_rejects_invalid_inputs(void **state)
{
    (void)state;
    World world = {0};
    ControllerInstance *controllers[1] = {NULL};
    Entity player = {0};

    reset_wildlife_test_state();
    player.id = 1;
    player.active = 1;
    world.entities[0] = &player;
    world.entity_count = 1;

    BANANA_TEST_ASSERT_INT_EQ(runtime_wildlife_signal_player_nearby(NULL, controllers, 1, 1, 1.0f), 0,
                              "wildlife signaling must reject null world");
    BANANA_TEST_ASSERT_INT_EQ(runtime_wildlife_signal_player_nearby(&world, NULL, 1, 1, 1.0f), 0,
                              "wildlife signaling must reject null controller arrays");
    BANANA_TEST_ASSERT_INT_EQ(runtime_wildlife_signal_player_nearby(&world, controllers, 0, 1, 1.0f), 0,
                              "wildlife signaling must reject empty controller lists");
    BANANA_TEST_ASSERT_INT_EQ(runtime_wildlife_signal_player_nearby(&world, controllers, 1, 0, 1.0f), 0,
                              "wildlife signaling must reject missing player ids");
    BANANA_TEST_ASSERT_INT_EQ(runtime_wildlife_signal_player_nearby(&world, controllers, 1, 1, 0.0f), 0,
                              "wildlife signaling must reject non-positive trigger radii");
}

static void test_wildlife_signal_requires_active_player(void **state)
{
    (void)state;
    World world = {0};
    ControllerInstance *controllers[1] = {NULL};
    Entity player = {0};

    reset_wildlife_test_state();
    player.id = 1;
    player.active = 0;
    world.entities[0] = &player;
    world.entity_count = 1;

    BANANA_TEST_ASSERT_INT_EQ(runtime_wildlife_signal_player_nearby(&world, controllers, 1, 1, 1.0f), 0,
                              "wildlife signaling must reject inactive players");
}

static void test_wildlife_signal_hits_only_nearby_wildlife(void **state)
{
    (void)state;
    World world = {0};
    Entity player = {0};
    ControllerInstance wildlife_close = {0};
    ControllerInstance wildlife_far = {0};
    ControllerInstance other_type = {0};
    ControllerInstance *controllers[4] = {NULL, &wildlife_close, &other_type, &wildlife_far};

    reset_wildlife_test_state();

    player.id = 1;
    player.active = 1;
    player.position[0] = 10.0f;
    player.position[2] = 10.0f;
    world.entities[0] = &player;
    world.entity_count = 1;

    strncpy(wildlife_close.type_name, "wildlife", sizeof(wildlife_close.type_name) - 1);
    wildlife_close.position[0] = 11.0f;
    wildlife_close.position[2] = 10.5f;
    wildlife_close.on_signal = wildlife_signal_callback;

    strncpy(wildlife_far.type_name, "wildlife", sizeof(wildlife_far.type_name) - 1);
    wildlife_far.position[0] = 30.0f;
    wildlife_far.position[2] = 30.0f;
    wildlife_far.on_signal = wildlife_signal_callback;

    strncpy(other_type.type_name, "combat", sizeof(other_type.type_name) - 1);
    other_type.position[0] = 10.5f;
    other_type.position[2] = 10.5f;
    other_type.on_signal = wildlife_signal_callback;

    BANANA_TEST_ASSERT_INT_EQ(runtime_wildlife_signal_player_nearby(&world,
                                                                    controllers,
                                                                    4,
                                                                    player.id,
                                                                    5.0f),
                              1,
                              "wildlife signaling must hit only the nearby wildlife controller");
    BANANA_TEST_ASSERT_INT_EQ(g_signal_count, 1,
                              "only one wildlife controller should receive the nearby signal");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_wildlife_signal_rejects_invalid_inputs),
    BANANA_TEST_CASE(test_wildlife_signal_requires_active_player),
    BANANA_TEST_CASE(test_wildlife_signal_hits_only_nearby_wildlife)
)
