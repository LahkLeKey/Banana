#include "application_service_render_port.h"
#include "application_service_render_actor_resolver.h"
#include "application_service_render_model_cache.h"

#include "runtime/render/material/render_material.h"
#include "runtime/render/submit/render_submit.h"
#include "runtime/terrain/terrain_height.h"

static EngineRuntimeState *s_render_state = NULL;

static void terrain_draw_indirect(void)
{
    runtime_terrain_draw(s_render_state->renderer,
                         s_render_state->terrain_chunks,
                         s_render_state->terrain_initialized,
                         BANANA_ENGINE_TERRAIN_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                         BANANA_ENGINE_TERRAIN_CHUNK_ROWS);
}

void runtime_application_render_submit_scene_port(EngineRuntimeState *state)
{
    if (!state)
        return;

    s_render_state = state;
    runtime_application_render_actor_resolver_set_state(state);
    runtime_render_submit_frame(state->renderer,
                                state->world,
                                state->entity_mesh,
                                state->terrain_initialized,
                                terrain_draw_indirect,
                                runtime_render_material_for_actor,
                                runtime_application_render_mesh_for_actor,
                                runtime_application_render_command_for_actor);
    runtime_application_render_actor_resolver_set_state(NULL);
    s_render_state = NULL;
}

void runtime_application_render_ports_reset(void)
{
    runtime_application_render_model_cache_reset();
}