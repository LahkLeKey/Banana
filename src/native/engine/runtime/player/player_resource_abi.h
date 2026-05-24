#ifndef BANANA_ENGINE_RUNTIME_PLAYER_RESOURCE_ABI_H
#define BANANA_ENGINE_RUNTIME_PLAYER_RESOURCE_ABI_H

#include "../resource_domain.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_player_resource_abi_get(const char *player_guid,
                                        const char *resource_type);

    int runtime_player_resource_abi_add(const char *player_guid,
                                        const char *resource_type,
                                        int amount);

    int runtime_player_resource_abi_get_by_key(const char *player_guid,
                                               RuntimeResourceKey resource_key);

    int runtime_player_resource_abi_add_by_key(const char *player_guid,
                                               RuntimeResourceKey resource_key,
                                               int amount);

    int runtime_player_resource_abi_set_total_by_key(const char *player_guid,
                                                     RuntimeResourceKey resource_key,
                                                     int target_amount);

#ifdef __cplusplus
}
#endif

#endif
