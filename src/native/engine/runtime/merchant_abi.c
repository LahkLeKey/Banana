#include "merchant_abi.h"

int runtime_merchant_abi_get_price(const RuntimeApplicationServicePorts *ports,
                                   int npc_id,
                                   const char *item_type,
                                   int *inout_seeded)
{
    int price = 0;

    if (!ports || !ports->merchant.get_price || !item_type)
        return 0;

    if (ports->merchant.get_price(npc_id, item_type, inout_seeded, &price) != 0)
        return 0;

    return price;
}

int runtime_merchant_abi_trade_buy(const RuntimeApplicationServicePorts *ports,
                                   int world_ready,
                                   int npc_id,
                                   const char *item_type,
                                   int quantity,
                                   int *inout_seeded,
                                   RuntimeMerchantResourceGateway resource_gateway)
{
    if (!world_ready || !ports || !ports->merchant.trade_buy || !item_type || quantity <= 0)
        return -1;

    return ports->merchant.trade_buy(npc_id,
                                     item_type,
                                     quantity,
                                     inout_seeded,
                                     resource_gateway);
}

int runtime_merchant_abi_trade_sell(const RuntimeApplicationServicePorts *ports,
                                    int world_ready,
                                    int npc_id,
                                    const char *item_type,
                                    int quantity,
                                    int *inout_seeded,
                                    RuntimeMerchantResourceGateway resource_gateway)
{
    if (!world_ready || !ports || !ports->merchant.trade_sell || !item_type || quantity <= 0)
        return -1;

    return ports->merchant.trade_sell(npc_id,
                                      item_type,
                                      quantity,
                                      inout_seeded,
                                      resource_gateway);
}
