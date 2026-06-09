#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_SENTIENCE_OVERRIDES_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_SENTIENCE_OVERRIDES_H

#include "../../engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

void runtime_gameplay_apply_sentience_overrides(EngineRuntimeState *runtime_state);

RuntimeWarSentienceBehaviorMode runtime_gameplay_apply_sentience_mode_override(
    RuntimeWarSentienceBehaviorMode mode,
    int is_banana);

#ifdef __cplusplus
}
#endif

#endif