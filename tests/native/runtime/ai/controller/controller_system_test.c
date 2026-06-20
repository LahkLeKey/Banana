#include "ai/controller.h"
#include "ai/controller_system.h"
#include "../../support/test_support.h"
#include <stdlib.h>
#include <string.h>

static int g_updates = 0;
static int g_signals = 0;
static int g_destroys = 0;
static int g_alpha_signals = 0;
static int g_beta_signals = 0;

static void fake_update(ControllerInstance *self, float dt)
{
    (void)self;
    g_updates += (int)dt;
}

static void fake_signal(ControllerInstance *self, const char *signal, const void *data)
{
    if (!signal || !data || strcmp(signal, "pulse") != 0)
        return;

    g_signals += *(const int *)data;

    if (self && strcmp(self->type_name, "alpha") == 0)
        g_alpha_signals += *(const int *)data;
    else if (self && strcmp(self->type_name, "beta") == 0)
        g_beta_signals += *(const int *)data;
}

static void fake_destroy(ControllerInstance *self)
{
    (void)self;
    g_destroys += 1;
}

static ControllerInstance *make_controller_instance(const char *type_name, float x, float y, float z)
{
    ControllerInstance *inst = (ControllerInstance *)calloc(1, sizeof(*inst));
    if (!inst)
        return NULL;

    inst->id = 7;
    inst->team = CONTROLLER_TEAM_BANANA;
    inst->position[0] = x;
    inst->position[1] = y;
    inst->position[2] = z;
    strncpy(inst->type_name, type_name, 63);
    inst->update = fake_update;
    inst->on_signal = fake_signal;
    inst->destroy = fake_destroy;
    return inst;
}

static ControllerInstance *fake_factory(float x, float y, float z)
{
    return make_controller_instance("fake", x, y, z);
}

static ControllerInstance *alpha_factory(float x, float y, float z)
{
    return make_controller_instance("alpha", x, y, z);
}

static ControllerInstance *beta_factory(float x, float y, float z)
{
    return make_controller_instance("beta", x, y, z);
}

static void test_controller_factory_and_system(void **state)
{
    (void)state;
    ControllerSystem *sys = NULL;
    int signal = 4;

    g_updates = 0;
    g_signals = 0;
    g_destroys = 0;
    g_alpha_signals = 0;
    g_beta_signals = 0;

    controller_register("fake", fake_factory);
    sys = controller_system_create();
    BANANA_TEST_ASSERT_TRUE(sys != NULL, "controller system must allocate");

    BANANA_TEST_ASSERT_INT_EQ(controller_system_spawn(sys, "fake", 1.0f, 2.0f, 3.0f), 1,
                              "first spawned controller id should be 1");
    BANANA_TEST_ASSERT_INT_EQ(sys->count, 1, "controller system should contain one instance");

    controller_system_update(sys, 5.0f);
    BANANA_TEST_ASSERT_INT_EQ(g_updates, 5, "controller update callback should receive dt");

    controller_system_signal_all(sys, "pulse", &signal);
    BANANA_TEST_ASSERT_INT_EQ(g_signals, 4, "signal-all should fan out once");

    controller_system_destroy(sys);
}

static void test_controller_signal_type_filters_by_controller_type(void **state)
{
    (void)state;
    ControllerSystem *sys = NULL;
    int signal = 3;

    g_updates = 0;
    g_signals = 0;
    g_destroys = 0;
    g_alpha_signals = 0;
    g_beta_signals = 0;

    controller_register("alpha", alpha_factory);
    controller_register("beta", beta_factory);
    sys = controller_system_create();
    BANANA_TEST_ASSERT_TRUE(sys != NULL, "controller system must allocate for type-filter test");

    BANANA_TEST_ASSERT_INT_EQ(controller_system_spawn(sys, "alpha", 1.0f, 2.0f, 3.0f), 1,
                              "alpha instance should receive id 1");
    BANANA_TEST_ASSERT_INT_EQ(controller_system_spawn(sys, "beta", 4.0f, 5.0f, 6.0f), 2,
                              "beta instance should receive id 2");
    BANANA_TEST_ASSERT_INT_EQ(sys->count, 2, "controller system should contain two instances");

    controller_system_signal_type(sys, "alpha", "pulse", &signal);
    BANANA_TEST_ASSERT_INT_EQ(g_alpha_signals, 3, "typed signal should reach matching type");
    BANANA_TEST_ASSERT_INT_EQ(g_beta_signals, 0, "typed signal should not reach non-matching type");

    controller_system_destroy(sys);
}

static void test_controller_system_destroy_tears_down_instances(void **state)
{
    (void)state;
    ControllerSystem *sys = NULL;
    uint32_t first_id;
    uint32_t second_id;

    g_updates = 0;
    g_signals = 0;
    g_destroys = 0;
    g_alpha_signals = 0;
    g_beta_signals = 0;

    controller_register("alpha", alpha_factory);
    controller_register("beta", beta_factory);
    sys = controller_system_create();
    BANANA_TEST_ASSERT_TRUE(sys != NULL, "controller system must allocate for teardown test");

    first_id = controller_system_spawn(sys, "alpha", 1.0f, 2.0f, 3.0f);
    second_id = controller_system_spawn(sys, "beta", 4.0f, 5.0f, 6.0f);
    BANANA_TEST_ASSERT_INT_EQ(first_id, 1, "first spawned controller should receive id 1");
    BANANA_TEST_ASSERT_INT_EQ(second_id, 2, "second spawned controller should receive id 2");
    BANANA_TEST_ASSERT_INT_EQ(sys->count, 2, "teardown test should create two instances");

    controller_system_destroy(sys);

    BANANA_TEST_ASSERT_INT_EQ(g_destroys, 2, "destroying the system should invoke the destroy callback for each instance");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_controller_factory_and_system),
    BANANA_TEST_CASE(test_controller_signal_type_filters_by_controller_type),
    BANANA_TEST_CASE(test_controller_system_destroy_tears_down_instances)
)
