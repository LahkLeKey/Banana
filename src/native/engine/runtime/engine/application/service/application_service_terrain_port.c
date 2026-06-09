#include "application_service_terrain_port.h"

#include "../../../terrain/terrain_height.h"
#include "../../../terrain/terrain_host.h"
#include "../../../terrain/terrain_mutation.h"

static EngineRuntimeState *s_mutation_state = NULL;

float runtime_application_terrain_sample_height_port(const EngineRuntimeState *state,
                                                     float x,
                                                     float z)
{
    if (!state)
        return 0.f;

    return runtime_terrain_sample_height(&state->terrain_height[0][0],
                                         BANANA_ENGINE_TERRAIN_SIZE,
                                         x,
                                         z,
                                         0.48f,
                                         -1.45f);
}

int runtime_application_terrain_rebuild_dirty_chunks_port(EngineRuntimeState *state,
                                                          int max_chunks)
{
    if (!state)
        return 0;

    return runtime_terrain_host_rebuild_dirty(&state->terrain_height[0][0],
                                              BANANA_ENGINE_TERRAIN_SIZE,
                                              BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                              state->terrain_chunks,
                                              BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                                              BANANA_ENGINE_TERRAIN_CHUNK_ROWS,
                                              max_chunks);
}

void runtime_application_terrain_draw_port(EngineRuntimeState *state)
{
    if (!state)
        return;

    runtime_terrain_draw(state->renderer,
                         state->terrain_chunks,
                         state->terrain_initialized,
                         BANANA_ENGINE_TERRAIN_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                         BANANA_ENGINE_TERRAIN_CHUNK_ROWS);
}

static void mark_chunk_dirty_port(int chunk_x, int chunk_z)
{
    if (!s_mutation_state)
        return;

    runtime_terrain_host_mark_chunk_dirty(s_mutation_state->terrain_chunks,
                                          BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                                          BANANA_ENGINE_TERRAIN_CHUNK_ROWS,
                                          chunk_x,
                                          chunk_z);
}

int runtime_application_terrain_set_height_port(EngineRuntimeState *state,
                                                int x,
                                                int z,
                                                int elevation)
{
    int result = 0;

    if (!state)
        return -1;

    s_mutation_state = state;
    result = runtime_terrain_set_height(&state->terrain_height[0][0],
                                        BANANA_ENGINE_TERRAIN_SIZE,
                                        BANANA_ENGINE_TERRAIN_MAX_LAYERS,
                                        BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                        x,
                                        z,
                                        elevation,
                                        mark_chunk_dirty_port);
    s_mutation_state = NULL;
    return result;
}

void runtime_application_terrain_mark_region_dirty_port(EngineRuntimeState *state,
                                                        int min_x,
                                                        int min_z,
                                                        int max_x,
                                                        int max_z)
{
    if (!state)
        return;

    s_mutation_state = state;
    runtime_terrain_mark_region_dirty(BANANA_ENGINE_TERRAIN_SIZE,
                                      BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                      min_x,
                                      min_z,
                                      max_x,
                                      max_z,
                                      mark_chunk_dirty_port);
    s_mutation_state = NULL;
}