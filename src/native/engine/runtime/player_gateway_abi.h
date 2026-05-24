#ifndef BANANA_ENGINE_RUNTIME_PLAYER_GATEWAY_ABI_H
#define BANANA_ENGINE_RUNTIME_PLAYER_GATEWAY_ABI_H

#include "merchant/merchant_service.h"
#include "player_registry.h"

#ifdef __cplusplus
extern "C"
{
#endif

    const char *runtime_player_gateway_active_guid(EntityId active_player_id);

    RuntimeMerchantResourceGateway runtime_player_gateway_resource_gateway(
        RuntimeMerchantGetResourceByKeyFn get,
        RuntimeMerchantSetResourceTotalByKeyFn set_total,
        RuntimeMerchantAddResourceByKeyFn add,
        void *context);

#ifdef __cplusplus
}
#endif

#endif