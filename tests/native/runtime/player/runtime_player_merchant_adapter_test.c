#include "runtime/player/player_merchant_adapter.h"

#include <stdio.h>

#include "runtime/player/player_resource_abi.h"
#include "runtime/player/player_registry.h"

typedef struct TradeProbe
{
    int buy_calls;
    int sell_calls;
    int observed_gold_before;
    int observed_gold_after;
    int observed_wood_after;
} TradeProbe;

static TradeProbe g_probe;

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s failed: got %d expected %d\n", label, actual, expected);
    return 0;
}

static int trade_buy_stub(int npc_id,
                          const char *item_type,
                          int quantity,
                          int *inout_seeded,
                          RuntimeMerchantResourceGateway resource_gateway)
{
    (void)npc_id;
    (void)item_type;
    (void)quantity;

    g_probe.buy_calls += 1;
    g_probe.observed_gold_before = resource_gateway.get(resource_gateway.context,
                                                        RUNTIME_RESOURCE_GOLD);
    g_probe.observed_gold_after = resource_gateway.add(resource_gateway.context,
                                                       RUNTIME_RESOURCE_GOLD,
                                                       -2);
    g_probe.observed_wood_after = resource_gateway.add(resource_gateway.context,
                                                       RUNTIME_RESOURCE_WOOD,
                                                       1);

    if (inout_seeded)
        *inout_seeded = 1;

    return 0;
}

static int trade_sell_stub(int npc_id,
                           const char *item_type,
                           int quantity,
                           int *inout_seeded,
                           RuntimeMerchantResourceGateway resource_gateway)
{
    (void)npc_id;
    (void)item_type;
    (void)quantity;

    g_probe.sell_calls += 1;
    resource_gateway.add(resource_gateway.context, RUNTIME_RESOURCE_GOLD, 3);

    if (inout_seeded)
        *inout_seeded = 1;

    return 0;
}

int main(void)
{
    RuntimeApplicationServicePorts ports = {0};
    int seeded = 0;
    int status = 0;
    int expected_player_a_gold = 2;
    int expected_player_b_gold = 5;
    int expected_player_a_wood = 0;
    int expected_player_b_wood = 0;

    runtime_player_registry_reset();
    runtime_player_registry_add_default(11, "guid-a", "PlayerA", "human", 1);
    runtime_player_registry_add_default(22, "guid-b", "PlayerB", "human", 1);

    runtime_player_resource_abi_add_by_key("guid-a", RUNTIME_RESOURCE_GOLD, 2);
    runtime_player_resource_abi_add_by_key("guid-b", RUNTIME_RESOURCE_GOLD, 5);

    if (!expect_int("get active resource", runtime_player_merchant_adapter_get_resource("gold", 22), 5))
        return 1;

    if (!expect_int("add active resource", runtime_player_merchant_adapter_add_resource("gold", 1, 22), 6))
        return 1;
    expected_player_b_gold = 6;

    ports.merchant.trade_buy = trade_buy_stub;
    status = runtime_player_merchant_adapter_trade_buy(&ports,
                                                       1,
                                                       7,
                                                       "wood",
                                                       1,
                                                       &seeded,
                                                       22);
    if (!expect_int("buy status", status, 0))
        return 1;
    if (!expect_int("buy call count", g_probe.buy_calls, 1))
        return 1;
    if (!expect_int("buy observed selected player gold", g_probe.observed_gold_before, 6))
        return 1;
    if (!expect_int("buy gold after mutation", g_probe.observed_gold_after, 4))
        return 1;
    if (!expect_int("buy wood after mutation", g_probe.observed_wood_after, 1))
        return 1;
    expected_player_b_gold -= 2;
    expected_player_b_wood += 1;

    ports.merchant.trade_sell = trade_sell_stub;
    status = runtime_player_merchant_adapter_trade_sell(&ports,
                                                        1,
                                                        7,
                                                        "wood",
                                                        1,
                                                        &seeded,
                                                        22);
    if (!expect_int("sell status", status, 0))
        return 1;
    if (!expect_int("sell call count", g_probe.sell_calls, 1))
        return 1;

    if (!expect_int("sell gold mutation", runtime_player_merchant_adapter_get_resource("gold", 22), 7))
        return 1;
    expected_player_b_gold += 3;

    if (!expect_int("other player unchanged", runtime_player_merchant_adapter_get_resource("gold", 11), 2))
        return 1;

    for (int i = 0; i < 10; i++)
    {
        status = runtime_player_merchant_adapter_trade_buy(&ports,
                                                           1,
                                                           7,
                                                           "wood",
                                                           1,
                                                           &seeded,
                                                           11);
        if (!expect_int("interleaved buy status", status, 0))
            return 1;
        expected_player_a_gold -= 2;
        if (expected_player_a_gold < 0)
            expected_player_a_gold = 0;
        expected_player_a_wood += 1;

        status = runtime_player_merchant_adapter_trade_sell(&ports,
                                                            1,
                                                            7,
                                                            "wood",
                                                            1,
                                                            &seeded,
                                                            22);
        if (!expect_int("interleaved sell status", status, 0))
            return 1;
        expected_player_b_gold += 3;
    }

    if (!expect_int("player a gold isolated", runtime_player_merchant_adapter_get_resource("gold", 11), expected_player_a_gold))
        return 1;
    if (!expect_int("player b gold isolated", runtime_player_merchant_adapter_get_resource("gold", 22), expected_player_b_gold))
        return 1;
    if (!expect_int("player a wood isolated", runtime_player_merchant_adapter_get_resource("wood", 11), expected_player_a_wood))
        return 1;
    if (!expect_int("player b wood isolated", runtime_player_merchant_adapter_get_resource("wood", 22), expected_player_b_wood))
        return 1;
    if (!expect_int("buy call count after interleaving", g_probe.buy_calls, 11))
        return 1;
    if (!expect_int("sell call count after interleaving", g_probe.sell_calls, 11))
        return 1;

    return 0;
}
