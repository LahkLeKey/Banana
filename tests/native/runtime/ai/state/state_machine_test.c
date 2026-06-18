#include "ai/state_machine.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

static void count_enter(void *ctx)
{
    int *counter = (int *)ctx;
    *counter += 1;
}

static void count_update(void *ctx, float dt)
{
    int *counter = (int *)ctx;
    *counter += (int)dt;
}

static void test_fsm_add_state_and_trigger(void **state)
{
    (void)state;
    StateMachine fsm;
    int counter = 0;
    int idle_idx = -1;
    int patrol_idx = -1;

    fsm_init(&fsm, &counter);
    idle_idx = fsm_add_state(&fsm, "idle", count_enter, count_update, NULL);
    patrol_idx = fsm_add_state(&fsm, "patrol", count_enter, count_update, NULL);
    fsm_add_transition(&fsm, idle_idx, patrol_idx, "start_patrol");

    assert_int_equal(idle_idx, 0);
    assert_int_equal(patrol_idx, 1);
    assert_string_equal(fsm_current_state_name(&fsm), "idle");
    assert_int_equal(counter, 1);

    fsm_trigger(&fsm, "start_patrol");
    assert_string_equal(fsm_current_state_name(&fsm), "patrol");
    assert_int_equal(counter, 2);

    fsm_update(&fsm, 1.5f);
    assert_int_equal(counter, 3);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fsm_add_state_and_trigger),
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

static void count_enter(void *ctx)
{
    int *counter = (int *)ctx;
    *counter += 1;
}

static void count_update(void *ctx, float dt)
{
    int *counter = (int *)ctx;
    *counter += (int)dt;
}

static int test_fsm_add_state_and_trigger(void)
{
    StateMachine fsm;
    int counter = 0;
    int idle_idx = -1;
    int patrol_idx = -1;

    fsm_init(&fsm, &counter);
    idle_idx = fsm_add_state(&fsm, "idle", count_enter, count_update, NULL);
    patrol_idx = fsm_add_state(&fsm, "patrol", count_enter, count_update, NULL);
    fsm_add_transition(&fsm, idle_idx, patrol_idx, "start_patrol");

    if (fail_if(idle_idx != 0 || patrol_idx != 1, "fsm state registration order must be stable") ||
        fail_if(strcmp(fsm_current_state_name(&fsm), "idle") != 0,
                "fsm should start on the first registered state") ||
        fail_if(counter != 1, "fsm initial state enter callback should run once"))
        return 1;

    fsm_trigger(&fsm, "start_patrol");
    if (fail_if(strcmp(fsm_current_state_name(&fsm), "patrol") != 0,
                "fsm trigger should transition to the linked state") ||
        fail_if(counter != 2, "fsm transition enter callback should run once"))
        return 1;

    fsm_update(&fsm, 1.5f);
    return fail_if(counter != 3, "fsm update should advance the state context while active");
}

int main(void)
{
    return test_fsm_add_state_and_trigger();
}
#endif
