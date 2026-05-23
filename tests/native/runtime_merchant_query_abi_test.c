#include "runtime/merchant_query_abi.h"

#include <stdio.h>

static int g_get_price_calls = 0;
static int g_trade_buy_calls = 0;
static int g_trade_sell_calls = 0;

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int stub_get_price(int npc_id,
                          const char *item_type,
                          int *inout_seeded,
                          int *out_price)
{
    (void)npc_id;
    (void)item_type;

    g_get_price_calls += 1;
    if (inout_seeded)
        *inout_seeded = 1;
    if (out_price)
        *out_price = 42;
    return 0;
}

static int trap_trade_buy(int npc_id,
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
    return 0;
}

static int trap_trade_sell(int npc_id,
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
    return 0;
}

int main(void)
{
    RuntimeApplicationServicePorts ports = {0};
    int seeded = 0;

    ports.merchant.get_price = stub_get_price;
    ports.merchant.trade_buy = trap_trade_buy;
    ports.merchant.trade_sell = trap_trade_sell;

    if (!expect_int("query delegates", runtime_merchant_query_abi_get_price(&ports, 3, "wood", &seeded), 42))
        return 1;
    if (!expect_int("get_price called", g_get_price_calls, 1))
        return 1;
    if (!expect_int("trade_buy not called by query", g_trade_buy_calls, 0))
        return 1;
    if (!expect_int("trade_sell not called by query", g_trade_sell_calls, 0))
        return 1;
    if (!expect_int("query rejects null item", runtime_merchant_query_abi_get_price(&ports, 3, NULL, &seeded), 0))
        return 1;
    if (!expect_int("query rejects null ports", runtime_merchant_query_abi_get_price(NULL, 3, "wood", &seeded), 0))
        return 1;

    return 0;
}
