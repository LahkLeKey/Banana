#include "merchant_service.h"

#include "../trade/merchant_trade_domain.h"
#include "../../resource/resource_domain.h"

#include "../../../ai/npc_merchant.h"

static int runtime_merchant_seed_default_if_needed(int npc_id, int *inout_seeded)
{
    if (!inout_seeded || npc_id < 0)
        return -1;

    if (!(*inout_seeded))
    {
        npc_merchant_init();
        *inout_seeded = 1;
    }

    while (npc_merchant_count() <= npc_id)
    {
        int merchant_id = npc_merchant_register(npc_merchant_count(), 0.0f, 0.0f, 500);
        if (merchant_id < 0)
            return -1;

        npc_merchant_add_inventory_item(merchant_id, "wood", 100, 200, 5);
        npc_merchant_add_inventory_item(merchant_id, "ore", 50, 100, 15);
    }

    return 0;
}

int runtime_merchant_service_get_price(int npc_id,
                                       const char *item_type,
                                       int *inout_seeded,
                                       int *out_price)
{
    RuntimeResourceKey item_key;
    const char *canonical_item_name = NULL;
    int stock = 0;
    int price = 0;

    if (!item_type || !out_price)
        return -1;

    if (runtime_merchant_seed_default_if_needed(npc_id, inout_seeded) != 0)
        return -1;

    if (runtime_resource_parse_key(item_type, &item_key) != 0)
        return -1;

    canonical_item_name = runtime_resource_key_name(item_key);
    if (npc_merchant_get_item_price(npc_id, canonical_item_name, &stock, &price) != 0)
        return -1;

    *out_price = price;
    return 0;
}

int runtime_merchant_service_trade_buy(int npc_id,
                                       const char *item_type,
                                       int quantity,
                                       int *inout_seeded,
                                       RuntimeMerchantResourceGateway resource_gateway)
{
    RuntimeResourceKey item_key;
    MerchantTradeResult result;
    int player_gold = 0;
    int mapped_status = -1;

    if (!item_type || quantity <= 0 || !resource_gateway.get || !resource_gateway.set_total ||
        !resource_gateway.add)
        return -1;

    if (runtime_merchant_seed_default_if_needed(npc_id, inout_seeded) != 0)
        return -1;

    if (runtime_resource_parse_key(item_type, &item_key) != 0)
        return -1;

    player_gold = resource_gateway.get(resource_gateway.context,
                                       RUNTIME_RESOURCE_GOLD);
    result = npc_merchant_trade_buy(npc_id,
                                    runtime_resource_key_name(item_key),
                                    quantity,
                                    &player_gold);
    mapped_status = runtime_merchant_map_buy_status(result);
    if (mapped_status != 0)
        return mapped_status;

    resource_gateway.set_total(resource_gateway.context,
                               RUNTIME_RESOURCE_GOLD,
                               player_gold);
    resource_gateway.add(resource_gateway.context, item_key, quantity);
    return 0;
}

int runtime_merchant_service_trade_sell(int npc_id,
                                        const char *item_type,
                                        int quantity,
                                        int *inout_seeded,
                                        RuntimeMerchantResourceGateway resource_gateway)
{
    RuntimeResourceKey item_key;
    MerchantTradeResult result;
    int player_amount = 0;
    int player_gold = 0;
    int mapped_status = -1;

    if (!item_type || quantity <= 0 || !resource_gateway.get || !resource_gateway.set_total ||
        !resource_gateway.add)
        return -1;

    if (runtime_merchant_seed_default_if_needed(npc_id, inout_seeded) != 0)
        return -1;

    if (runtime_resource_parse_key(item_type, &item_key) != 0)
        return -1;

    player_amount = resource_gateway.get(resource_gateway.context, item_key);
    if (player_amount < quantity)
        return -2;

    player_gold = resource_gateway.get(resource_gateway.context,
                                       RUNTIME_RESOURCE_GOLD);
    result = npc_merchant_trade_sell(npc_id,
                                     runtime_resource_key_name(item_key),
                                     quantity,
                                     &player_gold);
    mapped_status = runtime_merchant_map_sell_status(result);
    if (mapped_status != 0)
        return mapped_status;

    resource_gateway.add(resource_gateway.context, item_key, -quantity);
    resource_gateway.set_total(resource_gateway.context,
                               RUNTIME_RESOURCE_GOLD,
                               player_gold);
    return 0;
}
