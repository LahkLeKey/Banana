#ifndef BANANA_ENGINE_RUNTIME_ENGINE_COMPOSITION_H
#define BANANA_ENGINE_RUNTIME_ENGINE_COMPOSITION_H

#include "../state/engine_state.h"
#include "../../player/player_motion.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef uint32_t (*RuntimeEngineAttachControllerFn)(uint32_t entity_id,
                                                        const char *type_name);

    int runtime_engine_composition_init(EngineRuntimeState *state,
                                        int width,
                                        int height,
                                        RuntimeTerrainSampleHeightFn sample_height,
                                        RuntimeEngineAttachControllerFn attach_controller);

    int runtime_engine_composition_tick(EngineRuntimeState *state,
                                        float dt,
                                        RuntimeTerrainSampleHeightFn sample_height);

    void runtime_engine_composition_shutdown(EngineRuntimeState *state);

    void runtime_engine_composition_apply_click_input(EngineRuntimeState *state,
                                                      float normalized_x,
                                                      float normalized_y,
                                                      RuntimeTerrainSampleHeightFn sample_height);

#ifdef __cplusplus
}
#endif

#endif
