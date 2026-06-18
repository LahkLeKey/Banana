#include "runtime/controller/runtime/controller_runtime.h"
#include "../../support/test_support.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static int g_update_calls;
static int g_signal_calls;
static int g_last_signal_value;

static void fake_update(ControllerInstance *self, float dt)
{
    (void)self;
    g_update_calls += (int)dt;
}

static void fake_signal(ControllerInstance *self, const char *signal, const void *data)
{
    (void)self;
    if (signal && strcmp(signal, "enemy_spotted") == 0 && data)
    {
        const float *pos = (const float *)data;
        g_signal_calls += 1;
        g_last_signal_value = (int)(pos[0] * 10.0f + pos[2] * 10.0f);
    }
}

static void fake_destroy(ControllerInstance *self)
{
    (void)self;
}

static ControllerInstance *make_controller(uint32_t id, ControllerTeam team, float x, float y, float z)
{
    ControllerInstance *controller = (ControllerInstance *)calloc(1, sizeof(*controller));
    if (!controller)
        return NULL;

    controller->id = id;
    controller->team = team;
    controller->position[0] = x;
    controller->position[1] = y;
    controller->position[2] = z;
    controller->update = fake_update;
    controller->on_signal = fake_signal;
    controller->destroy = fake_destroy;
    return controller;
}

static void test_runtime_controller_helpers(void **state)
{
    (void)state;
    ControllerInstance *alpha = make_controller(1, CONTROLLER_TEAM_BANANA, 0.0f, 0.0f, 0.0f);
    ControllerInstance *beta = make_controller(2, CONTROLLER_TEAM_BEAN, 0.2f, 0.0f, 0.0f);
    ControllerInstance *controllers[] = { alpha, beta };
    int signal_value = 7;

    assert_non_null(alpha);
    assert_non_null(beta);

    g_update_calls = 0;
    g_signal_calls = 0;
    g_last_signal_value = 0;

    assert_non_null(runtime_controller_find_by_id(controllers, 2, 1));
    assert_null(runtime_controller_find_by_id(controllers, 2, 9));

    assert_true(runtime_controller_update_by_id(controllers, 2, 1, 2.0f));
    assert_int_equal(g_update_calls, 2);

    assert_true(runtime_controller_signal_by_id(controllers, 2, 2, "enemy_spotted", &signal_value));
    assert_int_equal(g_signal_calls, 1);

    assert_true(runtime_controller_assign_team_by_id(controllers, 2, 2, CONTROLLER_TEAM_NEUTRAL));
    assert_int_equal((int)beta->team, (int)CONTROLLER_TEAM_NEUTRAL);

    beta->team = CONTROLLER_TEAM_BEAN;

    g_signal_calls = 0;
    g_last_signal_value = 0;
    assert_int_equal(runtime_controller_signal_team_war(controllers, 2, 1.0f, "enemy_spotted"), 2);
    assert_int_equal(g_signal_calls, 2);

    controller_destroy(alpha);
    controller_destroy(beta);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_runtime_controller_helpers),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_update_calls;
static int g_signal_calls;
static int g_last_signal_value;

static void fake_update(ControllerInstance *self, float dt)
{
    (void)self;
    g_update_calls += (int)dt;
}

static void fake_signal(ControllerInstance *self, const char *signal, const void *data)
{
    (void)self;
    if (signal && strcmp(signal, "enemy_spotted") == 0 && data)
    {
        const float *pos = (const float *)data;
        g_signal_calls += 1;
        g_last_signal_value = (int)(pos[0] * 10.0f + pos[2] * 10.0f);
    }
}

static void fake_destroy(ControllerInstance *self)
{
    (void)self;
}

static ControllerInstance *make_controller(uint32_t id, ControllerTeam team, float x, float y, float z)
{
    ControllerInstance *controller = (ControllerInstance *)calloc(1, sizeof(*controller));
    if (!controller)
        return NULL;

    controller->id = id;
    controller->team = team;
    controller->position[0] = x;
    controller->position[1] = y;
    controller->position[2] = z;
    controller->update = fake_update;
    controller->on_signal = fake_signal;
    controller->destroy = fake_destroy;
    return controller;
}

static int test_runtime_controller_helpers(void)
{
    ControllerInstance *alpha = make_controller(1, CONTROLLER_TEAM_BANANA, 0.0f, 0.0f, 0.0f);
    ControllerInstance *beta = make_controller(2, CONTROLLER_TEAM_BEAN, 0.2f, 0.0f, 0.0f);
    ControllerInstance *controllers[] = { alpha, beta };
    int signal_value = 7;

    if (banana_test_fail_if(!alpha || !beta, "controller runtime fixtures must allocate cleanly"))
        return 1;

    g_update_calls = 0;
    g_signal_calls = 0;
    g_last_signal_value = 0;

    if (banana_test_fail_if(!runtime_controller_find_by_id(controllers, 2, 1), "find-by-id must locate an existing controller") ||
        banana_test_fail_if(runtime_controller_find_by_id(controllers, 2, 9) != NULL,
                "find-by-id must reject missing controllers"))
        return 1;

    if (banana_test_fail_if(!runtime_controller_update_by_id(controllers, 2, 1, 2.0f),
                "update-by-id must invoke the matching controller") ||
        banana_test_fail_if(g_update_calls != 2,
                "update-by-id must forward the dt argument through the callback"))
        return 1;

    if (banana_test_fail_if(!runtime_controller_signal_by_id(controllers, 2, 2, "enemy_spotted", &signal_value),
                "signal-by-id must dispatch to the matched controller") ||
        banana_test_fail_if(g_signal_calls != 1,
                "signal-by-id must trigger the controller callback"))
        return 1;

    if (banana_test_fail_if(!runtime_controller_assign_team_by_id(controllers, 2, 2, CONTROLLER_TEAM_NEUTRAL),
                "assign-team-by-id must update controller team membership") ||
        banana_test_fail_if(beta->team != CONTROLLER_TEAM_NEUTRAL,
                "assign-team-by-id must store the requested team"))
        return 1;

    beta->team = CONTROLLER_TEAM_BEAN;

    g_signal_calls = 0;
    g_last_signal_value = 0;
    if (runtime_controller_signal_team_war(controllers, 2, 1.0f, "enemy_spotted") != 2)
        return banana_test_fail_if(1, "signal-team-war must alert two hostile controllers within the trigger radius");

    if (banana_test_fail_if(g_signal_calls != 2,
                "signal-team-war must signal both hostile controllers in the pair"))
        return 1;

    controller_destroy(alpha);
    controller_destroy(beta);
    return 0;
}

int main(void)
{
    return test_runtime_controller_helpers();
}
#endif
