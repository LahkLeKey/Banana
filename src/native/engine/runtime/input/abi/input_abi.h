#ifndef BANANA_ENGINE_RUNTIME_INPUT_ABI_H
#define BANANA_ENGINE_RUNTIME_INPUT_ABI_H

#include "../../engine/composition/engine_composition.h"
#include "../../engine/state/engine_state.h"
#include "../move_target/move_target_service.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeInputCanvasPoint
    {
        float x;
        float y;
    } RuntimeInputCanvasPoint;

    int runtime_input_abi_get_click_count(void);

    int runtime_input_abi_get_target_reached_count(void);

    int runtime_input_abi_get_has_move_target(void);

    int runtime_input_abi_canvas_to_normalized(RuntimeInputCanvasPoint canvas,
                                               RuntimeViewportSize viewport,
                                               RuntimeScreenNormalizedPoint *out_normalized);

    int runtime_input_abi_handle_right_click(EngineRuntimeState *state,
                                             RuntimeInputCanvasPoint canvas,
                                             RuntimeTerrainSampleHeightFn sample_height);

    int runtime_input_abi_handle_right_click_normalized(EngineRuntimeState *state,
                                                        RuntimeScreenNormalizedPoint normalized,
                                                        RuntimeTerrainSampleHeightFn sample_height);

    void runtime_input_abi_set_move_input(EngineRuntimeState *state,
                                          float input_x,
                                          float input_z);

#ifdef __cplusplus
}
#endif

#endif