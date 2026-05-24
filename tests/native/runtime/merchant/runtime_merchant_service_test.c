#include "runtime/merchant/merchant_service.h"
#include "runtime/resource/resource_domain.h"

#include <stdio.h>

static int g_gold = 0;
static int g_wood = 0;
static int g_ore = 0;

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int get_resource(void *context, RuntimeResourceKey key)
{
    (void)context;
    if (key == RUNTIME_RESOURCE_GOLD)
        return g_gold;
    if (key == RUNTIME_RESOURCE_WOOD)
        return g_wood;
    if (key == RUNTIME_RESOURCE_ORE)
        return g_ore;

    return 0;
}

static int set_resource_total(void *context,
                              RuntimeResourceKey key,
                              int target_amount)
{
    (void)context;
    if (key == RUNTIME_RESOURCE_GOLD)
    {
        g_gold = target_amount;
        return g_gold;
    }

    return 0;
}

static int add_resource(void *context,
                        RuntimeResourceKey key,
                        int amount)
{
    (void)context;
    if (key == RUNTIME_RESOURCE_WOOD)
    {
        g_wood += amount;
        if (g_wood < 0)
            g_wood = 0;
        return g_wood;
    }

    if (key == RUNTIME_RESOURCE_ORE)
    {
        g_ore += amount;
        if (g_ore < 0)
            g_ore = 0;
        return g_ore;
    }

    if (key == RUNTIME_RESOURCE_GOLD)
    {
        g_gold += amount;
        if (g_gold < 0)
            g_gold = 0;
        return g_gold;
    }

    return 0;
}

int main(void)
{
    int seeded = 0;
    int price = 0;
    RuntimeMerchantResourceGateway gateway = {0};

    gateway.context = NULL;
    gateway.get = get_resource;
    gateway.set_total = set_resource_total;
    gateway.add = add_resource;

    if (!expect_int("seed starts false", seeded, 0))
        return 1;

    if (!expect_int("wood price", runtime_merchant_service_get_price(0, "wood", &seeded, &price), 0))
        return 1;
    if (!expect_int("wood price value", price, 5))
        return 1;
    if (!expect_int("seed flips true after first lookup", seeded, 1))
        return 1;

    if (!expect_int("ore price", runtime_merchant_service_get_price(0, "ore", &seeded, &price), 0))
        return 1;
    if (!expect_int("ore price value", price, 15))
        return 1;
    if (!expect_int("seed remains true on reuse", seeded, 1))
        return 1;

    if (!expect_int("unknown item rejected", runtime_merchant_service_trade_buy(0,
                                                                                   "banana",
                                                                                   1,
                                                                                   &seeded,
                                                                                   gateway),
                    -1))
        return 1;

    g_gold = 0;
    g_wood = 0;
    g_ore = 0;
    if (!expect_int("buy insufficient gold", runtime_merchant_service_trade_buy(0,
                                                                                  "wood",
                                                                                  1,
                                                                                  &seeded,
                                                                                  gateway),
                    -2))
        return 1;

    g_gold = 100;
    if (!expect_int("buy wood success", runtime_merchant_service_trade_buy(0,
                                                                             "wood",
                                                                             2,
                                                                             &seeded,
                                                                             gateway),
                    0))
        return 1;
    if (!expect_int("gold after buy", g_gold, 90))
        return 1;
    if (!expect_int("wood after buy", g_wood, 2))
        return 1;

    if (!expect_int("sell insufficient items", runtime_merchant_service_trade_sell(0,
                                                                                     "ore",
                                                                                     1,
                                                                                     &seeded,
                                                                                     gateway),
                    -2))
        return 1;

    g_ore = 40;
    if (!expect_int("sell insufficient merchant gold", runtime_merchant_service_trade_sell(0,
                                                                                             "ore",
                                                                                             40,
                                                                                             &seeded,
                                                                                             gateway),
                    -4))
        return 1;

    if (!expect_int("sell wood success", runtime_merchant_service_trade_sell(0,
                                                                               "wood",
                                                                               1,
                                                                               &seeded,
                                                                               gateway),
                    0))
        return 1;
    if (!expect_int("wood after sell", g_wood, 1))
        return 1;
    if (!expect_int("gold after sell", g_gold, 95))
        return 1;

    g_gold = 10;
    g_wood = 0;
    g_ore = 0;
    if (!expect_int("buy exact gold succeeds", runtime_merchant_service_trade_buy(1,
                                                                                    "wood",
                                                                                    2,
                                                                                    &seeded,
                                                                                    gateway),
                    0))
        return 1;
    if (!expect_int("gold transitions to zero", g_gold, 0))
        return 1;
    if (!expect_int("next buy fails insufficient gold", runtime_merchant_service_trade_buy(1,
                                                                                             "wood",
                                                                                             1,
                                                                                             &seeded,
                                                                                             gateway),
                    -2))
        return 1;

    g_gold = 2000;
    g_wood = 0;
    g_ore = 0;
    if (!expect_int("buy exact stock succeeds", runtime_merchant_service_trade_buy(2,
                                                                                     "wood",
                                                                                     100,
                                                                                     &seeded,
                                                                                     gateway),
                    0))
        return 1;
    if (!expect_int("next buy fails insufficient stock", runtime_merchant_service_trade_buy(2,
                                                                                              "wood",
                                                                                              1,
                                                                                              &seeded,
                                                                                              gateway),
                    -3))
        return 1;
    if (!expect_int("seed still true after multi-merchant flow", seeded, 1))
        return 1;

    return 0;
}
