/**
 * @file interactive_collection.c
 * @brief Implementation of interactive object collection system
 */

#include "interactive_collection.h"
#include "player_resources.h"

#include <stdlib.h>
#include <string.h>

#define MAX_COLLECTIBLE_OBJECTS 1024

typedef struct
{
    CollectibleObject objects[MAX_COLLECTIBLE_OBJECTS];
    int count;
} CollectibleRegistry;

static CollectibleRegistry g_registry = {0};
static int64_t g_tick_accumulator = 0;

int interactive_collection_init(void)
{
    memset(&g_registry, 0, sizeof(g_registry));
    g_registry.count = 0;
    g_tick_accumulator = 0;
    return player_resources_init();
}

int interactive_collection_register(int object_id, ResourceType resource_type, int amount,
                                    int max_hits, int cooldown_ms)
{
    if (g_registry.count >= MAX_COLLECTIBLE_OBJECTS)
    {
        return -1;
    }

    if (object_id < 0 || resource_type < 0 || resource_type >= RESOURCE_COUNT)
    {
        return -1;
    }

    /* Check for duplicate */
    for (int i = 0; i < g_registry.count; i++)
    {
        if (g_registry.objects[i].object_id == object_id)
        {
            return -1; /* Already registered */
        }
    }

    CollectibleObject *obj = &g_registry.objects[g_registry.count];
    obj->object_id = object_id;
    obj->resource_type = resource_type;
    obj->resource_per_hit = amount;
    obj->max_hits = max_hits;
    obj->remaining_hits = max_hits;
    obj->cooldown_ms = cooldown_ms;
    obj->last_collected_tick = -1; /* Never collected yet */

    g_registry.count++;
    return 0;
}

static CollectibleObject *find_object(int object_id)
{
    for (int i = 0; i < g_registry.count; i++)
    {
        if (g_registry.objects[i].object_id == object_id)
        {
            return &g_registry.objects[i];
        }
    }
    return NULL;
}

int interactive_collection_collect(int object_id, const char *player_guid)
{
    CollectibleObject *obj = find_object(object_id);
    if (!obj)
    {
        return COLLECTION_OBJECT_NOT_FOUND;
    }

    if (obj->remaining_hits <= 0)
    {
        return COLLECTION_OBJECT_DEPLETED;
    }

    int64_t time_since_last = g_tick_accumulator - obj->last_collected_tick;
    if (obj->last_collected_tick >= 0 && time_since_last < obj->cooldown_ms)
    {
        return COLLECTION_OBJECT_COOLDOWN;
    }

    if (!player_guid)
    {
        return COLLECTION_INVALID_PLAYER;
    }

    /* Add resources to player */
    int ret = player_resources_add(player_guid, obj->resource_type, obj->resource_per_hit);
    if (ret != 0)
    {
        return COLLECTION_INVALID_PLAYER;
    }

    /* Update object state */
    obj->remaining_hits--;
    obj->last_collected_tick = g_tick_accumulator;

    return COLLECTION_OK;
}

const CollectibleObject *interactive_collection_get(int object_id)
{
    return find_object(object_id);
}

int interactive_collection_respawn(int object_id)
{
    CollectibleObject *obj = find_object(object_id);
    if (!obj)
    {
        return -1;
    }

    obj->remaining_hits = obj->max_hits;
    obj->last_collected_tick = -1; /* Reset to "never collected" */
    return 0;
}

void interactive_collection_tick(int64_t dt_ms)
{
    g_tick_accumulator += dt_ms;
}

void interactive_collection_cleanup(void)
{
    player_resources_cleanup();
    memset(&g_registry, 0, sizeof(g_registry));
    g_tick_accumulator = 0;
}
