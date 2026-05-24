#include "input_abi.h"

#include "input_contract.h"
#include "move_target_domain.h"

#include <math.h>

int runtime_input_abi_get_click_count(void)
{
    return runtime_input_contract_get_click_count();
}

int runtime_input_abi_get_target_reached_count(void)
{
    return runtime_input_contract_get_target_reached_count();
}

int runtime_input_abi_get_has_move_target(void)
{
    return runtime_input_contract_get_has_move_target();
}

int runtime_input_abi_canvas_to_normalized(RuntimeInputCanvasPoint canvas,
                                           RuntimeViewportSize viewport,
                                           RuntimeScreenNormalizedPoint *out_normalized)
{
    if (!out_normalized || viewport.width <= 0 || viewport.height <= 0)
        return -1;

    out_normalized->x = (canvas.x / (float)viewport.width) * 2.f - 1.f;
    out_normalized->y = 1.f - (canvas.y / (float)viewport.height) * 2.f;
    return 0;
}

int runtime_input_abi_handle_right_click(EngineRuntimeState *state,
                                         RuntimeInputCanvasPoint canvas,
                                         RuntimeTerrainSampleHeightFn sample_height)
{
    RuntimeViewportSize viewport;
    RuntimeScreenNormalizedPoint normalized;
    int accepted = runtime_input_contract_handle_right_click(canvas.x, canvas.y);

    if (!accepted || !state)
        return accepted;

    viewport.width = state->viewport_width;
    viewport.height = state->viewport_height;
    if (runtime_input_abi_canvas_to_normalized(canvas, viewport, &normalized) != 0)
        return accepted;

    if (runtime_input_contract_handle_right_click_normalized(normalized.x, normalized.y))
    {
        runtime_engine_composition_apply_click_input(state,
                                                     normalized.x,
                                                     normalized.y,
                                                     sample_height);
    }

    return accepted;
}

int runtime_input_abi_handle_right_click_normalized(EngineRuntimeState *state,
                                                    RuntimeScreenNormalizedPoint normalized,
                                                    RuntimeTerrainSampleHeightFn sample_height)
{
    int accepted = runtime_input_contract_handle_right_click_normalized(normalized.x,
                                                                        normalized.y);

    if (accepted && state)
    {
        runtime_engine_composition_apply_click_input(state,
                                                     normalized.x,
                                                     normalized.y,
                                                     sample_height);
    }

    return accepted;
}

void runtime_input_abi_set_move_input(EngineRuntimeState *state,
                                      float input_x,
                                      float input_z)
{
    if (!state)
        return;

    runtime_input_contract_sanitize_move_input(input_x,
                                               input_z,
                                               &state->move_input_x,
                                               &state->move_input_z);

    if (fabsf(state->move_input_x) > 1e-4f || fabsf(state->move_input_z) > 1e-4f)
    {
        runtime_move_target_clear(&state->move_target_state);
        runtime_input_contract_set_has_move_target(0);
    }
}