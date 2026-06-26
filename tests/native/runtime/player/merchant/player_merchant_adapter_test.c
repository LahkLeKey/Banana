#include "runtime/player/merchant/player_merchant_adapter.h"

#include "runtime/support/test_support.h"

static int g_get_calls = 0;
static int g_add_calls = 0;
static int g_buy_calls = 0;
static int g_sell_calls = 0;

const char *runtime_player_gateway_active_guid(EntityId active_player_id)
{
    return active_player_id == 0 ? NULL : "player-guid";
}

RuntimeMerchantResourceGateway runtime_player_gateway_resource_gateway(RuntimeMerchantGetResourceByKeyFn get,
                                                                      RuntimeMerchantSetResourceTotalByKeyFn set_total,
                                                                      RuntimeMerchantAddResourceByKeyFn add,
                                                                      void *context)
{
    RuntimeMerchantResourceGateway gateway = {0};
    gateway.context = context;
    gateway.get = get;
    gateway.set_total = set_total;
    gateway.add = add;
    return gateway;
}

int runtime_player_resource_abi_get(const char *player_guid, const char *resource_type)
{
    (void)resource_type;
    g_get_calls++;
    return player_guid ? 17 : -1;
}

int runtime_player_resource_abi_add(const char *player_guid, const char *resource_type, int amount)
{
    (void)resource_type;
    g_add_calls++;
    return player_guid ? amount + 1 : -1;
}

int runtime_player_resource_abi_get_by_key(const char *player_guid, RuntimeResourceKey resource_key)
{
    (void)resource_key;
    return player_guid ? 3 : -1;
}

int runtime_player_resource_abi_set_total_by_key(const char *player_guid, RuntimeResourceKey resource_key, int target_amount)
{
    (void)resource_key;
    return player_guid ? target_amount : -1;
}

int runtime_player_resource_abi_add_by_key(const char *player_guid, RuntimeResourceKey resource_key, int amount)
{
    (void)resource_key;
    return player_guid ? amount : -1;
}

int runtime_merchant_trade_abi_buy(const RuntimeApplicationServicePorts *ports, int world_ready, int npc_id, const char *item_type, int quantity, int *inout_seeded, RuntimeMerchantResourceGateway resource_gateway)
{
    (void)ports; (void)world_ready; (void)npc_id; (void)item_type; (void)quantity; (void)resource_gateway;
    if (inout_seeded) *inout_seeded = 1;
    g_buy_calls++;
    return 21;
}

int runtime_merchant_trade_abi_sell(const RuntimeApplicationServicePorts *ports, int world_ready, int npc_id, const char *item_type, int quantity, int *inout_seeded, RuntimeMerchantResourceGateway resource_gateway)
{
    (void)ports; (void)world_ready; (void)npc_id; (void)item_type; (void)quantity; (void)resource_gateway;
    if (inout_seeded) *inout_seeded = 1;
    g_sell_calls++;
    return 22;
}

static void reset_calls(void)
{
    g_get_calls = g_add_calls = g_buy_calls = g_sell_calls = 0;
}

static void test_resource_wrappers(void **state)
{
    (void)state;
    reset_calls();
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_merchant_adapter_get_resource("wood", 1), 17,
                              "resource getter must resolve player guid and forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_merchant_adapter_add_resource("wood", 5, 1), 6,
                              "resource add must resolve player guid and forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(g_get_calls, 1, "get ABI must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_add_calls, 1, "add ABI must be called once");
}

static void test_trade_wrappers(void **state)
{
    (void)state;
    reset_calls();
    int seeded = 0;
    RuntimeApplicationServicePorts ports = {0};
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_merchant_adapter_trade_buy(&ports, 1, 2, "ore", 3, &seeded, 1), 21,
                              "trade buy must forward to merchant trade ABI");
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_merchant_adapter_trade_sell(&ports, 1, 2, "ore", 3, &seeded, 1), 22,
                              "trade sell must forward to merchant trade ABI");
    BANANA_TEST_ASSERT_INT_EQ(g_buy_calls, 1, "buy ABI must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_sell_calls, 1, "sell ABI must be called once");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_resource_wrappers),
    BANANA_TEST_CASE(test_trade_wrappers)
)
