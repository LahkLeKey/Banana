#include "runtime/merchant/query/merchant_query_abi.h"

#include "../../support/test_support.h"

#include <string.h>

static int g_get_price_status = 0;
static int g_get_price_value = 0;
static int g_get_price_calls = 0;

static int test_get_price(int npc_id,
                          const char *item_type,
                          int *inout_seeded,
                          int *out_price)
{
    (void)npc_id;
    (void)item_type;
    (void)inout_seeded;

    g_get_price_calls += 1;
    if (g_get_price_status != 0)
        return g_get_price_status;

    *out_price = g_get_price_value;
    return 0;
}

static void test_returns_zero_when_ports_or_inputs_are_invalid(void **state)
{
    RuntimeApplicationServicePorts ports;
    int seeded = 0;

    (void)state;

    memset(&ports, 0, sizeof(ports));
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_query_abi_get_price(NULL, 5, "apple", &seeded),
                              0,
                              "null ports must return zero");

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_query_abi_get_price(&ports, 5, "apple", &seeded),
                              0,
                              "missing get_price callback must return zero");

    ports.merchant.get_price = test_get_price;
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_query_abi_get_price(&ports, 5, NULL, &seeded),
                              0,
                              "null item type must return zero");
}

static void test_returns_zero_when_price_lookup_reports_error(void **state)
{
    RuntimeApplicationServicePorts ports;
    int seeded = 1;

    (void)state;

    memset(&ports, 0, sizeof(ports));
    ports.merchant.get_price = test_get_price;

    g_get_price_calls = 0;
    g_get_price_status = -1;
    g_get_price_value = 77;

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_query_abi_get_price(&ports, 9, "banana", &seeded),
                              0,
                              "merchant callback errors must map to zero");
    BANANA_TEST_ASSERT_INT_EQ(g_get_price_calls,
                              1,
                              "merchant callback should be invoked for valid inputs");
}

static void test_returns_callback_price_on_success(void **state)
{
    RuntimeApplicationServicePorts ports;
    int seeded = 1;

    (void)state;

    memset(&ports, 0, sizeof(ports));
    ports.merchant.get_price = test_get_price;

    g_get_price_calls = 0;
    g_get_price_status = 0;
    g_get_price_value = 314;

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_query_abi_get_price(&ports, 4, "pbj", &seeded),
                              314,
                              "successful merchant price lookups should return callback price");
    BANANA_TEST_ASSERT_INT_EQ(g_get_price_calls,
                              1,
                              "merchant callback should be called exactly once");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_returns_zero_when_ports_or_inputs_are_invalid),
    BANANA_TEST_CASE(test_returns_zero_when_price_lookup_reports_error),
    BANANA_TEST_CASE(test_returns_callback_price_on_success)
)
