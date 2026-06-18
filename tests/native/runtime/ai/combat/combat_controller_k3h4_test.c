#include "ai/combat_controller.h"
#include "ai/controller.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_combat_bias_and_engagement(void **state)
{
    (void)state;
    ControllerInstance *controller = NULL;
    float target[3] = {10.0f, 0.0f, 10.0f};
    float bias = 0.0f;

    combat_controller_register();
    controller = controller_create("combat", 0.0f, 0.0f, 0.0f);
    assert_non_null(controller);

    controller_signal(controller, "battle_engage", target);
    bias = combat_controller_k3h4_bias(controller);

    assert_true(bias >= 0.0f && bias <= 1.0f);

    controller_destroy(controller);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_combat_bias_and_engagement),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>
#include <stdlib.h>

static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

int main(void)
{
    ControllerInstance *controller = NULL;
    float target[3] = {10.0f, 0.0f, 10.0f};
    float bias = 0.0f;

    combat_controller_register();
    controller = controller_create("combat", 0.0f, 0.0f, 0.0f);
    if (!controller)
        return 1;

    controller_signal(controller, "battle_engage", target);
    bias = combat_controller_k3h4_bias(controller);

    if (fail_if(!(bias >= 0.0f && bias <= 1.0f), "combat k3h4 bias must be in [0, 1]"))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_destroy(controller);
    return 0;
}
#endif
