#include "engine_lifecycle.h"

#include "engine_aux_abi.h"
#include "engine_host.h"

#include "../controller/kind/controller_kind_domain.h"
#include "../terrain/terrain_generation.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int runtime_launch_gate_env_flag(const char *name)
{
    const char *value = getenv(name);

    if (!value || value[0] == '\0')
        return 0;

    if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0 || strcmp(value, "TRUE") == 0 || strcmp(value, "yes") == 0 || strcmp(value, "YES") == 0)
        return 1;

    return 0;
}

static const char *runtime_launch_gate_mode_env(void)
{
    return runtime_engine_host_launch_gate_mode_label_for(NULL);
}

int runtime_engine_lifecycle_preflight_launch_gate(EngineRuntimeState *state)
{
    banana_launch_gate_policy policy;
    banana_launch_gate_reason_code reason_code = BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE;
    int allow = 0;
    int has_steam_identity = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_HAS_STEAM_IDENTITY");
    int account_linked = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_ACCOUNT_LINKED");
    int account_in_good_standing = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_ACCOUNT_IN_GOOD_STANDING");
    int verification_fresh = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_VERIFICATION_FRESH");
    int verification_available = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_VERIFICATION_AVAILABLE");

    if (!state)
        return -1;

    if (runtime_engine_aux_launch_gate_policy_resolve(runtime_launch_gate_mode_env(), &policy) != 0)
    {
        runtime_engine_state_apply_launch_gate_decision(state, 0, BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE);
        fprintf(stderr, "[engine] launch gate blocked: unknown mode\n");
        return -1;
    }

    if (runtime_engine_aux_launch_gate_decide(&policy,
                                              has_steam_identity,
                                              account_linked,
                                              account_in_good_standing,
                                              verification_fresh,
                                              verification_available,
                                              &reason_code,
                                              &allow) != 0)
    {
        runtime_engine_state_apply_launch_gate_decision(state, 0, BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE);
        fprintf(stderr, "[engine] launch gate blocked: decision evaluation failed\n");
        return -1;
    }

    runtime_engine_state_apply_launch_gate_decision(state, allow, reason_code);
    if (!allow)
    {
        fprintf(stderr, "[engine] launch gate blocked: reason=%d state=%d\n",
                (int)reason_code,
                (int)state->launch_gate_blocked_state);
        return -1;
    }

    return 0;
}

static unsigned int runtime_spawn_hash(unsigned int value)
{
    unsigned int n = value * 374761393u + 20260523u;
    n = (n ^ (n >> 13)) * 1274126177u;
    return n ^ (n >> 16);
}

static float runtime_spawn_jitter(unsigned int seed)
{
    return (((float)(seed & 0xFFu) / 255.0f) - 0.5f) * 2.4f;
}

/* Sample center + footprint corners and return the highest ground point.
   This avoids actor corners clipping out of sloped terrain near map edges. */
static float runtime_spawn_sample_max_ground(RuntimeTerrainSampleFn terrain_sample_height,
                                             float x,
                                             float z,
                                             float sx,
                                             float sz)
{
    float half_x = sx * 0.5f;
    float half_z = sz * 0.5f;
    float max_h = terrain_sample_height(x, z);
    float h = 0.0f;

    h = terrain_sample_height(x - half_x, z - half_z);
    if (h > max_h)
        max_h = h;
    h = terrain_sample_height(x + half_x, z - half_z);
    if (h > max_h)
        max_h = h;
    h = terrain_sample_height(x - half_x, z + half_z);
    if (h > max_h)
        max_h = h;
    h = terrain_sample_height(x + half_x, z + half_z);
    if (h > max_h)
        max_h = h;

    return max_h;
}

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
    strncpy(player->controller_kind,
            runtime_controller_kind_name(RUNTIME_CONTROLLER_KIND_HUMAN),
            sizeof(player->controller_kind) - 1);

    runtime_player_registry_add_default(player_id,
                                        "native-default-player",
                                        "native-default",
                                        runtime_controller_kind_name(RUNTIME_CONTROLLER_KIND_HUMAN),
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
        {-0.20f, -0.16f, 0.55f, ENTITY_TYPE_NPC,     1.15f, 1.00f, 1.15f, "merchant", 0},
        {-0.62f, -0.38f, 0.40f, ENTITY_TYPE_DYNAMIC, 0.75f, 0.75f, 0.75f, "resource", 0},
        {-0.18f, -0.56f, 0.45f, ENTITY_TYPE_DYNAMIC, 0.85f, 0.90f, 0.85f, "resource", 0},
        { 0.66f, -0.42f, 0.55f, ENTITY_TYPE_NPC,     1.05f, 1.05f, 1.05f, "combat",   1},
        { 0.82f, -0.06f, 0.55f, ENTITY_TYPE_NPC,     0.95f, 1.10f, 0.95f, "wildlife", 1},
        {-0.56f,  0.66f, 0.45f, ENTITY_TYPE_TRIGGER, 0.95f, 1.30f, 0.95f, "quest",    0},
        /* Camp scaffold prop: keep it visibly above terrain so it does not read as a buried box. */
        {-0.82f,  0.78f, 0.72f, ENTITY_TYPE_STATIC,  1.20f, 0.90f, 1.20f, "camp",     0},
        { 0.22f,  0.18f, 0.42f, ENTITY_TYPE_STATIC,  1.25f, 1.05f, 1.25f, "pbj_pickup", 0},
        { 0.88f,  0.62f, 0.55f, ENTITY_TYPE_NPC,     1.00f, 1.00f, 1.00f, "wildlife", 1},
    };
    const float spread_radius = 18.0f;
    int spawned = 0;
    int i = 0;

    if (!world || !terrain_sample_height)
        return 0;

    for (i = 0; i < (int)(sizeof(actor_archetypes) / sizeof(actor_archetypes[0])); i++)
    {
        const RuntimeActorArchetype *archetype = &actor_archetypes[i];
        unsigned int hash = runtime_spawn_hash((unsigned int)(i + 1));
        float x = (archetype->x * spread_radius) + runtime_spawn_jitter(hash);
        float z = (archetype->z * spread_radius) + runtime_spawn_jitter(hash >> 8);
        float base_ground = runtime_spawn_sample_max_ground(terrain_sample_height,
                                    x,
                                    z,
                                    archetype->sx,
                                    archetype->sz);
        float y = base_ground + archetype->y_offset;
        EntityId actor_id = world_spawn_entity(world, archetype->type, x, y, z);
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
