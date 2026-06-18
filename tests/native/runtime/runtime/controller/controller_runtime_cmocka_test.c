#include "runtime/controller/runtime/controller_runtime.h"

#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct ControllerFixture
{
    ControllerInstance *alpha;
    ControllerInstance *beta;
    ControllerInstance *controllers[2];
    int signal_value;
} ControllerFixture;

static int g_update_calls;
static int g_signal_calls;

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
        g_signal_calls += 1;
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

static int controller_fixture_setup(void **state)
{
    ControllerFixture *fixture = (ControllerFixture *)calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    fixture->alpha = make_controller(1, CONTROLLER_TEAM_BANANA, 0.0f, 0.0f, 0.0f);
    fixture->beta = make_controller(2, CONTROLLER_TEAM_BEAN, 0.2f, 0.0f, 0.0f);
    assert_non_null(fixture->alpha);
    assert_non_null(fixture->beta);

    fixture->controllers[0] = fixture->alpha;
    fixture->controllers[1] = fixture->beta;
    fixture->signal_value = 7;

    g_update_calls = 0;
    g_signal_calls = 0;

    *state = fixture;
    return 0;
}

static int controller_fixture_teardown(void **state)
{
    ControllerFixture *fixture = (ControllerFixture *)(*state);
    if (fixture)
    {
        controller_destroy(fixture->alpha);
        controller_destroy(fixture->beta);
        free(fixture);
    }
    return 0;
}

static void test_runtime_controller_basic_ops(void **state)
{
    ControllerFixture *fixture = (ControllerFixture *)(*state);

    assert_non_null(runtime_controller_find_by_id(fixture->controllers, 2, 1));
    assert_null(runtime_controller_find_by_id(fixture->controllers, 2, 9));

    assert_true(runtime_controller_update_by_id(fixture->controllers, 2, 1, 2.0f));
    assert_int_equal(g_update_calls, 2);

    assert_true(runtime_controller_signal_by_id(
        fixture->controllers,
        2,
        2,
        "enemy_spotted",
        &fixture->signal_value));
    assert_int_equal(g_signal_calls, 1);

    assert_true(runtime_controller_assign_team_by_id(
        fixture->controllers,
        2,
        2,
        CONTROLLER_TEAM_NEUTRAL));
    assert_int_equal((int)fixture->beta->team, (int)CONTROLLER_TEAM_NEUTRAL);
}

static void test_runtime_controller_war_signal(void **state)
{
    ControllerFixture *fixture = (ControllerFixture *)(*state);

    g_signal_calls = 0;

    assert_int_equal(
        runtime_controller_signal_team_war(fixture->controllers, 2, 1.0f, "enemy_spotted"),
        2);
    assert_int_equal(g_signal_calls, 2);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_runtime_controller_basic_ops,
            controller_fixture_setup,
            controller_fixture_teardown),
        cmocka_unit_test_setup_teardown(
            test_runtime_controller_war_signal,
            controller_fixture_setup,
            controller_fixture_teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
