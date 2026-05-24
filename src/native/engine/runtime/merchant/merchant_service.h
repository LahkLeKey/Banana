#ifndef BANANA_ENGINE_RUNTIME_MERCHANT_SERVICE_H
#define BANANA_ENGINE_RUNTIME_MERCHANT_SERVICE_H

#include "../resource/resource_domain.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef int (*RuntimeMerchantGetResourceByKeyFn)(void *context,
                                                     RuntimeResourceKey resource_key);
    typedef int (*RuntimeMerchantSetResourceTotalByKeyFn)(void *context,
                                                          RuntimeResourceKey resource_key,
                                                          int target_amount);
    typedef int (*RuntimeMerchantAddResourceByKeyFn)(void *context,
                                                     RuntimeResourceKey resource_key,
                                                     int amount);

    typedef struct RuntimeMerchantResourceGateway
    {
        void *context;
        RuntimeMerchantGetResourceByKeyFn get;
        RuntimeMerchantSetResourceTotalByKeyFn set_total;
        RuntimeMerchantAddResourceByKeyFn add;
    } RuntimeMerchantResourceGateway;

    int runtime_merchant_service_get_price(int npc_id,
                                           const char *item_type,
                                           int *inout_seeded,
                                           int *out_price);

    int runtime_merchant_service_trade_buy(int npc_id,
                                           const char *item_type,
                                           int quantity,
                                           int *inout_seeded,
                                           RuntimeMerchantResourceGateway resource_gateway);

    int runtime_merchant_service_trade_sell(int npc_id,
                                            const char *item_type,
                                            int quantity,
                                            int *inout_seeded,
                                            RuntimeMerchantResourceGateway resource_gateway);

#ifdef __cplusplus
}
#endif

#endif
