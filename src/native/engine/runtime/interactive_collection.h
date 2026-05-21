/**
 * @file interactive_collection.h
 * @brief Bind interactive objects to resource collection mechanics
 *
 * Maps interactive objects (trees, ore deposits) to resource rewards.
 * Handles click events on interactive objects and triggers resource collection.
 * Manages collection cooldowns and state persistence.
 */

#ifndef BANANA_ENGINE_INTERACTIVE_COLLECTION_H
#define BANANA_ENGINE_INTERACTIVE_COLLECTION_H

#include "player_resources.h"
#include "input_binding.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Collection result codes
     */
    typedef enum
    {
        COLLECTION_OK = 0,
        COLLECTION_OBJECT_NOT_FOUND = -1,
        COLLECTION_OBJECT_DEPLETED = -2,
        COLLECTION_OBJECT_COOLDOWN = -3,
        COLLECTION_INVALID_PLAYER = -4,
    } CollectionResult;

    /**
     * Interactive object with resource collection properties
     */
    typedef struct
    {
        int object_id;
        ResourceType resource_type;
        int resource_per_hit;
        int max_hits;
        int remaining_hits;
        int cooldown_ms;
        int64_t last_collected_tick;
    } CollectibleObject;

    /**
     * Initialize interactive collection system
     */
    int interactive_collection_init(void);

    /**
     * Register an interactive object as collectible
     * @param object_id From interactive_objects registry
     * @param resource_type What resource this yields
     * @param amount Amount per collection
     * @param max_hits How many times before depleted
     * @param cooldown_ms Milliseconds between collections
     * @return 0 on success
     */
    int interactive_collection_register(int object_id, ResourceType resource_type, int amount,
                                        int max_hits, int cooldown_ms);

    /**
     * Process a collection click on an interactive object
     * Updates player resources and object state
     * @param object_id Object being clicked
     * @param player_guid Player collecting
     * @return CollectionResult code
     */
    int interactive_collection_collect(int object_id, const char *player_guid);

    /**
     * Get state of a collectible object
     */
    const CollectibleObject *interactive_collection_get(int object_id);

    /**
     * Respawn an object (reset hits, cooldown)
     */
    int interactive_collection_respawn(int object_id);

    /**
     * Update cooldowns (tick callback)
     * @param dt_ms Delta time in milliseconds since last update
     */
    void interactive_collection_tick(int64_t dt_ms);

    /**
     * Cleanup system
     */
    void interactive_collection_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_INTERACTIVE_COLLECTION_H */
