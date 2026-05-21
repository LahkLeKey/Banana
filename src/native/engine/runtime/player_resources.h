/**
 * @file player_resources.h
 * @brief Player resource tracking and inventory management
 *
 * Manages player resources (wood, ore, etc.) and collection mechanics.
 * Resources are server-owned and synchronized to clients.
 * Collection is triggered by interactive object interactions.
 */

#ifndef BANANA_ENGINE_PLAYER_RESOURCES_H
#define BANANA_ENGINE_PLAYER_RESOURCES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Resource type identifiers
     */
    typedef enum
    {
        RESOURCE_WOOD = 0,
        RESOURCE_ORE = 1,
        RESOURCE_STONE = 2,
        RESOURCE_COUNT = 3
    } ResourceType;

    /**
     * Player resource inventory
     */
    typedef struct
    {
        const char *player_guid;
        int resources[RESOURCE_COUNT]; /* Amount of each resource */
        int64_t last_updated_tick;
    } PlayerResources;

    /**
     * Resource collection callback: triggered when player clicks interactive object
     */
    typedef struct
    {
        ResourceType resource_type;
        int amount;
        int cooldown_ms; /* Milliseconds before object can be collected again */
    } ResourceReward;

    /**
     * Initialize player resources system
     */
    int player_resources_init(void);

    /**
     * Get or create player resources entry
     * Returns pointer to player resources, creating if needed
     */
    PlayerResources *player_resources_get_or_create(const char *player_guid);

    /**
     * Add resources to player (collection event)
     * @param player_guid Player identifier
     * @param resource_type Resource being collected
     * @param amount Amount to add
     * @return 0 on success, -1 on error
     */
    int player_resources_add(const char *player_guid, ResourceType resource_type, int amount);

    /**
     * Subtract resources from player (trade event)
     * @param player_guid Player identifier
     * @param resource_type Resource being spent
     * @param amount Amount to subtract
     * @return 0 on success, -1 if insufficient resources
     */
    int player_resources_subtract(const char *player_guid, ResourceType resource_type, int amount);

    /**
     * Get resource balance for player
     * @return Amount of resource, or -1 if player not found
     */
    int player_resources_get_balance(const char *player_guid, ResourceType resource_type);

    /**
     * Transfer resources between players (trading)
     * @return 0 on success
     */
    int player_resources_transfer(const char *from_player, const char *to_player,
                                   ResourceType resource_type, int amount);

    /**
     * Get all resources for a player (for snapshot/sync)
     * @param player_guid Player identifier
     * @param out_resources Array to fill with resource amounts (must be RESOURCE_COUNT size)
     * @return 0 on success
     */
    int player_resources_get_snapshot(const char *player_guid, int *out_resources);

    /**
     * Reset player resources (new session)
     */
    int player_resources_reset(const char *player_guid);

    /**
     * Cleanup the system
     */
    void player_resources_cleanup(void);

    /**
     * Get resource name string
     */
    const char *player_resources_type_name(ResourceType type);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_PLAYER_RESOURCES_H */
