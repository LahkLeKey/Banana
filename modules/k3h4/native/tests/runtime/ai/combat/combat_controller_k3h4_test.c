#include "ai/combat_controller.h"
#include "ai/controller.h"
#include "runtime/support/test_support.h"

static void test_combat_bias_and_engagement(void **state)
{
    (void)state;
    ControllerInstance *controller = NULL;
    float target[3] = {10.0f, 0.0f, 10.0f};
    float bias = 0.0f;

    combat_controller_register();
    controller = controller_create("combat", 0.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_TRUE(controller != NULL, "combat controller must instantiate");

    controller_signal(controller, "battle_engage", target);
    bias = combat_controller_k3h4_bias(controller);

    BANANA_TEST_ASSERT_TRUE(bias >= 0.0f && bias <= 1.0f,
                            "combat k3h4 bias must be in [0, 1]");

    controller_destroy(controller);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_combat_bias_and_engagement)
)
