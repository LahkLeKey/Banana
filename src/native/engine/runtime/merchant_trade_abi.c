#include "merchant_trade_abi.h"

int runtime_merchant_trade_abi_buy(const RuntimeApplicationServicePorts *ports,
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

int runtime_merchant_trade_abi_sell(const RuntimeApplicationServicePorts *ports,
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
