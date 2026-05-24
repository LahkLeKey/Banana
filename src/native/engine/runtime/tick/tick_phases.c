#include "tick_phases.h"

void runtime_phase_viewport_resize(Window *window,
                                   Renderer *renderer,
                                   int *viewport_width,
                                   int *viewport_height)
{
    int width = 0;
    int height = 0;

    if (!window || !renderer || !viewport_width || !viewport_height)
        return;

    window_get_size(window, &width, &height);
    if (width > 0 && height > 0 && (width != *viewport_width || height != *viewport_height))
    {
        *viewport_width = width;
        *viewport_height = height;
        renderer_resize(renderer, width, height);
    }
}

int runtime_phase_terrain_budget(void *context,
                                 RuntimeTerrainRebuildFn rebuild_dirty_chunks,
                                 int max_chunks)
{
    if (!rebuild_dirty_chunks)
        return -1;

    return rebuild_dirty_chunks(context, max_chunks) ? 0 : -1;
}