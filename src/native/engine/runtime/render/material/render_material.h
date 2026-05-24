#ifndef BANANA_ENGINE_RUNTIME_RENDER_MATERIAL_H
#define BANANA_ENGINE_RUNTIME_RENDER_MATERIAL_H

#include "../../../render/material.h"
#include "../../../world/entity.h"

#ifdef __cplusplus
extern "C"
{
#endif

    Material runtime_render_material_for_entity(EntityType type);
    Material runtime_render_material_for_actor(const Entity *entity);

#ifdef __cplusplus
}
#endif

#endif
