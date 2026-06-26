#include "ai/controller_system.h"
#include "runtime/support/test_support.h"

#include <string.h>

static int g_dispatch_updates = 0;
static int g_dispatch_signals = 0;
static int g_dispatch_alpha_signals = 0;
static int g_dispatch_beta_signals = 0;

static void fake_update(ControllerInstance *self, float dt)
{
    (void)self;
    g_dispatch_updates += (int)dt;
}

static void fake_signal(ControllerInstance *self, const char *signal, const void *data)
{
    if (!self || !signal || !data || strcmp(signal, "pulse") != 0)
        return;

    g_dispatch_signals += *(const int *)data;

    if (strncmp(self->type_name, "alpha", 63) == 0)
        g_dispatch_alpha_signals += *(const int *)data;
    else if (strncmp(self->type_name, "beta", 63) == 0)
        g_dispatch_beta_signals += *(const int *)data;
}

static void fake_destroy(ControllerInstance *self)
{
    (void)self;
}

static ControllerInstance make_instance(const char *type_name)
{
    ControllerInstance instance;
    memset(&instance, 0, sizeof(instance));
    strncpy(instance.type_name, type_name, 63);
    instance.update = fake_update;
    instance.on_signal = fake_signal;
    instance.destroy = fake_destroy;
    return instance;
}

static void test_controller_system_dispatch_rejects_null_system(void **state)
{
    (void)state;

    g_dispatch_updates = 0;
    g_dispatch_signals = 0;
    g_dispatch_alpha_signals = 0;
    g_dispatch_beta_signals = 0;

    controller_system_update(NULL, 4.0f);
    controller_system_signal_all(NULL, "pulse", NULL);
    controller_system_signal_type(NULL, "alpha", "pulse", NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_updates,
                              0,
                              "update dispatch must no-op for null systems");
    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_signals,
                              0,
                              "signal dispatch must no-op for null systems");
}

static void test_controller_system_signal_type_null_type_broadcasts_all(void **state)
{
    (void)state;
    ControllerSystem system;
    ControllerInstance alpha = make_instance("alpha");
    ControllerInstance beta = make_instance("beta");
    int signal = 3;

    memset(&system, 0, sizeof(system));
    system.instances[0] = &alpha;
    system.instances[1] = &beta;
    system.count = 2;

    g_dispatch_updates = 0;
    g_dispatch_signals = 0;
    g_dispatch_alpha_signals = 0;
    g_dispatch_beta_signals = 0;

    controller_system_signal_type(&system, NULL, "pulse", &signal);

    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_signals,
                              6,
                              "null type filter must broadcast to all controller instances");
    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_alpha_signals,
                              3,
                              "alpha controller must receive broadcasted signal");
    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_beta_signals,
                              3,
                              "beta controller must receive broadcasted signal");
}

static void test_controller_system_dispatch_updates_and_filters_by_type(void **state)
{
    (void)state;
    ControllerSystem system;
    ControllerInstance alpha = make_instance("alpha");
    ControllerInstance beta = make_instance("beta");
    int pulse = 2;
    int alpha_only = 5;

    memset(&system, 0, sizeof(system));
    system.instances[0] = &alpha;
    system.instances[1] = &beta;
    system.count = 2;

    g_dispatch_updates = 0;
    g_dispatch_signals = 0;
    g_dispatch_alpha_signals = 0;
    g_dispatch_beta_signals = 0;

    controller_system_update(&system, 2.0f);
    controller_system_signal_all(&system, "pulse", &pulse);
    controller_system_signal_type(&system, "alpha", "pulse", &alpha_only);

    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_updates,
                              4,
                              "update dispatch must call each controller update callback");
    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_signals,
                              9,
                              "signal dispatch must include broadcast and filtered deliveries");
    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_alpha_signals,
                              7,
                              "typed signal dispatch must deliver to matching controllers");
    BANANA_TEST_ASSERT_INT_EQ(g_dispatch_beta_signals,
                              2,
                              "typed signal dispatch must not deliver to non-matching controllers");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_controller_system_dispatch_rejects_null_system),
    BANANA_TEST_CASE(test_controller_system_signal_type_null_type_broadcasts_all),
    BANANA_TEST_CASE(test_controller_system_dispatch_updates_and_filters_by_type)
)
