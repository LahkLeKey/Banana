#include "engine_lifecycle.h"

#include "terrain_generation.h"

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
    const float actor_positions[4][2] = {
        {-3.0f, -2.5f},
        {2.5f, -3.0f},
        {-2.0f, 2.75f},
        {3.25f, 2.25f},
    };
    const EntityType actor_types[4] = {
        ENTITY_TYPE_NPC,
        ENTITY_TYPE_DYNAMIC,
        ENTITY_TYPE_NPC,
        ENTITY_TYPE_DYNAMIC,
    };
    int i = 0;

    if (!world || !terrain_sample_height)
        return 0;

    for (i = 0; i < 4; i++)
    {
        float x = actor_positions[i][0];
        float z = actor_positions[i][1];
        float y = terrain_sample_height(x, z) + 0.55f;
        EntityId actor_id = world_spawn_entity(world, actor_types[i], x, y, z);
        Entity *actor = world_get_entity(world, actor_id);
        if (actor)
        {
            actor->scale[0] = 1.0f;
            actor->scale[1] = 1.0f;
            actor->scale[2] = 1.0f;

            if (actor->type == ENTITY_TYPE_NPC && attach_controller)
            {
                uint32_t controller_id = attach_controller(actor_id, "wildlife");
                if (controller_id != 0)
                    strncpy(actor->controller_kind, "wildlife", sizeof(actor->controller_kind) - 1);
            }
        }
    }

    return 4;
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
