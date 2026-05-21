#include "render_material.h"

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
