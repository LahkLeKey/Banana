#ifndef BANANA_ENGINE_RUNTIME_RENDER_SUBMIT_H
#define BANANA_ENGINE_RUNTIME_RENDER_SUBMIT_H

#include "../render/material.h"
#include "../render/mesh.h"
#include "../render/renderer.h"
#include "../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef Material (*RuntimeMaterialResolver)(const Entity *entity);
    typedef void (*RuntimeTerrainDrawFn)(void);

    void runtime_render_submit_frame(Renderer *renderer,
                                     World *world,
                                     Mesh *entity_mesh,
                                     int terrain_initialized,
                                     RuntimeTerrainDrawFn terrain_draw,
                                     RuntimeMaterialResolver resolve_material);

#ifdef __cplusplus
}
#endif

#endif