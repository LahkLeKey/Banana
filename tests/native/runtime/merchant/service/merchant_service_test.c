#include "runtime/merchant/service/merchant_service.h"
#include "ai/npc_merchant.h"
#include "runtime/merchant/trade/merchant_trade_domain.h"

#include "runtime/support/test_support.h"

static int g_init_calls = 0;
static int g_register_calls = 0;
static int g_add_item_calls = 0;
static int g_price_calls = 0;
static int g_buy_calls = 0;
static int g_sell_calls = 0;

int npc_merchant_init(void) { g_init_calls++; return 0; }
int npc_merchant_count(void) { return g_register_calls; }
int npc_merchant_register(int merchant_id, float x, float y, int gold) { (void)x; (void)y; (void)gold; g_register_calls++; return merchant_id; }
int npc_merchant_add_inventory_item(int merchant_id, const char *item_name, int quantity, int max_stock, int price_gold) { (void)merchant_id; (void)item_name; (void)quantity; (void)max_stock; (void)price_gold; g_add_item_calls++; return 0; }
int runtime_resource_parse_key(const char *resource_type, RuntimeResourceKey *out_key) { if (!resource_type || !out_key) return -1; if (resource_type[0]=='w') *out_key = RUNTIME_RESOURCE_WOOD; else if (resource_type[0]=='o') *out_key = RUNTIME_RESOURCE_ORE; else if (resource_type[0]=='g') *out_key = RUNTIME_RESOURCE_GOLD; else return -1; return 0; }
const char *runtime_resource_key_name(RuntimeResourceKey resource_key) { if (resource_key==RUNTIME_RESOURCE_WOOD) return "wood"; if (resource_key==RUNTIME_RESOURCE_ORE) return "ore"; if (resource_key==RUNTIME_RESOURCE_GOLD) return "gold"; return "unknown"; }
int npc_merchant_get_item_price(int merchant_id, const char *item_type, int *out_stock, int *out_price) { (void)merchant_id; (void)item_type; g_price_calls++; if (out_stock) *out_stock = 5; if (out_price) *out_price = 42; return 0; }

MerchantTradeResult npc_merchant_trade_buy(int merchant_id, const char *item_name, int quantity, int *player_gold) { (void)merchant_id; (void)item_name; (void)quantity; (void)player_gold; g_buy_calls++; return MERCHANT_TRADE_OK; }
MerchantTradeResult npc_merchant_trade_sell(int merchant_id, const char *item_name, int quantity, int *player_gold) { (void)merchant_id; (void)item_name; (void)quantity; (void)player_gold; g_sell_calls++; return MERCHANT_TRADE_OK; }
int runtime_merchant_map_buy_status(MerchantTradeResult result) { return (int)result + 10; }
int runtime_merchant_map_sell_status(MerchantTradeResult result) { return (int)result + 20; }

static int stub_get(void *context, RuntimeResourceKey resource_key) { (void)context; (void)resource_key; return 100; }
static int stub_set_total(void *context, RuntimeResourceKey resource_key, int target_amount) { (void)context; (void)resource_key; return target_amount; }
static int stub_add(void *context, RuntimeResourceKey resource_key, int amount) { (void)context; (void)resource_key; return amount; }

static RuntimeMerchantResourceGateway gateway(void)
{
    RuntimeMerchantResourceGateway g = {0};
    g.get = stub_get;
    g.set_total = stub_set_total;
    g.add = stub_add;
    return g;
}

static void reset_calls(void)
{
    g_init_calls = g_register_calls = g_add_item_calls = 0;
    g_price_calls = g_buy_calls = g_sell_calls = 0;
}

static void test_price_paths(void **state)
{
    (void)state;
    int seeded = 0;
    int price = 0;
    reset_calls();

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_get_price(1, NULL, &seeded, &price), -1,
                              "null item_type must fail");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_get_price(1, "wood", &seeded, NULL), -1,
                              "null out_price must fail");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_get_price(1, "invalid", &seeded, &price), -1,
                              "invalid resource type must fail");

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_get_price(1, "wood", &seeded, &price), 0,
                              "valid get_price must succeed");
    BANANA_TEST_ASSERT_INT_EQ(price, 42,
                              "get_price must expose merchant price");
    BANANA_TEST_ASSERT_INT_EQ(g_init_calls, 1,
                              "seed helper must initialize merchants once");
    BANANA_TEST_ASSERT_TRUE(g_register_calls >= 2,
                            "seed helper must register merchants up to npc_id");
}

static void test_trade_paths(void **state)
{
    (void)state;
    int seeded = 0;
    RuntimeMerchantResourceGateway g = gateway();
    reset_calls();

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_trade_buy(1, NULL, 1, &seeded, g), -1,
                              "null item_type must fail buy");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_trade_buy(1, "wood", 0, &seeded, g), -1,
                              "non-positive buy quantity must fail");
    RuntimeMerchantResourceGateway bad = {0};
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_trade_buy(1, "wood", 1, &seeded, bad), -1,
                              "missing buy gateway callbacks must fail");

    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_trade_buy(1, "wood", 2, &seeded, g), 10,
                              "valid buy must forward to trade domain");
    BANANA_TEST_ASSERT_INT_EQ(runtime_merchant_service_trade_sell(1, "ore", 3, &seeded, g), 20,
                              "valid sell must forward to trade domain");
    BANANA_TEST_ASSERT_INT_EQ(g_buy_calls, 1,
                              "buy domain must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_sell_calls, 1,
                              "sell domain must be called once");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_price_paths),
    BANANA_TEST_CASE(test_trade_paths)
)
