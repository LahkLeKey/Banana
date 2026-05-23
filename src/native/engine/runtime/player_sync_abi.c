#include "player_sync_abi.h"

#include "player_registry.h"

void runtime_player_sync_abi_mark_seen(const char *player_guid,
                                       int64_t current_time_ms)
{
    runtime_player_registry_mark_seen(player_guid, current_time_ms);
}

void runtime_player_sync_abi_update_staleness(RuntimePlayerSyncWindow window)
{
    runtime_player_registry_update_staleness(window.current_time_ms,
                                             window.stale_threshold_ms);
}

void runtime_player_sync_abi_deactivate_stale(void)
{
    runtime_player_registry_deactivate_stale();
}

int runtime_player_sync_abi_count_active(void)
{
    return runtime_player_registry_count_active();
}