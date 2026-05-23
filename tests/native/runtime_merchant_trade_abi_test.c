#include "runtime/merchant_trade_abi.h"

#include <stdio.h>

static int g_buy_calls = 0;
static int g_sell_calls = 0;
static int g_get_price_calls = 0;

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int trap_get_price(int npc_id,
                          const char *item_type,
                          int *inout_seeded,
                          int *out_price)
{
    (void)npc_id;
    (void)item_type;
    (void)inout_seeded;
    (void)out_price;
    g_get_price_calls += 1;
    return 0;
}

static int stub_trade_buy(int npc_id,
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
    g_buy_calls += 1;
    return 0;
}

static int stub_trade_sell(int npc_id,
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
    g_sell_calls += 1;
    return 0;
}

int main(void)
{
    RuntimeApplicationServicePorts ports = {0};
    RuntimeMerchantResourceGateway gateway = {0};
    int seeded = 0;

    ports.merchant.get_price = trap_get_price;
    ports.merchant.trade_buy = stub_trade_buy;
    ports.merchant.trade_sell = stub_trade_sell;

    if (!expect_int("buy delegates", runtime_merchant_trade_abi_buy(&ports, 1, 5, "wood", 2, &seeded, gateway), 0))
        return 1;
    if (!expect_int("sell delegates", runtime_merchant_trade_abi_sell(&ports, 1, 5, "ore", 2, &seeded, gateway), 0))
        return 1;
    if (!expect_int("buy called", g_buy_calls, 1))
        return 1;
    if (!expect_int("sell called", g_sell_calls, 1))
        return 1;
    if (!expect_int("trade facade never calls get_price", g_get_price_calls, 0))
        return 1;

    if (!expect_int("buy rejects world not ready", runtime_merchant_trade_abi_buy(&ports, 0, 5, "wood", 1, &seeded, gateway), -1))
        return 1;
    if (!expect_int("sell rejects quantity", runtime_merchant_trade_abi_sell(&ports, 1, 5, "ore", 0, &seeded, gateway), -1))
        return 1;

    ports.merchant.trade_buy = NULL;
    if (!expect_int("buy rejects null buy port", runtime_merchant_trade_abi_buy(&ports, 1, 5, "wood", 1, &seeded, gateway), -1))
        return 1;

    ports.merchant.trade_buy = stub_trade_buy;
    ports.merchant.trade_sell = NULL;
    if (!expect_int("sell rejects null sell port", runtime_merchant_trade_abi_sell(&ports, 1, 5, "ore", 1, &seeded, gateway), -1))
        return 1;

    if (!expect_int("buy rejects null ports", runtime_merchant_trade_abi_buy(NULL, 1, 5, "wood", 1, &seeded, gateway), -1))
        return 1;

    return 0;
}
