#ifndef BANANA_ENGINE_RUNTIME_PLAYER_REGISTRY_H
#define BANANA_ENGINE_RUNTIME_PLAYER_REGISTRY_H

#include "../../world/world.h"
#include "../resource/resource_domain.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define BANANA_MAX_NATIVE_PLAYERS 128

    typedef struct NativePlayerBinding
    {
        char guid[40];
        char name[64];
        char controller_kind[16];
        EntityId entity_id;
        float pending_move_x;
        float pending_move_z;
        int active;
        int64_t last_seen_ms;  /* Timestamp in milliseconds; used for staleness detection */
        int is_stale;          /* Flag: set when player exceeds staleness window */
        int wood_count;        /* Resource inventory: wood */
        int ore_count;         /* Resource inventory: ore */
        int gold_count;        /* Resource inventory: gold */
    } NativePlayerBinding;

    typedef float (*RuntimeTerrainSampleFn)(float x, float z);
    typedef uint32_t (*RuntimeControllerAttachFn)(uint32_t entity_id, const char *type);

    void runtime_player_registry_reset(void);
    int runtime_player_registry_count(void);
    NativePlayerBinding *runtime_player_registry_get(int index);
    NativePlayerBinding *runtime_player_registry_find(const char *guid);

    void runtime_player_registry_add_default(EntityId entity_id,
                                             const char *guid,
                                             const char *name,
                                             const char *controller_kind,
                                             int active);

    NativePlayerBinding *runtime_player_registry_upsert(World *world,
                                                        const char *guid,
                                                        const char *name,
                                                        const char *controller_kind,
                                                        int active,
                                                        RuntimeTerrainSampleFn terrain_sample_height,
                                                        RuntimeControllerAttachFn attach_controller,
                                                        EntityId *inout_primary_player_id);

    void runtime_player_registry_apply_input(const char *guid, float input_x, float input_z);

    void runtime_player_registry_set_transform(World *world,
                                               const char *guid,
                                               float x,
                                               float y,
                                               float z,
                                               int active,
                                               float island_span,
                                               RuntimeTerrainSampleFn terrain_sample_height);

    /**
     * Multiplayer sync ownership APIs (C-owned reconciliation)
     */
    
    /* Update player's last-seen timestamp and reset stale flag */
    void runtime_player_registry_mark_seen(const char *guid, int64_t current_time_ms);
    
    /* Check staleness for all players against current time and a staleness threshold (milliseconds) */
    void runtime_player_registry_update_staleness(int64_t current_time_ms, int64_t stale_threshold_ms);
    
    /* Deactivate stale players (called after update_staleness) */
    void runtime_player_registry_deactivate_stale(void);
    
    /* Count active (non-stale) players connected to session */
    int runtime_player_registry_count_active(void);

    /**
     * Resource inventory management APIs
     */

    /* Add resource to player inventory using typed resource keys. */
    void runtime_player_registry_add_resource_key(const char *guid,
                                                  RuntimeResourceKey resource_key,
                                                  int amount);

    /* Get resource count from player inventory using typed resource keys. */
    int runtime_player_registry_get_resource_key(const char *guid,
                                                 RuntimeResourceKey resource_key);
    
    /* Add resource to player inventory */
    void runtime_player_registry_add_resource(const char *guid, const char *resource_type, int amount);
    
    /* Get resource count from player inventory */
    int runtime_player_registry_get_resource(const char *guid, const char *resource_type);

#ifdef __cplusplus
}
#endif

#endif