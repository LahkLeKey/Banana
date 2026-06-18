#include "ai/npc_merchant.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

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
    assert_true(merchant_id >= 0);

    assert_int_equal(npc_merchant_add_inventory_item(merchant_id, "wood", 4, 8, 5), 0);
    assert_int_equal(npc_merchant_get_item_price(merchant_id, "wood", &quantity, &price), 0);
    assert_int_equal(quantity, 4);
    assert_int_equal(price, 5);

    result = npc_merchant_trade_buy(merchant_id, "wood", 2, &player_gold);
    assert_int_equal((int)result, (int)MERCHANT_TRADE_OK);
    assert_int_equal(player_gold, 15);

    npc_merchant_shutdown();
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_merchant_register_and_trade_flow),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>

static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

static int test_merchant_register_and_trade_flow(void)
{
    int merchant_id = -1;
    int player_gold = 25;
    MerchantTradeResult result = MERCHANT_TRADE_OK;
    int quantity = 0;
    int price = 0;

    npc_merchant_init();
    merchant_id = npc_merchant_register(7, 1.0f, 2.0f, 50);
    if (fail_if(merchant_id < 0, "merchant registration must succeed"))
        return 1;

    if (fail_if(npc_merchant_add_inventory_item(merchant_id, "wood", 4, 8, 5) != 0,
                "merchant inventory item must be added"))
        return 1;

    if (fail_if(npc_merchant_get_item_price(merchant_id, "wood", &quantity, &price) != 0,
                "merchant inventory lookup must succeed") ||
        fail_if(quantity != 4 || price != 5,
                "merchant inventory lookup must return the registered stock and price"))
        return 1;

    result = npc_merchant_trade_buy(merchant_id, "wood", 2, &player_gold);
    if (fail_if(result != MERCHANT_TRADE_OK,
                "merchant trade buy must succeed when player can afford it") ||
        fail_if(player_gold != 15,
                "merchant trade buy must deduct gold from the player"))
        return 1;

    npc_merchant_shutdown();
    return 0;
}

int main(void)
{
    return test_merchant_register_and_trade_flow();
}
#endif
