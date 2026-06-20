#include "runtime/engine/composition/engine_composition_shutdown.h"
#include "ai/controller.h"
#include "ai/npc_merchant.h"

#include "../../support/test_support.h"

#include <stdlib.h>
#include <string.h>

static int g_shutdown_destroy_calls = 0;

static void fake_controller_destroy(ControllerInstance *self)
{
    (void)self;
    g_shutdown_destroy_calls++;
}

static void test_engine_composition_shutdown_rejects_null_state(void **state)
{
    (void)state;

    runtime_engine_composition_shutdown_state(NULL);

    BANANA_TEST_ASSERT_TRUE(1,
                            "engine composition shutdown must tolerate null state");
}

static void test_engine_composition_shutdown_cleans_controllers_and_merchants(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;
    ControllerInstance *controller = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    int merchant_id = -1;

    BANANA_TEST_ASSERT_TRUE(controller != NULL,
                            "test must allocate a heap controller for lifecycle destroy coverage");

    memset(&runtime_state, 0, sizeof(runtime_state));
    controller->destroy = fake_controller_destroy;

    runtime_state.controllers[0] = controller;
    runtime_state.controller_count = 1;
    runtime_state.merchants_seeded = 1;

    BANANA_TEST_ASSERT_INT_EQ(npc_merchant_init(), 0,
                              "merchant registry init must succeed for shutdown branch coverage");
    merchant_id = npc_merchant_register(7, 4.0f, 5.0f, 100);
    BANANA_TEST_ASSERT_TRUE(merchant_id >= 0,
                            "merchant registration must succeed before shutdown");
    BANANA_TEST_ASSERT_INT_EQ(npc_merchant_count(), 1,
                              "merchant registry should contain a seeded merchant before shutdown");

    g_shutdown_destroy_calls = 0;
    runtime_engine_composition_shutdown_state(&runtime_state);

    BANANA_TEST_ASSERT_INT_EQ(g_shutdown_destroy_calls, 1,
                              "shutdown must destroy active controllers");
    BANANA_TEST_ASSERT_TRUE(runtime_state.controllers[0] == NULL,
                            "shutdown must clear controller slots after destruction");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.controller_count, 0,
                              "shutdown must reset controller count");
    BANANA_TEST_ASSERT_INT_EQ(npc_merchant_count(), 0,
                              "shutdown must clear seeded merchants when merchant seeding is enabled");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_composition_shutdown_rejects_null_state),
    BANANA_TEST_CASE(test_engine_composition_shutdown_cleans_controllers_and_merchants)
)
