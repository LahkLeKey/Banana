#include "ai/npc_merchant.h"
#include "runtime/support/test_support.h"

static void test_merchant_register_and_trade_flow(void **state)
{
    (void)state;
    int merchant_id = -1;
    int player_gold = 25;
    MerchantTradeResult result = MERCHANT_TRADE_OK;
    int quantity = 0;
    int price = 0;

    npc_merchant_init();
    merchant_id = npc_merchant_register(7, 1.0f, 2.0f, 50);
    BANANA_TEST_ASSERT_TRUE(merchant_id >= 0, "merchant registration must succeed");

    BANANA_TEST_ASSERT_INT_EQ(npc_merchant_add_inventory_item(merchant_id, "wood", 4, 8, 5), 0,
                              "merchant inventory item must be added");
    BANANA_TEST_ASSERT_INT_EQ(npc_merchant_get_item_price(merchant_id, "wood", &quantity, &price), 0,
                              "merchant inventory lookup must succeed");
    BANANA_TEST_ASSERT_INT_EQ(quantity, 4,
                              "merchant inventory lookup must return registered stock");
    BANANA_TEST_ASSERT_INT_EQ(price, 5,
                              "merchant inventory lookup must return registered price");

    result = npc_merchant_trade_buy(merchant_id, "wood", 2, &player_gold);
    BANANA_TEST_ASSERT_INT_EQ((int)result, (int)MERCHANT_TRADE_OK,
                              "merchant trade buy must succeed when player can afford it");
    BANANA_TEST_ASSERT_INT_EQ(player_gold, 15,
                              "merchant trade buy must deduct gold from player");

    npc_merchant_shutdown();
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_merchant_register_and_trade_flow)
)
