#include "runtime/merchant_abi.h"

#include <stdio.h>

static int g_seeded_seen = -1;
static int g_buy_called = 0;
static int g_sell_called = 0;
static int g_get_price_called = 0;

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
    g_get_price_called += 1;
    g_seeded_seen = inout_seeded ? *inout_seeded : -1;
    if (inout_seeded)
        *inout_seeded = 1;
    if (out_price)
        *out_price = 42;
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
    (void)resource_gateway;
    g_buy_called += 1;
    if (inout_seeded)
        *inout_seeded = 1;
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
    (void)resource_gateway;
    g_sell_called += 1;
    if (inout_seeded)
        *inout_seeded = 1;
    return 0;
}

static int stub_get_resource(RuntimeResourceKey key)
{
    (void)key;
    return 0;
}

static int stub_set_resource_total(RuntimeResourceKey key, int target_amount)
{
    (void)key;
    return target_amount;
}

static int stub_add_resource(RuntimeResourceKey key, int amount)
{
    (void)key;
    return amount;
}

int main(void)
{
    RuntimeApplicationServicePorts ports = {0};
    RuntimeMerchantResourceGateway gateway = {0};
    int seeded = 0;

    ports.merchant.get_price = stub_get_price;
    ports.merchant.trade_buy = stub_trade_buy;
    ports.merchant.trade_sell = stub_trade_sell;

    gateway.get = stub_get_resource;
    gateway.set_total = stub_set_resource_total;
    gateway.add = stub_add_resource;

    if (!expect_int("price delegates", runtime_merchant_abi_get_price(&ports, 0, "wood", &seeded), 42))
        return 1;
    if (!expect_int("get price called", g_get_price_called, 1))
        return 1;
    if (!expect_int("seed seen at delegate", g_seeded_seen, 0))
        return 1;

    if (!expect_int("buy delegates", runtime_merchant_abi_trade_buy(&ports, 1, 0, "wood", 1, &seeded, gateway), 0))
        return 1;
    if (!expect_int("buy called", g_buy_called, 1))
        return 1;

    if (!expect_int("sell delegates", runtime_merchant_abi_trade_sell(&ports, 1, 0, "wood", 1, &seeded, gateway), 0))
        return 1;
    if (!expect_int("sell called", g_sell_called, 1))
        return 1;

    if (!expect_int("buy rejects missing world", runtime_merchant_abi_trade_buy(&ports, 0, 0, "wood", 1, &seeded, gateway), -1))
        return 1;
    if (!expect_int("sell rejects invalid qty", runtime_merchant_abi_trade_sell(&ports, 1, 0, "wood", 0, &seeded, gateway), -1))
        return 1;
    if (!expect_int("price rejects null type", runtime_merchant_abi_get_price(&ports, 0, NULL, &seeded), 0))
        return 1;

    return 0;
}
