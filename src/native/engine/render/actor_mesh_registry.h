/**
 * @file actor_mesh_registry.h
 * @brief Registry for mapping actor types to their mesh representations
 *
 * This module manages a centralized registry of actor types (tree, merchant, barrel, etc.)
 * and their associated meshes (geometry + material). Used to bind gameplay actors to
 * renderable geometry and collision/interaction boundaries.
 *
 * Design: Actor meshes are registered once at engine init, then queried by entity
 * rendering pipeline. Supports LOD variants per actor type.
 */

#ifndef BANANA_ENGINE_ACTOR_MESH_REGISTRY_H
#define BANANA_ENGINE_ACTOR_MESH_REGISTRY_H

#include "mesh.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * LOD level enumeration for actor meshes
     */
    typedef enum
    {
        ACTOR_LOD_ULTRA = 0, /* highest detail (< 10 units) */
        ACTOR_LOD_HIGH = 1,  /* high detail (10-50 units) */
        ACTOR_LOD_MED = 2,   /* medium detail (50-150 units) */
        ACTOR_LOD_LOW = 3,   /* low detail (150+ units) */
        ACTOR_LOD_COUNT = 4
    } ActorLODLevel;

    /**
     * Metadata for a single LOD variant of an actor mesh
     */
    typedef struct
    {
        Mesh *mesh;              /* OpenGL mesh (VAO/VBO/EBO) */
        float aabb_min[3];       /* Bounding box minimum for culling */
        float aabb_max[3];       /* Bounding box maximum for culling */
        float click_radius;      /* Interaction radius for click detection */
        int vertex_count;        /* For statistics/validation */
    } ActorMeshLOD;

    /**
     * Registry entry for a single actor type
     */
    typedef struct
    {
        const char *actor_type;              /* e.g., "tree", "merchant", "barrel" */
        ActorMeshLOD lods[ACTOR_LOD_COUNT]; /* LOD0 (ultra) -> LOD3 (low) */
        int lod_count;                       /* How many LOD levels actually populated */
        float scale;                         /* Default scale multiplier for this actor */
        int flags;                           /* ACTOR_FLAG_* bitmask */
    } ActorMeshEntry;

    /**
     * Actor flags (can be combined)
     */
#define ACTOR_FLAG_STATIC (1 << 0)      /* Static, non-physics object */
#define ACTOR_FLAG_INTERACTIVE (1 << 1) /* Can be clicked/interacted */
#define ACTOR_FLAG_MOBILE (1 << 2)      /* Has animation/movement */

    /**
     * Initialize the actor mesh registry.
     * Must be called once at engine startup.
     */
    int actor_mesh_registry_init(void);

    /**
     * Register an actor type with its mesh(es).
     * actor_type: unique string identifier (e.g., "tree")
     * lod_meshes: array of LOD meshes (LOD0, LOD1, ..., LOD(lod_count-1))
     * lod_count: number of LOD levels (typically 1-4)
     * click_radius: interaction click radius (0 = non-interactive)
     * scale: default scale multiplier
     * flags: ACTOR_FLAG_* bitmask
     *
     * Returns 0 on success, -1 on error (max registry size exceeded).
     */
    int actor_mesh_registry_register(const char *actor_type, const ActorMeshLOD *lod_meshes,
                                     int lod_count, float click_radius, float scale, int flags);

    /**
     * Look up a registered actor type.
     * Returns pointer to ActorMeshEntry if found, NULL otherwise.
     */
    const ActorMeshEntry *actor_mesh_registry_lookup(const char *actor_type);

    /**
     * Get the best LOD level for a given distance.
     * entry: actor mesh entry (from lookup)
     * distance: distance from camera in world units
     * Returns LOD level (0-3) or -1 if entry is invalid.
     */
    int actor_mesh_registry_select_lod(const ActorMeshEntry *entry, float distance);

    /**
     * Iterate over all registered actor types.
     * callback: called for each entry; return non-zero to stop iteration.
     * Returns number of entries processed.
     */
    int actor_mesh_registry_foreach(int (*callback)(const ActorMeshEntry *entry, void *userdata),
                                    void *userdata);

    /**
     * Get total count of registered actor types.
     */
    int actor_mesh_registry_count(void);

    /**
     * Clear the registry and free all meshes.
     * Called at engine shutdown.
     */
    void actor_mesh_registry_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_ACTOR_MESH_REGISTRY_H */
