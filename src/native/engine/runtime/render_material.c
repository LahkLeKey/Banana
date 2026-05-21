#include "render_material.h"

#include <string.h>

Material runtime_render_material_for_entity(EntityType type)
{
    switch (type)
    {
    case ENTITY_TYPE_NPC:
        return material_solid(0.20f, 0.80f, 0.30f, 1.f);
    case ENTITY_TYPE_DYNAMIC:
        return material_solid(0.30f, 0.55f, 0.90f, 1.f);
    case ENTITY_TYPE_PLAYER:
        return material_solid(0.95f, 0.75f, 0.10f, 1.f);
    default:
        return material_solid(0.55f, 0.55f, 0.55f, 1.f);
    }
}

Material runtime_render_material_for_actor(const Entity *entity)
{
    if (!entity)
        return runtime_render_material_for_entity((EntityType)99);

    if (entity->type == ENTITY_TYPE_NPC)
    {
        if (strcmp(entity->controller_kind, "merchant") == 0)
            return material_solid(0.95f, 0.55f, 0.15f, 1.f);
        if (strcmp(entity->controller_kind, "combat") == 0)
            return material_solid(0.85f, 0.22f, 0.22f, 1.f);
        if (strcmp(entity->controller_kind, "wildlife") == 0)
            return material_solid(0.25f, 0.80f, 0.35f, 1.f);
    }

    if (entity->type == ENTITY_TYPE_DYNAMIC && strcmp(entity->controller_kind, "resource") == 0)
        return material_solid(0.20f, 0.65f, 0.85f, 1.f);

    if (entity->type == ENTITY_TYPE_TRIGGER && strcmp(entity->controller_kind, "quest") == 0)
        return material_solid(0.75f, 0.35f, 0.90f, 1.f);

    if (entity->type == ENTITY_TYPE_STATIC && strcmp(entity->controller_kind, "camp") == 0)
        return material_solid(0.62f, 0.48f, 0.30f, 1.f);

    return runtime_render_material_for_entity(entity->type);
}
