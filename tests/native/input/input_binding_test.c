/**
 * @file input_binding_test.c
 * @brief Unit tests for input binding system
 *
 * Tests event dispatch, callback registration, and routing.
 */

#include "runtime/input/binding/input_binding.h"
#include <stdio.h>
#include <string.h>

/* Test counters */
static int tests_run = 0;
static int tests_passed = 0;

#define ASSERT_EQ(actual, expected, msg)                                                       \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if ((actual) == (expected))                                                            \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected %d, got %d)\n", msg, expected, actual);                \
        }                                                                                      \
    } while (0)

/* Mock callback that counts invocations */
static int mock_callback_count = 0;
static int mock_proximity_callback_count = 0;
static int mock_exit_count = 0;
static int mock_trade_count = 0;
static int mock_callback1_count = 0;
static int mock_callback2_count = 0;
static ClickEventData last_click_event = {0};

uint32_t mock_click_callback(const void *event_data, uint32_t *controller_ids, int max_controllers)
{
    if (!event_data || !controller_ids || max_controllers < 1)
    {
        return 0;
    }

    mock_callback_count++;
    const ClickEventData *click = (const ClickEventData *)event_data;
    last_click_event = *click;

    /* Return 1 mock controller */
    controller_ids[0] = 100;
    return 1;
}

uint32_t mock_proximity_callback(const void *event_data, uint32_t *controller_ids,
                                 int max_controllers)
{
    mock_proximity_callback_count++;
    if (max_controllers >= 1)
    {
        controller_ids[0] = 200;
        return 1;
    }
    return 0;
}

uint32_t mock_exit_callback(const void *event_data, uint32_t *controller_ids, int max_controllers)
{
    mock_exit_count++;
    return 0;
}

uint32_t mock_trade_callback(const void *event_data, uint32_t *controller_ids, int max_controllers)
{
    mock_trade_count++;
    const TradeEventData *trade = (const TradeEventData *)event_data;
    if (max_controllers >= 1 && strcmp(trade->trade_type, "buy") == 0)
    {
        controller_ids[0] = 300;
        return 1;
    }
    return 0;
}

uint32_t mock_callback1(const void *event_data, uint32_t *controller_ids, int max_controllers)
{
    mock_callback1_count++;
    if (max_controllers >= 1)
    {
        controller_ids[0] = 101;
        return 1;
    }
    return 0;
}

uint32_t mock_callback2(const void *event_data, uint32_t *controller_ids, int max_controllers)
{
    mock_callback2_count++;
    if (max_controllers >= 1)
    {
        controller_ids[0] = 102;
        return 1;
    }
    return 0;
}

/**
 * Test: Initialize binding system
 */
void test_init(void)
{
    int ret = input_binding_init();
    ASSERT_EQ(ret, 0, "input_binding_init should succeed");
}

/**
 * Test: Register callback
 */
void test_register_callback(void)
{
    input_binding_init();
    int ret = input_binding_register_callback(INPUT_EVENT_CLICK, mock_click_callback);
    ASSERT_EQ(ret, 0, "Register callback should succeed");
    input_binding_cleanup();
}

/**
 * Test: Dispatch click event
 */
void test_dispatch_click(void)
{
    input_binding_init();
    mock_callback_count = 0;

    input_binding_register_callback(INPUT_EVENT_CLICK, mock_click_callback);

    int ret = input_binding_dispatch_click(10.5f, 20.5f, "player123");
    ASSERT_EQ(ret, 1, "Click dispatch should return 1 affected controller");
    ASSERT_EQ(mock_callback_count, 1, "Callback should be invoked once");

    /* Verify event data was passed correctly */
    ASSERT_EQ((int)last_click_event.world_x, 10, "world_x should be ~10.5");
    ASSERT_EQ((int)last_click_event.world_z, 20, "world_z should be ~20.5");

    input_binding_cleanup();
}

/**
 * Test: Dispatch proximity enter
 */
void test_dispatch_proximity_enter(void)
{
    input_binding_init();
    mock_proximity_callback_count = 0;

    input_binding_register_callback(INPUT_EVENT_PROXIMITY_ENTER, mock_proximity_callback);

    int ret = input_binding_dispatch_proximity("player456", 5.0f, 10.0f, 1);
    ASSERT_EQ(ret, 1, "Proximity enter should dispatch 1 event");

    input_binding_cleanup();
}

/**
 * Test: Dispatch proximity exit
 */
void test_dispatch_proximity_exit(void)
{
    input_binding_init();
    mock_exit_count = 0;

    input_binding_register_callback(INPUT_EVENT_PROXIMITY_EXIT, mock_exit_callback);

    int ret = input_binding_dispatch_proximity("player789", 15.0f, 25.0f, 0);
    ASSERT_EQ(ret, 0, "Proximity exit should dispatch (callback returns 0 controllers)");

    input_binding_cleanup();
}

/**
 * Test: Dispatch trade request
 */
void test_dispatch_trade_request(void)
{
    input_binding_init();
    mock_trade_count = 0;

    input_binding_register_callback(INPUT_EVENT_TRADE_REQUEST, mock_trade_callback);

    int ret = input_binding_dispatch_trade_request("player999", 42, "buy");
    ASSERT_EQ(ret, 1, "Trade request should dispatch 1 event");

    input_binding_cleanup();
}

/**
 * Test: Multiple callbacks for same event type
 */
void test_multiple_callbacks(void)
{
    input_binding_init();
    mock_callback1_count = 0;
    mock_callback2_count = 0;

    input_binding_register_callback(INPUT_EVENT_CLICK, mock_callback1);
    input_binding_register_callback(INPUT_EVENT_CLICK, mock_callback2);

    int ret = input_binding_dispatch_click(1.0f, 2.0f, "player111");
    ASSERT_EQ(ret, 2, "Both callbacks should be invoked");

    input_binding_cleanup();
}

/**
 * Main test runner
 */
int main(void)
{
    printf("=== Input Binding Tests ===\n\n");

    test_init();
    test_register_callback();
    test_dispatch_click();
    test_dispatch_proximity_enter();
    test_dispatch_proximity_exit();
    test_dispatch_trade_request();
    test_multiple_callbacks();

    printf("\n=== Results ===\n");
    printf("Passed: %d / %d\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
