#include "engine_lifecycle.h"

#include "terrain_generation.h"

#include <stddef.h>
#include <string.h>

int runtime_engine_lifecycle_build_terrain(unsigned char *height_grid,
                                           int terrain_size,
                                           int terrain_max_layers,
                                           int chunk_size,
                                           RuntimeTerrainChunk *chunks,
                                           int chunk_cols,
                                           int chunk_rows)
{
    if (!runtime_terrain_generate_island(height_grid, terrain_size, terrain_max_layers))
        return 0;

    runtime_terrain_mark_all_chunks_dirty(chunks, chunk_cols * chunk_rows);
    return runtime_terrain_rebuild_dirty_chunks(height_grid,
                                                terrain_size,
                                                chunk_size,
                                                chunks,
                                                chunk_cols,
                                                chunk_rows,
                                                0);
}

int runtime_engine_lifecycle_bootstrap_primary_player(World *world,
                                                      EntityId player_id,
                                                      RuntimeTerrainSampleFn terrain_sample_height)
{
    Entity *player = NULL;

    if (!world || !player_id || !terrain_sample_height)
        return 0;

    player = world_get_entity(world, player_id);
    if (!player)
        return 0;

    player->position[1] = terrain_sample_height(0.f, 0.f) + 0.55f;
    player->scale[0] = 1.25f;
    player->scale[1] = 0.95f;
    player->scale[2] = 1.10f;
    strncpy(player->player_guid, "native-default-player", sizeof(player->player_guid) - 1);
    strncpy(player->controller_kind, "human", sizeof(player->controller_kind) - 1);

    runtime_player_registry_add_default(player_id,
                                        "native-default-player",
                                        "native-default",
                                        "human",
                                        1);

    return 1;
}

int runtime_engine_lifecycle_spawn_default_actors(World *world,
                                                  RuntimeTerrainSampleFn terrain_sample_height,
                                                  RuntimeAttachControllerFn attach_controller)
{
    typedef struct RuntimeActorArchetype {
        float x;
        float z;
        float y_offset;
        EntityType type;
        float sx;
        float sy;
        float sz;
        const char *role;
        int attach_ai;
    } RuntimeActorArchetype;

    static const RuntimeActorArchetype actor_archetypes[] = {
        {-4.2f, -2.8f, 0.55f, ENTITY_TYPE_NPC,     1.15f, 1.00f, 1.15f, "merchant", 0},
        {-2.6f, -3.4f, 0.40f, ENTITY_TYPE_DYNAMIC, 0.75f, 0.75f, 0.75f, "resource", 0},
        {-1.4f, -2.0f, 0.45f, ENTITY_TYPE_DYNAMIC, 0.85f, 0.90f, 0.85f, "resource", 0},
        { 3.2f, -2.9f, 0.55f, ENTITY_TYPE_NPC,     1.05f, 1.05f, 1.05f, "combat",   1},
        { 4.0f, -1.8f, 0.55f, ENTITY_TYPE_NPC,     0.95f, 1.10f, 0.95f, "wildlife", 1},
        {-2.4f,  2.8f, 0.45f, ENTITY_TYPE_TRIGGER, 0.95f, 1.30f, 0.95f, "quest",    0},
        {-3.7f,  3.6f, 0.35f, ENTITY_TYPE_STATIC,  1.40f, 0.60f, 1.40f, "camp",     0},
        { 1.8f,  1.6f, 0.42f, ENTITY_TYPE_STATIC,  1.25f, 1.05f, 1.25f, "pbj_pickup", 0},
        { 2.8f,  2.9f, 0.55f, ENTITY_TYPE_NPC,     1.00f, 1.00f, 1.00f, "wildlife", 1},
    };
    int spawned = 0;
    int i = 0;

    if (!world || !terrain_sample_height)
        return 0;

    for (i = 0; i < (int)(sizeof(actor_archetypes) / sizeof(actor_archetypes[0])); i++)
    {
        const RuntimeActorArchetype *archetype = &actor_archetypes[i];
        float y = terrain_sample_height(archetype->x, archetype->z) + archetype->y_offset;
        EntityId actor_id = world_spawn_entity(world, archetype->type, archetype->x, y, archetype->z);
        Entity *actor = world_get_entity(world, actor_id);
        if (actor)
        {
            actor->scale[0] = archetype->sx;
            actor->scale[1] = archetype->sy;
            actor->scale[2] = archetype->sz;
            if (archetype->role)
                strncpy(actor->controller_kind, archetype->role, sizeof(actor->controller_kind) - 1);

            if (actor->type == ENTITY_TYPE_NPC && archetype->attach_ai && attach_controller)
            {
                uint32_t controller_id = attach_controller(actor_id, "wildlife");
                if (controller_id != 0)
                    actor->controller_id = controller_id;
            }
            spawned += 1;
        }
    }

    return spawned;
}

void runtime_engine_lifecycle_destroy_controllers(ControllerInstance **controllers,
                                                  int max_controllers,
                                                  int *inout_controller_count)
{
    int i = 0;
    int count = 0;

    if (!controllers || max_controllers <= 0)
        return;

    if (inout_controller_count && *inout_controller_count > 0)
        count = *inout_controller_count;

    if (count > max_controllers)
        count = max_controllers;

    for (i = 0; i < count; i++)
    {
        controller_destroy(controllers[i]);
        controllers[i] = NULL;
    }

    if (inout_controller_count)
        *inout_controller_count = 0;
}

void runtime_engine_lifecycle_reset_terrain_chunks(RuntimeTerrainChunk *chunks,
                                                   int total_chunks)
{
    int i = 0;

    if (!chunks || total_chunks <= 0)
        return;

    for (i = 0; i < total_chunks; i++)
    {
        mesh_destroy(chunks[i].mesh);
        chunks[i].mesh = NULL;
        chunks[i].dirty = 0u;
    }
}
