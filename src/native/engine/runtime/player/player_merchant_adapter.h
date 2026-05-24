#ifndef BANANA_ENGINE_RUNTIME_PLAYER_MERCHANT_ADAPTER_H
#define BANANA_ENGINE_RUNTIME_PLAYER_MERCHANT_ADAPTER_H

#include "../engine/application_service_ports.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_player_merchant_adapter_get_resource(const char *resource_type,
                                                     EntityId active_player_id);

    int runtime_player_merchant_adapter_add_resource(const char *resource_type,
                                                     int amount,
                                                     EntityId active_player_id);

    int runtime_player_merchant_adapter_trade_buy(const RuntimeApplicationServicePorts *ports,
                                                  int world_ready,
                                                  int npc_id,
                                                  const char *item_type,
                                                  int quantity,
                                                  int *inout_seeded,
                                                  EntityId active_player_id);

    int runtime_player_merchant_adapter_trade_sell(const RuntimeApplicationServicePorts *ports,
                                                   int world_ready,
                                                   int npc_id,
                                                   const char *item_type,
                                                   int quantity,
                                                   int *inout_seeded,
                                                   EntityId active_player_id);

#ifdef __cplusplus
}
#endif

#endif