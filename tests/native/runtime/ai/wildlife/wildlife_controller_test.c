#include "ai/wildlife_controller.h"
#include "ai/controller.h"
#include "../../support/test_support.h"
#include <string.h>

static void test_wildlife_debug_state_starts_idle(void **state)
{
    (void)state;
    ControllerInstance *controller = NULL;
    const char *state_name = NULL;

    wildlife_controller_register();
    controller = controller_create("wildlife", 0.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_TRUE(controller != NULL, "wildlife controller must instantiate");

    state_name = wildlife_controller_debug_state_name(controller);
    BANANA_TEST_ASSERT_TRUE(state_name != NULL, "wildlife debug state must be available");
    BANANA_TEST_ASSERT_STR_EQ(state_name, "idle", "wildlife controller should start in idle");

    controller_destroy(controller);
}

static void test_wildlife_signal_can_enter_combat(void **state)
{
    (void)state;
    ControllerInstance *controller = NULL;
    const char *state_name = NULL;
    float target[3] = {4.0f, 0.0f, 4.0f};

    wildlife_controller_register();
    controller = controller_create("wildlife", 0.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_TRUE(controller != NULL,
                            "wildlife controller must instantiate for combat signal test");

    controller_signal(controller, "battle_engage", target);
    state_name = wildlife_controller_debug_state_name(controller);
    BANANA_TEST_ASSERT_TRUE(state_name != NULL,
                            "wildlife debug state must report after battle_engage");
    BANANA_TEST_ASSERT_STR_EQ(state_name, "combat",
                              "battle_engage should move wildlife into combat mode");

    controller_destroy(controller);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_wildlife_debug_state_starts_idle),
    BANANA_TEST_CASE(test_wildlife_signal_can_enter_combat)
)
