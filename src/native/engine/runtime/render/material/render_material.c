#include "render_material.h"

#include "../../controller/kind/controller_kind_domain.h"

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
    RuntimeControllerKind kind = RUNTIME_CONTROLLER_KIND_UNKNOWN;

    if (!entity)
        return runtime_render_material_for_entity((EntityType)99);

    kind = runtime_controller_kind_parse_or_unknown(entity->controller_kind);

    if (entity->type == ENTITY_TYPE_NPC)
    {
        if (kind == RUNTIME_CONTROLLER_KIND_MERCHANT)
            return material_solid(0.95f, 0.55f, 0.15f, 1.f);
        if (kind == RUNTIME_CONTROLLER_KIND_COMBAT)
            return material_solid(0.85f, 0.22f, 0.22f, 1.f);
        if (kind == RUNTIME_CONTROLLER_KIND_WILDLIFE)
            return material_solid(0.25f, 0.80f, 0.35f, 1.f);
    }

    if (entity->type == ENTITY_TYPE_DYNAMIC && kind == RUNTIME_CONTROLLER_KIND_RESOURCE)
        return material_solid(0.20f, 0.65f, 0.85f, 1.f);

    if (entity->type == ENTITY_TYPE_TRIGGER && kind == RUNTIME_CONTROLLER_KIND_QUEST)
        return material_solid(0.75f, 0.35f, 0.90f, 1.f);

    if (entity->type == ENTITY_TYPE_STATIC && kind == RUNTIME_CONTROLLER_KIND_CAMP)
        return material_solid(0.62f, 0.48f, 0.30f, 1.f);

    if (entity->type == ENTITY_TYPE_STATIC && kind == RUNTIME_CONTROLLER_KIND_PBJ_PICKUP)
        return material_solid(0.82f, 0.54f, 0.22f, 1.f);

    return runtime_render_material_for_entity(entity->type);
}
