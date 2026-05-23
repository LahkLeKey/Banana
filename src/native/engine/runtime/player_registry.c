#include "player_registry.h"
#include "controller_kind_domain.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

static NativePlayerBinding s_player_bindings[BANANA_MAX_NATIVE_PLAYERS];
static int s_player_binding_count = 0;

static float clampf_local(float v, float lo, float hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

static int is_controller_ai(const char *controller_kind)
{
    return runtime_controller_kind_parse_or_unknown(controller_kind) == RUNTIME_CONTROLLER_KIND_AI;
}

static const char *normalize_controller_kind_or_default_human(const char *controller_kind)
{
    RuntimeControllerKind kind = runtime_controller_kind_parse_or_unknown(controller_kind);
    if (kind == RUNTIME_CONTROLLER_KIND_UNKNOWN)
        kind = RUNTIME_CONTROLLER_KIND_HUMAN;

    return runtime_controller_kind_name(kind);
}

void runtime_player_registry_reset(void)
{
    s_player_binding_count = 0;
    memset(s_player_bindings, 0, sizeof(s_player_bindings));
}

int runtime_player_registry_count(void)
{
    return s_player_binding_count;
}

NativePlayerBinding *runtime_player_registry_get(int index)
{
    if (index < 0 || index >= s_player_binding_count)
        return NULL;
    return &s_player_bindings[index];
}

NativePlayerBinding *runtime_player_registry_find(const char *guid)
{
    if (!guid || guid[0] == '\0')
        return NULL;

    for (int i = 0; i < s_player_binding_count; i++)
    {
        if (strncmp(s_player_bindings[i].guid, guid, sizeof(s_player_bindings[i].guid)) == 0)
            return &s_player_bindings[i];
    }

    return NULL;
}

void runtime_player_registry_add_default(EntityId entity_id,
                                         const char *guid,
                                         const char *name,
                                         const char *controller_kind,
                                         int active)
{
    NativePlayerBinding *binding = NULL;

    if (s_player_binding_count >= BANANA_MAX_NATIVE_PLAYERS)
        return;

    binding = &s_player_bindings[s_player_binding_count++];
    memset(binding, 0, sizeof(*binding));

    if (guid)
        strncpy(binding->guid, guid, sizeof(binding->guid) - 1);
    if (name)
        strncpy(binding->name, name, sizeof(binding->name) - 1);
    if (controller_kind)
        strncpy(binding->controller_kind,
                normalize_controller_kind_or_default_human(controller_kind),
                sizeof(binding->controller_kind) - 1);

    binding->entity_id = entity_id;
    binding->active = active ? 1 : 0;
}

NativePlayerBinding *runtime_player_registry_upsert(World *world,
                                                    const char *guid,
                                                    const char *name,
                                                    const char *controller_kind,
                                                    int active,
                                                    RuntimeTerrainSampleFn terrain_sample_height,
                                                    RuntimeControllerAttachFn attach_controller,
                                                    EntityId *inout_primary_player_id)
{
    NativePlayerBinding *binding = runtime_player_registry_find(guid);
    Entity *entity = NULL;

    if (!world || !guid || guid[0] == '\0' || !terrain_sample_height)
        return NULL;

    if (!binding)
    {
        if (s_player_binding_count >= BANANA_MAX_NATIVE_PLAYERS)
            return NULL;

        {
            int index = s_player_binding_count;
            float radius = 1.5f + ((float)(index % 8) * 0.55f);
            float angle = (float)(index % 24) * 0.42f;
            float x = cosf(angle) * radius;
            float z = sinf(angle) * radius;
            float y = terrain_sample_height(x, z) + 0.55f;

            binding = &s_player_bindings[s_player_binding_count++];
            memset(binding, 0, sizeof(*binding));
            strncpy(binding->guid, guid, sizeof(binding->guid) - 1);
            strncpy(binding->controller_kind,
                    normalize_controller_kind_or_default_human(controller_kind),
                    sizeof(binding->controller_kind) - 1);
            binding->entity_id = world_spawn_entity(world, ENTITY_TYPE_PLAYER, x, y, z);
            binding->active = active ? 1 : 0;
        }
    }

    if (!binding || binding->entity_id == 0)
        return NULL;

    if (name && name[0] != '\0')
        strncpy(binding->name, name, sizeof(binding->name) - 1);
    if (controller_kind && controller_kind[0] != '\0')
        strncpy(binding->controller_kind,
                normalize_controller_kind_or_default_human(controller_kind),
                sizeof(binding->controller_kind) - 1);
    binding->active = active ? 1 : 0;

    entity = world_get_entity(world, binding->entity_id);
    if (entity)
    {
        entity->type = ENTITY_TYPE_PLAYER;
        entity->active = binding->active;
        strncpy(entity->player_guid, binding->guid, sizeof(entity->player_guid) - 1);
        strncpy(entity->controller_kind, binding->controller_kind, sizeof(entity->controller_kind) - 1);
        if (is_controller_ai(binding->controller_kind) && entity->controller_id == 0 && attach_controller)
            entity->controller_id = attach_controller(entity->id, "wildlife");
    }

    if (inout_primary_player_id && *inout_primary_player_id == 0)
        *inout_primary_player_id = binding->entity_id;

    return binding;
}

void runtime_player_registry_apply_input(const char *guid, float input_x, float input_z)
{
    NativePlayerBinding *binding = runtime_player_registry_find(guid);
    if (!binding)
        return;

    if (!isfinite(input_x))
        input_x = 0.f;
    if (!isfinite(input_z))
        input_z = 0.f;

    binding->pending_move_x = clampf_local(input_x, -1.f, 1.f);
    binding->pending_move_z = clampf_local(input_z, -1.f, 1.f);
}

void runtime_player_registry_set_transform(World *world,
                                           const char *guid,
                                           float x,
                                           float y,
                                           float z,
                                           int active,
                                           float island_span,
                                           RuntimeTerrainSampleFn terrain_sample_height)
{
    NativePlayerBinding *binding = NULL;
    Entity *entity = NULL;
    float next_x = 0.f;
    float next_y = 0.f;
    float next_z = 0.f;

    if (!world)
        return;

    binding = runtime_player_registry_upsert(world,
                                             guid,
                                             NULL,
                                             runtime_controller_kind_name(RUNTIME_CONTROLLER_KIND_HUMAN),
                                             active,
                                             terrain_sample_height,
                                             NULL,
                                             NULL);
    if (!binding)
        return;

    entity = world_get_entity(world, binding->entity_id);
    if (!entity)
        return;

    next_x = isfinite(x) ? x : entity->position[0];
    next_z = isfinite(z) ? z : entity->position[2];
    next_y = isfinite(y) ? y : terrain_sample_height(next_x, next_z) + 0.55f;

    entity->position[0] = clampf_local(next_x, -island_span, island_span);
    entity->position[2] = clampf_local(next_z, -island_span, island_span);
    entity->position[1] = next_y;
    entity->active = active ? 1 : 0;
    binding->active = entity->active;
}

/* Multiplayer sync ownership: staleness tracking */

void runtime_player_registry_mark_seen(const char *guid, int64_t current_time_ms)
{
    NativePlayerBinding *binding = runtime_player_registry_find(guid);
    if (!binding)
        return;

    binding->last_seen_ms = current_time_ms;
    binding->is_stale = 0;
}

void runtime_player_registry_update_staleness(int64_t current_time_ms, int64_t stale_threshold_ms)
{
    for (int i = 0; i < s_player_binding_count; i++)
    {
        NativePlayerBinding *binding = &s_player_bindings[i];
        if (binding->entity_id == 0)
            continue;

        /* If player has never been seen, assume fresh (just added) */
        if (binding->last_seen_ms == 0)
        {
            binding->last_seen_ms = current_time_ms;
            binding->is_stale = 0;
            continue;
        }

        /* Check staleness against threshold */
        int64_t age_ms = current_time_ms - binding->last_seen_ms;
        binding->is_stale = (age_ms > stale_threshold_ms) ? 1 : 0;
    }
}

void runtime_player_registry_deactivate_stale(void)
{
    for (int i = 0; i < s_player_binding_count; i++)
    {
        NativePlayerBinding *binding = &s_player_bindings[i];
        if (binding->entity_id == 0)
            continue;

        /* Only deactivate if marked stale and currently active */
        if (binding->is_stale && binding->active)
        {
            binding->active = 0;
            /* Note: entity will be marked inactive in the next sync */
        }
    }
}

int runtime_player_registry_count_active(void)
{
    int active_count = 0;
    for (int i = 0; i < s_player_binding_count; i++)
    {
        if (s_player_bindings[i].entity_id != 0 && s_player_bindings[i].active && !s_player_bindings[i].is_stale)
            active_count++;
    }
    return active_count;
}

void runtime_player_registry_add_resource_key(const char *guid,
                                              RuntimeResourceKey resource_key,
                                              int amount)
{
    if (!guid || resource_key < 0 || resource_key >= RUNTIME_RESOURCE_COUNT)
        return;

    NativePlayerBinding *binding = runtime_player_registry_find(guid);
    if (!binding)
        return;

    if (resource_key == RUNTIME_RESOURCE_WOOD)
    {
        binding->wood_count += amount;
        if (binding->wood_count < 0)
            binding->wood_count = 0;
    }
    else if (resource_key == RUNTIME_RESOURCE_ORE)
    {
        binding->ore_count += amount;
        if (binding->ore_count < 0)
            binding->ore_count = 0;
    }
    else if (resource_key == RUNTIME_RESOURCE_GOLD)
    {
        binding->gold_count += amount;
        if (binding->gold_count < 0)
            binding->gold_count = 0;
    }
}

int runtime_player_registry_get_resource_key(const char *guid,
                                             RuntimeResourceKey resource_key)
{
    if (!guid || resource_key < 0 || resource_key >= RUNTIME_RESOURCE_COUNT)
        return 0;

    NativePlayerBinding *binding = runtime_player_registry_find(guid);
    if (!binding)
        return 0;

    if (resource_key == RUNTIME_RESOURCE_WOOD)
        return binding->wood_count;
    else if (resource_key == RUNTIME_RESOURCE_ORE)
        return binding->ore_count;
    else if (resource_key == RUNTIME_RESOURCE_GOLD)
        return binding->gold_count;

    return 0;
}

void runtime_player_registry_add_resource(const char *guid, const char *resource_type, int amount)
{
    RuntimeResourceKey resource_key;
    if (runtime_resource_parse_key(resource_type, &resource_key) != 0)
        return;

    runtime_player_registry_add_resource_key(guid, resource_key, amount);
}

int runtime_player_registry_get_resource(const char *guid, const char *resource_type)
{
    RuntimeResourceKey resource_key;
    if (runtime_resource_parse_key(resource_type, &resource_key) != 0)
        return 0;

    return runtime_player_registry_get_resource_key(guid, resource_key);
}