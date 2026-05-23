#ifndef BANANA_ENGINE_RUNTIME_PLAYER_SYNC_ABI_H
#define BANANA_ENGINE_RUNTIME_PLAYER_SYNC_ABI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimePlayerSyncWindow
    {
        int64_t current_time_ms;
        int64_t stale_threshold_ms;
    } RuntimePlayerSyncWindow;

    void runtime_player_sync_abi_mark_seen(const char *player_guid,
                                           int64_t current_time_ms);

    void runtime_player_sync_abi_update_staleness(RuntimePlayerSyncWindow window);

    void runtime_player_sync_abi_deactivate_stale(void);

    int runtime_player_sync_abi_count_active(void);

#ifdef __cplusplus
}
#endif

#endif