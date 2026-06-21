#include "runtime/merchant/trade/merchant_trade_abi.h"

#include "../../support/test_support.h"

#include <string.h>

static int g_trade_buy_result = 0;
static int g_trade_sell_result = 0;
static int g_trade_buy_calls = 0;
static int g_trade_sell_calls = 0;

static int fake_trade_buy(int npc_id,
                          const char *item_type,
                          int quantity,
                          int *inout_seeded,
                          RuntimeMerchantResourceGateway resource_gateway)
{
    (void)npc_id;
    (void)item_type;
    (void)quantity;
    (void)inout_seeded;
    (void)resource_gateway;
    g_trade_buy_calls += 1;
    return g_trade_buy_result;
}

static int fake_trade_sell(int npc_id,
                           const char *item_type,
                           int quantity,
                           int *inout_seeded,
                           RuntimeMerchantResourceGateway resource_gateway)
{
    (void)npc_id;
    (void)item_type;
    (void)quantity;
    (void)inout_seeded;
    (void)resource_gateway;
    g_trade_sell_calls += 1;
    return g_trade_sell_result;
}

static void test_buy_validates_inputs_before_callback(void **state)
{
    RuntimeApplicationServicePorts ports;
    RuntimeMerchantResourceGateway gateway;
    int seeded = 0;

    (void)state;

    memset(&ports, 0, sizeof(ports));
    memset(&gateway, 0, sizeof(gateway));
    g_trade_buy_calls = 0;

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_buy(NULL, 1, 1, "wood", 1, &seeded, gateway),
                              -1,
                              "buy should reject null ports");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_buy(&ports, 0, 1, "wood", 1, &seeded, gateway),
                              -1,
                              "buy should reject world_not_ready");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_buy(&ports, 1, 1, NULL, 1, &seeded, gateway),
                              -1,
                              "buy should reject null item type");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_buy(&ports, 1, 1, "wood", 0, &seeded, gateway),
                              -1,
                              "buy should reject non-positive quantities");
    BANANA_TEST_ASSERT_INT_EQ(g_trade_buy_calls,
                              0,
                              "buy callback should not be called for rejected inputs");
}

static void test_sell_validates_inputs_before_callback(void **state)
{
    RuntimeApplicationServicePorts ports;
    RuntimeMerchantResourceGateway gateway;
    int seeded = 0;

    (void)state;

    memset(&ports, 0, sizeof(ports));
    memset(&gateway, 0, sizeof(gateway));
    g_trade_sell_calls = 0;

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_sell(NULL, 1, 1, "wood", 1, &seeded, gateway),
                              -1,
                              "sell should reject null ports");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_sell(&ports, 0, 1, "wood", 1, &seeded, gateway),
                              -1,
                              "sell should reject world_not_ready");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_sell(&ports, 1, 1, NULL, 1, &seeded, gateway),
                              -1,
                              "sell should reject null item type");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_sell(&ports, 1, 1, "wood", 0, &seeded, gateway),
                              -1,
                              "sell should reject non-positive quantities");
    BANANA_TEST_ASSERT_INT_EQ(g_trade_sell_calls,
                              0,
                              "sell callback should not be called for rejected inputs");
}

static void test_buy_and_sell_forward_service_result_when_valid(void **state)
{
    RuntimeApplicationServicePorts ports;
    RuntimeMerchantResourceGateway gateway;
    int seeded = 5;

    (void)state;

    memset(&ports, 0, sizeof(ports));
    memset(&gateway, 0, sizeof(gateway));

    ports.merchant.trade_buy = fake_trade_buy;
    ports.merchant.trade_sell = fake_trade_sell;

    g_trade_buy_calls = 0;
    g_trade_sell_calls = 0;
    g_trade_buy_result = 17;
    g_trade_sell_result = 23;

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_buy(&ports, 1, 8, "ore", 2, &seeded, gateway),
                              17,
                              "buy should forward merchant trade_buy return code");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_trade_abi_sell(&ports, 1, 8, "ore", 2, &seeded, gateway),
                              23,
                              "sell should forward merchant trade_sell return code");
    BANANA_TEST_ASSERT_INT_EQ(g_trade_buy_calls, 1, "buy should call merchant trade_buy once");
    BANANA_TEST_ASSERT_INT_EQ(g_trade_sell_calls, 1, "sell should call merchant trade_sell once");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_buy_validates_inputs_before_callback),
    BANANA_TEST_CASE(test_sell_validates_inputs_before_callback),
    BANANA_TEST_CASE(test_buy_and_sell_forward_service_result_when_valid)
)
