/**
 * @file interactive_objects.h
 * @brief Interactive object system for clickable world entities (trees, barrels, etc.)
 *
 * This module manages interactive objects in the game world that players can click to
 * trigger actions (resource collection, events). State is C-owned and synchronized
 * across multiplayer clients.
 */

#ifndef BANANA_ENGINE_INTERACTIVE_OBJECTS_H
#define BANANA_ENGINE_INTERACTIVE_OBJECTS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Interactive object types (expandable enum)
 */
typedef enum {
  INTERACTIVE_TYPE_TREE = 1,
  INTERACTIVE_TYPE_ORE_DEPOSIT = 2,
  INTERACTIVE_TYPE_BARREL = 3,
} InteractiveObjectType;

/**
 * Interactive object state
 */
typedef enum {
  INTERACTIVE_STATE_ACTIVE = 1,      // Available for interaction
  INTERACTIVE_STATE_DEPLETED = 2,    // Exhausted, no longer yields resources
  INTERACTIVE_STATE_COOLDOWN = 3,    // Temporarily unavailable (respawning or cooldown)
} InteractiveObjectState;

/**
 * Represents a single interactive object in the world
 */
typedef struct {
  int id;                       // Unique object ID
  InteractiveObjectType type;   // Type of object (tree, ore, barrel)
  float x, z;                   // World position
  float click_radius;           // Clickable radius in world units
  InteractiveObjectState state; // Current state
  int64_t state_changed_at;     // Timestamp of last state change (for cooldown tracking)
  int resource_yield;           // Amount of resource granted on successful interaction
  const char *resource_type;    // Resource type (e.g., "wood", "ore")
} InteractiveObject;

/**
 * Initialize interactive object system
 * @return Status code (0 = success)
 */
int interactive_objects_init(void);

/**
 * Register a new interactive object in the world
 * @param object Pointer to initialized InteractiveObject
 * @return Object ID on success, -1 on error
 */
int interactive_objects_register(const InteractiveObject *object);

/**
 * Perform a click at world position and return the object that was clicked
 * Validates click is within object's click_radius, handles state transitions
 * @param world_x X position of click
 * @param world_z Z position of click
 * @param click_radius Radius of click detection area
 * @param out_object_id [out] ID of clicked object (if any)
 * @return Resource yield amount on success (>0), 0 if no object clicked, -1 on error
 */
int interactive_objects_click_at(float world_x, float world_z, float click_radius,
                                  int *out_object_id);

/**
 * Get current state of an interactive object
 * @param object_id Object ID
 * @param out_state [out] Current state
 * @return 0 on success, -1 if object not found
 */
int interactive_objects_get_state(int object_id, InteractiveObjectState *out_state);

/**
 * Set state of an interactive object
 * @param object_id Object ID
 * @param state New state
 * @param current_time_ms Current time for cooldown tracking
 * @return 0 on success, -1 if object not found
 */
int interactive_objects_set_state(int object_id, InteractiveObjectState state,
                                   int64_t current_time_ms);

/**
 * Tick interactive objects (update cooldowns, state transitions)
 * @param current_time_ms Current time in milliseconds
 * @param cooldown_duration_ms Cooldown duration after depletion
 */
void interactive_objects_tick(int64_t current_time_ms, int64_t cooldown_duration_ms);

/**
 * Get count of all registered interactive objects
 * @return Number of interactive objects
 */
int interactive_objects_count(void);

/**
 * Get interactive object by ID
 * @param object_id Object ID
 * @param out_object [out] Pointer to object data
 * @return 0 on success, -1 if not found
 */
int interactive_objects_get(int object_id, InteractiveObject *out_object);

/**
 * Iterate over all interactive objects (snapshot)
 * @param max_count Maximum number of objects to return
 * @param out_objects [out] Array to populate
 * @return Number of objects copied
 */
int interactive_objects_snapshot(int max_count, InteractiveObject *out_objects);

/**
 * Clear all interactive objects (typically on engine reset)
 */
void interactive_objects_clear(void);

/**
 * Shutdown interactive object system
 */
void interactive_objects_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif // BANANA_ENGINE_INTERACTIVE_OBJECTS_H
