/**
 * @file actor_mesh_registry.c
 * @brief Actor mesh registry implementation
 *
 * Central registry for actor types and their mesh representations.
 * Supports multiple LOD levels per actor type for efficient rendering
 * at varied distances.
 */

#include "actor_mesh_registry.h"

#include <stdlib.h>
#include <string.h>

/* Maximum number of actor types we can register */
#define MAX_ACTOR_TYPES 64

/* Registry state */
static struct
{
    int count;
    ActorMeshEntry entries[MAX_ACTOR_TYPES];
} g_actor_registry = {0};

int actor_mesh_registry_init(void)
{
    g_actor_registry.count = 0;
    memset(g_actor_registry.entries, 0, sizeof(g_actor_registry.entries));
    return 0;
}

int actor_mesh_registry_register(const char *actor_type, const ActorMeshLOD *lod_meshes,
                                 int lod_count, float click_radius, float scale, int flags)
{
    if (!actor_type || !lod_meshes || lod_count <= 0 || lod_count > ACTOR_LOD_COUNT)
    {
        return -1;
    }

    if (g_actor_registry.count >= MAX_ACTOR_TYPES)
    {
        return -1; /* Registry full */
    }

    /* Check for duplicate */
    for (int i = 0; i < g_actor_registry.count; i++)
    {
        if (strcmp(g_actor_registry.entries[i].actor_type, actor_type) == 0)
        {
            return -1; /* Already registered */
        }
    }

    ActorMeshEntry *entry = &g_actor_registry.entries[g_actor_registry.count];
    entry->actor_type = actor_type;
    entry->scale = scale;
    entry->flags = flags;
    entry->lod_count = lod_count;

    /* Copy LOD meshes */
    for (int i = 0; i < lod_count; i++)
    {
        entry->lods[i] = lod_meshes[i];
    }

    g_actor_registry.count++;
    return 0;
}

const ActorMeshEntry *actor_mesh_registry_lookup(const char *actor_type)
{
    if (!actor_type)
    {
        return NULL;
    }

    for (int i = 0; i < g_actor_registry.count; i++)
    {
        if (strcmp(g_actor_registry.entries[i].actor_type, actor_type) == 0)
        {
            return &g_actor_registry.entries[i];
        }
    }

    return NULL;
}

int actor_mesh_registry_select_lod(const ActorMeshEntry *entry, float distance)
{
    if (!entry || entry->lod_count <= 0)
    {
        return -1;
    }

    /* Simple distance-based LOD selection */
    if (distance < 10.0f)
        return 0; /* ULTRA */
    if (distance < 50.0f)
        return 1; /* HIGH */
    if (distance < 150.0f)
        return 2; /* MED */

    /* Clamp to highest available LOD */
    int lod = entry->lod_count - 1;
    return (lod < ACTOR_LOD_COUNT) ? lod : (ACTOR_LOD_COUNT - 1);
}

int actor_mesh_registry_foreach(int (*callback)(const ActorMeshEntry *entry, void *userdata),
                                void *userdata)
{
    if (!callback)
    {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < g_actor_registry.count; i++)
    {
        int ret = callback(&g_actor_registry.entries[i], userdata);
        count++;
        if (ret != 0)
        {
            break;
        }
    }

    return count;
}

int actor_mesh_registry_count(void)
{
    return g_actor_registry.count;
}

void actor_mesh_registry_cleanup(void)
{
    for (int i = 0; i < g_actor_registry.count; i++)
    {
        ActorMeshEntry *entry = &g_actor_registry.entries[i];
        for (int j = 0; j < entry->lod_count; j++)
        {
            if (entry->lods[j].mesh)
            {
                mesh_destroy(entry->lods[j].mesh);
                entry->lods[j].mesh = NULL;
            }
        }
    }

    g_actor_registry.count = 0;
}
