#ifndef BANANA_ENGINE_RUNTIME_ENGINE_COMPOSITION_INIT_H
#define BANANA_ENGINE_RUNTIME_ENGINE_COMPOSITION_INIT_H

#include "engine_composition.h"

#ifdef __cplusplus
extern "C"
{
#endif

int runtime_engine_composition_init_state(EngineRuntimeState *state,
                                          int width,
                                          int height,
                                          RuntimeTerrainSampleHeightFn sample_height,
                                          RuntimeEngineAttachControllerFn attach_controller);

#ifdef __cplusplus
}
#endif

#endif