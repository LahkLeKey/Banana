#include "ai/wildlife_controller.h"
#include "ai/controller.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

static void test_wildlife_debug_state_starts_idle(void **state)
{
    (void)state;
    ControllerInstance *controller = NULL;
    const char *state_name = NULL;

    wildlife_controller_register();
    controller = controller_create("wildlife", 0.0f, 0.0f, 0.0f);
    assert_non_null(controller);

    state_name = wildlife_controller_debug_state_name(controller);
    assert_non_null(state_name);
    assert_string_equal(state_name, "idle");

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
    assert_non_null(controller);

    controller_signal(controller, "battle_engage", target);
    state_name = wildlife_controller_debug_state_name(controller);
    assert_non_null(state_name);
    assert_string_equal(state_name, "combat");

    controller_destroy(controller);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_wildlife_debug_state_starts_idle),
        cmocka_unit_test(test_wildlife_signal_can_enter_combat),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>
#include <string.h>

static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

static int test_wildlife_debug_state_starts_idle(void)
{
    ControllerInstance *controller = NULL;
    const char *state = NULL;

    wildlife_controller_register();
    controller = controller_create("wildlife", 0.0f, 0.0f, 0.0f);
    if (!controller)
        return fail_if(1, "wildlife controller must instantiate");

    state = wildlife_controller_debug_state_name(controller);
    if (fail_if(state == NULL, "wildlife debug state must be available") ||
        fail_if(strcmp(state, "idle") != 0, "wildlife controller should start in idle"))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_destroy(controller);
    return 0;
}

static int test_wildlife_signal_can_enter_combat(void)
{
    ControllerInstance *controller = NULL;
    const char *state = NULL;
    float target[3] = {4.0f, 0.0f, 4.0f};

    wildlife_controller_register();
    controller = controller_create("wildlife", 0.0f, 0.0f, 0.0f);
    if (!controller)
        return fail_if(1, "wildlife controller must instantiate for combat signal test");

    controller_signal(controller, "battle_engage", target);
    state = wildlife_controller_debug_state_name(controller);
    if (fail_if(state == NULL, "wildlife debug state must report after battle_engage") ||
        fail_if(strcmp(state, "combat") != 0,
                "battle_engage should move wildlife into combat mode"))
    {
        controller_destroy(controller);
        return 1;
    }

    controller_destroy(controller);
    return 0;
}

int main(void)
{
    if (test_wildlife_debug_state_starts_idle() ||
        test_wildlife_signal_can_enter_combat())
        return 1;

    return 0;
}
#endif
