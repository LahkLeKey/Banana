/**
 * @file player_resources.c
 * @brief Player resource management implementation
 */

#include "player_resources.h"

#include <stdlib.h>
#include <string.h>

#define MAX_PLAYERS 256
#define PLAYER_GUID_MAX 64

/* Player resource registry */
static struct
{
    int count;
    PlayerResources entries[MAX_PLAYERS];
} g_player_resources = {0};

/* Resource type names */
static const char *g_resource_names[RESOURCE_COUNT] = {
    "wood",
    "ore",
    "stone",
};

int player_resources_init(void)
{
    g_player_resources.count = 0;
    memset(g_player_resources.entries, 0, sizeof(g_player_resources.entries));
    return 0;
}

PlayerResources *player_resources_get_or_create(const char *player_guid)
{
    if (!player_guid || strlen(player_guid) == 0)
    {
        return NULL;
    }

    /* Check if player exists */
    for (int i = 0; i < g_player_resources.count; i++)
    {
        if (strcmp(g_player_resources.entries[i].player_guid, player_guid) == 0)
        {
            return &g_player_resources.entries[i];
        }
    }

    /* Create new entry */
    if (g_player_resources.count >= MAX_PLAYERS)
    {
        return NULL; /* Registry full */
    }

    PlayerResources *entry = &g_player_resources.entries[g_player_resources.count++];
    entry->player_guid = player_guid;
    memset(entry->resources, 0, sizeof(entry->resources));
    entry->last_updated_tick = 0;

    return entry;
}

int player_resources_add(const char *player_guid, ResourceType resource_type, int amount)
{
    if (!player_guid || resource_type < 0 || resource_type >= RESOURCE_COUNT || amount <= 0)
    {
        return -1;
    }

    PlayerResources *player = player_resources_get_or_create(player_guid);
    if (!player)
    {
        return -1;
    }

    player->resources[resource_type] += amount;
    player->last_updated_tick = 0; /* Mark for sync */

    return 0;
}

int player_resources_subtract(const char *player_guid, ResourceType resource_type, int amount)
{
    if (!player_guid || resource_type < 0 || resource_type >= RESOURCE_COUNT || amount <= 0)
    {
        return -1;
    }

    PlayerResources *player = player_resources_get_or_create(player_guid);
    if (!player)
    {
        return -1;
    }

    if (player->resources[resource_type] < amount)
    {
        return -1; /* Insufficient resources */
    }

    player->resources[resource_type] -= amount;
    player->last_updated_tick = 0; /* Mark for sync */

    return 0;
}

int player_resources_get_balance(const char *player_guid, ResourceType resource_type)
{
    if (!player_guid || resource_type < 0 || resource_type >= RESOURCE_COUNT)
    {
        return -1;
    }

    for (int i = 0; i < g_player_resources.count; i++)
    {
        if (strcmp(g_player_resources.entries[i].player_guid, player_guid) == 0)
        {
            return g_player_resources.entries[i].resources[resource_type];
        }
    }

    return -1; /* Player not found */
}

int player_resources_transfer(const char *from_player, const char *to_player,
                               ResourceType resource_type, int amount)
{
    if (!from_player || !to_player || resource_type < 0 || resource_type >= RESOURCE_COUNT ||
        amount <= 0)
    {
        return -1;
    }

    /* Subtract from source */
    if (player_resources_subtract(from_player, resource_type, amount) != 0)
    {
        return -1;
    }

    /* Add to destination */
    if (player_resources_add(to_player, resource_type, amount) != 0)
    {
        /* Rollback on failure */
        player_resources_add(from_player, resource_type, amount);
        return -1;
    }

    return 0;
}

int player_resources_get_snapshot(const char *player_guid, int *out_resources)
{
    if (!player_guid || !out_resources)
    {
        return -1;
    }

    for (int i = 0; i < g_player_resources.count; i++)
    {
        if (strcmp(g_player_resources.entries[i].player_guid, player_guid) == 0)
        {
            memcpy(out_resources, g_player_resources.entries[i].resources,
                   sizeof(g_player_resources.entries[i].resources));
            return 0;
        }
    }

    return -1; /* Player not found */
}

int player_resources_reset(const char *player_guid)
{
    if (!player_guid)
    {
        return -1;
    }

    for (int i = 0; i < g_player_resources.count; i++)
    {
        if (strcmp(g_player_resources.entries[i].player_guid, player_guid) == 0)
        {
            memset(g_player_resources.entries[i].resources, 0,
                   sizeof(g_player_resources.entries[i].resources));
            g_player_resources.entries[i].last_updated_tick = 0;
            return 0;
        }
    }

    return -1; /* Player not found */
}

const char *player_resources_type_name(ResourceType type)
{
    if (type < 0 || type >= RESOURCE_COUNT)
    {
        return "unknown";
    }

    return g_resource_names[type];
}

void player_resources_cleanup(void)
{
    g_player_resources.count = 0;
    memset(g_player_resources.entries, 0, sizeof(g_player_resources.entries));
}
