#ifndef BANANA_ENGINE_RUNTIME_MERCHANT_ABI_H
#define BANANA_ENGINE_RUNTIME_MERCHANT_ABI_H

#include "application_service_ports.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_merchant_abi_get_price(const RuntimeApplicationServicePorts *ports,
                                       int npc_id,
                                       const char *item_type,
                                       int *inout_seeded);

    int runtime_merchant_abi_trade_buy(const RuntimeApplicationServicePorts *ports,
                                       int world_ready,
                                       int npc_id,
                                       const char *item_type,
                                       int quantity,
                                       int *inout_seeded,
                                       RuntimeMerchantResourceGateway resource_gateway);

    int runtime_merchant_abi_trade_sell(const RuntimeApplicationServicePorts *ports,
                                        int world_ready,
                                        int npc_id,
                                        const char *item_type,
                                        int quantity,
                                        int *inout_seeded,
                                        RuntimeMerchantResourceGateway resource_gateway);

#ifdef __cplusplus
}
#endif

#endif
