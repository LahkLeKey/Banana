#ifndef BANANA_ENGINE_RUNTIME_TICK_PHASES_H
#define BANANA_ENGINE_RUNTIME_TICK_PHASES_H

#include "../render/renderer.h"
#include "../render/window.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef int (*RuntimeTerrainRebuildFn)(void *context, int max_chunks);

    void runtime_phase_viewport_resize(Window *window,
                                       Renderer *renderer,
                                       int *viewport_width,
                                       int *viewport_height);

    int runtime_phase_terrain_budget(void *context,
                                     RuntimeTerrainRebuildFn rebuild_dirty_chunks,
                                     int max_chunks);

#ifdef __cplusplus
}
#endif

#endif