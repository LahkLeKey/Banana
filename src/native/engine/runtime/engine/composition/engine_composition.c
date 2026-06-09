#include "engine_composition.h"
#include "engine_composition_init.h"
#include "engine_composition_shutdown.h"

#include "../../orchestration/tick/runtime_tick_orchestration.h"

#include "../engine_tick.h"

int runtime_engine_composition_init(EngineRuntimeState *state,
                                    int width,
                                    int height,
                                    RuntimeTerrainSampleHeightFn sample_height,
                                    RuntimeEngineAttachControllerFn attach_controller)
{
    return runtime_engine_composition_init_state(state,
                                                 width,
                                                 height,
                                                 sample_height,
                                                 attach_controller);
}

int runtime_engine_composition_tick(EngineRuntimeState *state,
                                    float dt,
                                    RuntimeTerrainSampleHeightFn sample_height)
{
    int result = 0;
    RuntimeTickOrchestrationContext context = {0};

    if (!state || !sample_height)
        return -1;

    context.state = state;
    context.sample_height = sample_height;

    context.service_ports = state->service_ports;

    result = runtime_engine_tick_execute(state->window,
                                         state->renderer,
                                         state->physics,
                                         state->world,
                                         &state->viewport_width,
                                         &state->viewport_height,
                                         runtime_tick_orchestration_rebuild_dirty_chunks,
                                         state->controllers,
                                         state->controller_count,
                                         dt,
                                         &context,
                                         runtime_tick_orchestration_update_player_motion,
                                         runtime_tick_orchestration_follow_player_camera,
                                         runtime_tick_orchestration_apply_click_move_input,
                                         runtime_tick_orchestration_gameplay,
                                         runtime_tick_orchestration_render_scene);
    return result;
}

void runtime_engine_composition_shutdown(EngineRuntimeState *state)
{
    runtime_engine_composition_shutdown_state(state);
}

void runtime_engine_composition_apply_click_input(EngineRuntimeState *state,
                                                  float normalized_x,
                                                  float normalized_y,
                                                  RuntimeTerrainSampleHeightFn sample_height)
{
    if (!state || !sample_height || !state->service_ports || !state->service_ports->player.apply_click_input)
        return;

    state->service_ports->player.apply_click_input(state,
                                                   normalized_x,
                                                   normalized_y,
                                                   sample_height);
}
