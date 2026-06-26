#include "runtime/input/binding/input_binding.h"

#include "runtime/support/test_support.h"

#include <stdint.h>

static int g_callback_calls = 0;
static InputEventType g_last_event_type;
static uint32_t g_last_controller_id = 0;

static uint32_t stub_callback(const void *event_data,
                               uint32_t *controller_ids,
                               int max_controllers)
{
    (void)event_data;
    (void)max_controllers;
    g_callback_calls += 1;
    if (controller_ids)
        controller_ids[0] = 77;
    return 1;
}

static void test_input_binding_init_and_cleanup(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_INT_EQ(input_binding_init(),
                              0,
                              "init must succeed");
    input_binding_cleanup();
    BANANA_TEST_ASSERT_INT_EQ(input_binding_init(),
                              0,
                              "re-init after cleanup must succeed");
}

static void test_register_callback_validates_event_type(void **state)
{
    (void)state;
    input_binding_init();

    BANANA_TEST_ASSERT_INT_EQ(input_binding_register_callback(0, stub_callback),
                              -1,
                              "invalid event type 0 must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(input_binding_register_callback(99, stub_callback),
                              -1,
                              "out-of-range event type must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(input_binding_register_callback(INPUT_EVENT_CLICK, NULL),
                              -1,
                              "null callback must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(input_binding_register_callback(INPUT_EVENT_CLICK, stub_callback),
                              0,
                              "valid click callback registration must succeed");
    BANANA_TEST_ASSERT_INT_EQ(input_binding_register_callback(INPUT_EVENT_PROXIMITY_ENTER, stub_callback),
                              0,
                              "proximity enter callback registration must succeed");
    BANANA_TEST_ASSERT_INT_EQ(input_binding_register_callback(INPUT_EVENT_PROXIMITY_EXIT, stub_callback),
                              0,
                              "proximity exit callback registration must succeed");
    BANANA_TEST_ASSERT_INT_EQ(input_binding_register_callback(INPUT_EVENT_TRADE_REQUEST, stub_callback),
                              0,
                              "trade request callback registration must succeed");

    input_binding_cleanup();
}

static void test_dispatch_calls_registered_callbacks(void **state)
{
    (void)state;
    g_callback_calls = 0;

    input_binding_init();
    input_binding_register_callback(INPUT_EVENT_CLICK, stub_callback);

    input_binding_dispatch_click(10.0f, 20.0f, "player-a");
    BANANA_TEST_ASSERT_INT_EQ(g_callback_calls,
                              1,
                              "dispatch click must invoke registered callback");

    input_binding_dispatch_proximity("player-a", 5.0f, 7.0f, 1);
    input_binding_dispatch_proximity("player-a", 5.0f, 7.0f, 0);
    input_binding_dispatch_trade_request("player-a", 3, "buy");

    /* null/empty guard paths */
    input_binding_dispatch_click(0.0f, 0.0f, NULL);
    input_binding_dispatch_proximity(NULL, 0.0f, 0.0f, 1);
    input_binding_dispatch_trade_request(NULL, 0, "buy");

    input_binding_cleanup();
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_input_binding_init_and_cleanup),
    BANANA_TEST_CASE(test_register_callback_validates_event_type),
    BANANA_TEST_CASE(test_dispatch_calls_registered_callbacks)
)
