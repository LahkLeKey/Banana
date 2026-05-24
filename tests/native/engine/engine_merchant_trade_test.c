#include "../../../src/native/engine/engine.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    int rc = 0;
    int value = 0;

    if (!expect_int("pre-init get gold", engine_player_get_resource("gold"), 0))
        rc = 1;

    if (!expect_int("pre-init add gold", engine_player_add_resource("gold", 10), 0))
        rc = 1;

    if (engine_init(640, 360) != 0)
    {
        fprintf(stderr, "engine_init failed\n");
        return 1;
    }

    if (engine_player_upsert("native-default-player", "native-default-player", "human", 1) == 0)
    {
        fprintf(stderr, "engine_player_upsert failed\n");
        engine_shutdown();
        return 1;
    }

    value = engine_npc_merchant_get_price(0, "wood");
    if (!expect_int("wood price", value, 5))
        rc = 1;

    value = engine_npc_merchant_get_price(0, "ore");
    if (!expect_int("ore price", value, 15))
        rc = 1;

    value = engine_npc_merchant_get_price(0, "banana");
    if (!expect_int("unknown item price", value, 0))
        rc = 1;

    if (!expect_int("unknown resource add", engine_player_add_resource("banana", 3), 0))
        rc = 1;

    if (!expect_int("unknown resource get", engine_player_get_resource("banana"), 0))
        rc = 1;

    if (!expect_int("buy unknown item", engine_npc_merchant_trade_buy(0, "banana", 1), -1))
        rc = 1;

    if (!expect_int("buy invalid quantity", engine_npc_merchant_trade_buy(0, "wood", 0), -1))
        rc = 1;

    if (!expect_int("sell invalid quantity", engine_npc_merchant_trade_sell(0, "wood", 0), -1))
        rc = 1;

    if (!expect_int("buy insufficient gold", engine_npc_merchant_trade_buy(0, "wood", 1), -2))
        rc = 1;

    if (!expect_int("gold after seed", engine_player_add_resource("gold", 1000), 1000))
        rc = 1;

    if (!expect_int("buy wood success", engine_npc_merchant_trade_buy(0, "wood", 2), 0))
        rc = 1;

    if (!expect_int("gold after wood buy", engine_player_get_resource("gold"), 990))
        rc = 1;

    if (!expect_int("wood after buy", engine_player_get_resource("wood"), 2))
        rc = 1;

    if (!expect_int("gold top-up", engine_player_add_resource("gold", 10000), 10990))
        rc = 1;

    if (!expect_int("buy insufficient stock", engine_npc_merchant_trade_buy(0, "wood", 1000), -3))
        rc = 1;

    if (!expect_int("sell insufficient items", engine_npc_merchant_trade_sell(0, "ore", 1), -2))
        rc = 1;

    if (!expect_int("ore add", engine_player_add_resource("ore", 40), 40))
        rc = 1;

    if (!expect_int("sell insufficient merchant gold", engine_npc_merchant_trade_sell(0, "ore", 40), -4))
        rc = 1;

    if (!expect_int("sell wood success", engine_npc_merchant_trade_sell(0, "wood", 1), 0))
        rc = 1;

    if (!expect_int("wood after sell", engine_player_get_resource("wood"), 1))
        rc = 1;

    if (!expect_int("gold after sell", engine_player_get_resource("gold"), 10995))
        rc = 1;

    engine_shutdown();
    return rc;
}
