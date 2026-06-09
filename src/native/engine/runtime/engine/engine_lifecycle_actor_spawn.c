#include "engine_lifecycle.h"
#include "engine_lifecycle_spawn_math.h"

#include <string.h>

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
        const char *gameplay_model_id;
        const char *controller_type;
        int attach_ai;
    } RuntimeActorArchetype;

    static const RuntimeActorArchetype actor_archetypes[] = {
        {-0.20f, -0.16f, 0.55f, ENTITY_TYPE_NPC,     1.15f, 1.00f, 1.15f, "merchant",   "gameplay/reference/banana-basic-v1", NULL, 0},
        {-0.62f, -0.38f, 0.40f, ENTITY_TYPE_DYNAMIC, 0.75f, 0.75f, 0.75f, "resource",   "gameplay/tropical-coastal/palm-cluster-v1", NULL, 0},
        {-0.18f, -0.56f, 0.45f, ENTITY_TYPE_DYNAMIC, 0.85f, 0.90f, 0.85f, "resource",   "gameplay/urban-industrial/platform-barrier-v1", NULL, 0},
        { 0.66f, -0.42f, 0.55f, ENTITY_TYPE_NPC,     1.05f, 1.05f, 1.05f, "combat",     "gameplay/reference/banana-basic-v1", "combat", 1},
        { 0.82f, -0.06f, 0.55f, ENTITY_TYPE_NPC,     0.95f, 1.10f, 0.95f, "wildlife",   "gameplay/reference/banana-bean-green-v1", "wildlife", 1},
        {-0.56f,  0.66f, 0.45f, ENTITY_TYPE_TRIGGER, 0.95f, 1.30f, 0.95f, "quest",      "gameplay/urban-industrial/market-stall-v1", NULL, 0},
        /* Camp scaffold prop: keep it visibly above terrain so it does not read as a buried box. */
        {-0.82f,  0.78f, 0.72f, ENTITY_TYPE_STATIC,  1.20f, 0.90f, 1.20f, "camp",       "gameplay/tropical-coastal/volcanic-arch-v1", NULL, 0},
        { 0.22f,  0.18f, 0.42f, ENTITY_TYPE_STATIC,  1.25f, 1.05f, 1.25f, "pbj_pickup", "gameplay/reference/banana-basic-v1", NULL, 0},
        { 0.88f,  0.62f, 0.55f, ENTITY_TYPE_NPC,     1.00f, 1.00f, 1.00f, "wildlife",   "gameplay/reference/banana-bean-green-v1", "wildlife", 1},
    };
    const float spread_radius = 18.0f;
    int spawned = 0;
    int i = 0;

    if (!world || !terrain_sample_height)
        return 0;

    for (i = 0; i < (int)(sizeof(actor_archetypes) / sizeof(actor_archetypes[0])); i++)
    {
        const RuntimeActorArchetype *archetype = &actor_archetypes[i];
        unsigned int hash = runtime_engine_lifecycle_spawn_hash((unsigned int)(i + 1));
        float x = (archetype->x * spread_radius) + runtime_engine_lifecycle_spawn_jitter(hash);
        float z = (archetype->z * spread_radius) + runtime_engine_lifecycle_spawn_jitter(hash >> 8);
        float base_ground = runtime_engine_lifecycle_spawn_sample_max_ground(terrain_sample_height,
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
            if (archetype->gameplay_model_id)
            {
                strncpy(actor->gameplay_model_id,
                        archetype->gameplay_model_id,
                        sizeof(actor->gameplay_model_id) - 1);
                actor->gameplay_model_id[sizeof(actor->gameplay_model_id) - 1] = '\0';
            }

            if (actor->type == ENTITY_TYPE_NPC && archetype->attach_ai && attach_controller)
            {
                const char *controller_type =
                    (archetype->controller_type && archetype->controller_type[0] != '\0')
                        ? archetype->controller_type
                        : "wildlife";
                uint32_t controller_id = attach_controller(actor_id, controller_type);
                if (controller_id != 0)
                    actor->controller_id = controller_id;
            }
            spawned += 1;
        }
    }

    return spawned;
}