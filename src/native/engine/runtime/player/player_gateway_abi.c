#include "player_gateway_abi.h"

const char *runtime_player_gateway_active_guid(EntityId active_player_id)
{
    const char *player_guid = NULL;
    int binding_count = runtime_player_registry_count();

    for (int i = 0; i < binding_count; i++) {
        NativePlayerBinding *binding = runtime_player_registry_get(i);
        if (!binding || !binding->guid[0])
            continue;

        if (binding->entity_id == active_player_id)
            return binding->guid;

        if (!player_guid)
            player_guid = binding->guid;
    }

    return player_guid;
}

RuntimeMerchantResourceGateway runtime_player_gateway_resource_gateway(
    RuntimeMerchantGetResourceByKeyFn get,
    RuntimeMerchantSetResourceTotalByKeyFn set_total,
    RuntimeMerchantAddResourceByKeyFn add,
    void *context)
{
    RuntimeMerchantResourceGateway gateway;
    gateway.context = context;
    gateway.get = get;
    gateway.set_total = set_total;
    gateway.add = add;
    return gateway;
}