#include "ai/controller.h"
#include "ai/controller_system.h"
#include "../../support/test_support.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static int g_updates = 0;
static int g_signals = 0;
static int g_destroys = 0;

static void fake_update(ControllerInstance *self, float dt)
{
    (void)self;
    g_updates += (int)dt;
}

static void fake_signal(ControllerInstance *self, const char *signal, const void *data)
{
    (void)self;
    if (signal && data && strcmp(signal, "pulse") == 0)
        g_signals += *(const int *)data;
}

static void fake_destroy(ControllerInstance *self)
{
    (void)self;
    g_destroys += 1;
}

static ControllerInstance *fake_factory(float x, float y, float z)
{
    ControllerInstance *inst = (ControllerInstance *)calloc(1, sizeof(*inst));
    if (!inst)
        return NULL;

    inst->id = 7;
    inst->team = CONTROLLER_TEAM_BANANA;
    inst->position[0] = x;
    inst->position[1] = y;
    inst->position[2] = z;
    strncpy(inst->type_name, "fake", 63);
    inst->update = fake_update;
    inst->on_signal = fake_signal;
    inst->destroy = fake_destroy;
    return inst;
}

static void test_controller_factory_and_system(void **state)
{
    (void)state;
    ControllerSystem *sys = NULL;
    ControllerInstance *inst = NULL;
    int signal = 4;

    g_updates = 0;
    g_signals = 0;
    g_destroys = 0;

    controller_register("fake", fake_factory);
    sys = controller_system_create();
    assert_non_null(sys);

    inst = controller_system_create_instance(sys, "fake", 1.0f, 2.0f, 3.0f);
    assert_non_null(inst);
    assert_int_equal(inst->id, 7);
    assert_int_equal(inst->team, CONTROLLER_TEAM_BANANA);

    controller_system_update(sys, 5.0f);
    assert_int_equal(g_updates, 5);

    controller_system_signal(sys, inst->id, "pulse", &signal);
    assert_int_equal(g_signals, 4);

    controller_system_destroy_instance(sys, inst->id);
    assert_int_equal(g_destroys, 1);

    controller_system_destroy(sys);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_controller_factory_and_system),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_updates = 0;
static int g_signals = 0;
static int g_destroys = 0;

static void fake_update(ControllerInstance *self, float dt)
{
    (void)self;
    g_updates += (int)dt;
}

static void fake_signal(ControllerInstance *self, const char *signal, const void *data)
{
    (void)self;
    if (signal && data && strcmp(signal, "pulse") == 0)
        g_signals += *(const int *)data;
}

static void fake_destroy(ControllerInstance *self)
{
    (void)self;
    g_destroys += 1;
}

static ControllerInstance *fake_factory(float x, float y, float z)
{
    ControllerInstance *inst = (ControllerInstance *)calloc(1, sizeof(*inst));
    if (!inst)
        return NULL;

    inst->id = 7;
    inst->team = CONTROLLER_TEAM_BANANA;
    inst->position[0] = x;
    inst->position[1] = y;
    inst->position[2] = z;
    strncpy(inst->type_name, "fake", 63);
    inst->update = fake_update;
    inst->on_signal = fake_signal;
    inst->destroy = fake_destroy;
    return inst;
}

static int test_controller_factory_and_system(void)
{
    ControllerSystem *sys = NULL;
    ControllerInstance *inst = NULL;
    int signal = 4;

    g_updates = 0;
    g_signals = 0;
    g_destroys = 0;

    controller_register("fake", fake_factory);
    sys = controller_system_create();
    if (!sys)
        return 1;

    inst = controller_system_create_instance(sys, "fake", 1.0f, 2.0f, 3.0f);
    if (!inst || inst->id != 7 || inst->team != CONTROLLER_TEAM_BANANA)
    {
        controller_system_destroy(sys);
        return 1;
    }

    controller_system_update(sys, 5.0f);
    if (g_updates != 5)
    {
        controller_system_destroy(sys);
        return 1;
    }

    controller_system_signal(sys, inst->id, "pulse", &signal);
    if (g_signals != 4)
    {
        controller_system_destroy(sys);
        return 1;
    }

    controller_system_destroy_instance(sys, inst->id);
    if (g_destroys != 1)
    {
        controller_system_destroy(sys);
        return 1;
    }

    controller_system_destroy(sys);
    return 0;
}

int main(void)
{
    return test_controller_factory_and_system();
}
#endif

int main(void)
{
    return test_controller_factory_and_system();
}
