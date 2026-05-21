/**
 * @file interactive_object_spawner.c
 * @brief Implementation of interactive object spawner
 */

#include "interactive_object_spawner.h"
#include "terrain_chunks.h"

#include <stdlib.h>
#include <string.h>

#define MAX_SPAWN_RULES 32
#define MAX_SPAWNED_OBJECTS 4096
#define CHUNK_TILES 64 /* 64x64 tiles per chunk */

typedef struct
{
    int object_id;
    int chunk_x;
    int chunk_z;
    int tile_x;
    int tile_z;
} SpawnedObject;

typedef struct
{
    BiomeSpawnRule rules[MAX_SPAWN_RULES];
    int rule_count;
    SpawnedObject objects[MAX_SPAWNED_OBJECTS];
    int object_count;
    int next_object_id;
} SpawnerRegistry;

static SpawnerRegistry g_spawner = {0};

int interactive_object_spawner_init(int max_rules)
{
    memset(&g_spawner, 0, sizeof(g_spawner));
    g_spawner.next_object_id = 1000; /* Start at 1000 to avoid collisions */
    return interactive_collection_init();
}

int interactive_object_spawner_register_rule(const BiomeSpawnRule *rule)
{
    if (!rule || g_spawner.rule_count >= MAX_SPAWN_RULES)
    {
        return -1;
    }

    if (rule->spawn_density < 0.0f || rule->spawn_density > 1.0f)
    {
        return -1;
    }

    memcpy(&g_spawner.rules[g_spawner.rule_count], rule, sizeof(BiomeSpawnRule));
    g_spawner.rule_count++;
    return 0;
}

static const BiomeSpawnRule *find_rule_for_biome(BiomeType biome)
{
    for (int i = 0; i < g_spawner.rule_count; i++)
    {
        if (g_spawner.rules[i].biome == biome)
        {
            return &g_spawner.rules[i];
        }
    }
    return NULL;
}

/* Simple pseudo-random generator based on position */
static int pseudo_random(int chunk_x, int chunk_z, int tile_x, int tile_z)
{
    int hash = 17;
    hash = hash * 31 + chunk_x;
    hash = hash * 31 + chunk_z;
    hash = hash * 31 + tile_x;
    hash = hash * 31 + tile_z;
    return abs(hash);
}

int interactive_object_spawner_spawn_chunk(const TerrainChunk *chunk, int chunk_x, int chunk_z)
{
    if (!chunk || g_spawner.object_count >= MAX_SPAWNED_OBJECTS)
    {
        return -1;
    }

    int spawned = 0;

    /* Iterate over chunk biome tiles (64x64) */
    for (int ty = 0; ty < CHUNK_TILES; ty++)
    {
        for (int tx = 0; tx < CHUNK_TILES; tx++)
        {
            BiomeType biome = (BiomeType)chunk->biomes[ty][tx];
            const BiomeSpawnRule *rule = find_rule_for_biome(biome);

            if (!rule)
            {
                continue; /* No spawn rule for this biome */
            }

            /* Check spawn density */
            int random_val = pseudo_random(chunk_x, chunk_z, tx, ty);
            float probability = (float)(random_val % 100) / 100.0f;

            if (probability > rule->spawn_density)
            {
                continue; /* Don't spawn this tile */
            }

            /* Spawn object */
            if (g_spawner.object_count >= MAX_SPAWNED_OBJECTS)
            {
                break;
            }

            int object_id = g_spawner.next_object_id++;
            float world_x = (float)(chunk_x * CHUNK_TILES + tx);
            float world_z = (float)(chunk_z * CHUNK_TILES + ty);

            /* Register with interactive collection system */
            int ret = interactive_collection_register(object_id, rule->resource_type,
                                                       rule->reward_amount, rule->max_collections,
                                                       rule->cooldown_ms);
            if (ret != 0)
            {
                continue;
            }

            /* Track spawned object */
            SpawnedObject *obj = &g_spawner.objects[g_spawner.object_count];
            obj->object_id = object_id;
            obj->chunk_x = chunk_x;
            obj->chunk_z = chunk_z;
            obj->tile_x = tx;
            obj->tile_z = ty;

            g_spawner.object_count++;
            spawned++;
        }
    }

    return spawned;
}

int interactive_object_spawner_count(void)
{
    return g_spawner.object_count;
}

int interactive_object_spawner_despawn_chunk(int chunk_x, int chunk_z)
{
    int removed = 0;

    /* Find and remove objects in chunk */
    for (int i = g_spawner.object_count - 1; i >= 0; i--)
    {
        if (g_spawner.objects[i].chunk_x == chunk_x && g_spawner.objects[i].chunk_z == chunk_z)
        {
            /* Shift remaining objects */
            if (i < g_spawner.object_count - 1)
            {
                memmove(&g_spawner.objects[i], &g_spawner.objects[i + 1],
                        (g_spawner.object_count - i - 1) * sizeof(SpawnedObject));
            }
            g_spawner.object_count--;
            removed++;
        }
    }

    return removed;
}

void interactive_object_spawner_cleanup(void)
{
    interactive_collection_cleanup();
    memset(&g_spawner, 0, sizeof(g_spawner));
    g_spawner.next_object_id = 1000;
}
