#include "player_resource_abi.h"

#include "player_registry.h"

int runtime_player_resource_abi_get(const char *player_guid,
                                    const char *resource_type)
{
    RuntimeResourceKey resource_key;

    if (!resource_type)
        return 0;

    if (runtime_resource_parse_key(resource_type, &resource_key) != 0)
        return 0;

    return runtime_player_resource_abi_get_by_key(player_guid, resource_key);
}

int runtime_player_resource_abi_add(const char *player_guid,
                                    const char *resource_type,
                                    int amount)
{
    RuntimeResourceKey resource_key;

    if (!resource_type)
        return 0;

    if (runtime_resource_parse_key(resource_type, &resource_key) != 0)
        return 0;

    return runtime_player_resource_abi_add_by_key(player_guid, resource_key, amount);
}

int runtime_player_resource_abi_get_by_key(const char *player_guid,
                                           RuntimeResourceKey resource_key)
{
    return runtime_player_registry_get_resource_key(player_guid, resource_key);
}

int runtime_player_resource_abi_add_by_key(const char *player_guid,
                                           RuntimeResourceKey resource_key,
                                           int amount)
{
    runtime_player_registry_add_resource_key(player_guid, resource_key, amount);
    return runtime_player_registry_get_resource_key(player_guid, resource_key);
}

int runtime_player_resource_abi_set_total_by_key(const char *player_guid,
                                                 RuntimeResourceKey resource_key,
                                                 int target_amount)
{
    int current = runtime_player_resource_abi_get_by_key(player_guid, resource_key);
    return runtime_player_resource_abi_add_by_key(player_guid,
                                                  resource_key,
                                                  target_amount - current);
}
