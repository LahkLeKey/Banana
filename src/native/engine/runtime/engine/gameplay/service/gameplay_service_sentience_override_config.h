#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_SENTIENCE_OVERRIDE_CONFIG_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_SENTIENCE_OVERRIDE_CONFIG_H

#include "../../engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RuntimeWarSentienceOverrides
{
    int initialized;
    int has_humanoid_index;
    int humanoid_index;
    int has_coordination_level;
    int coordination_level;
    int has_empathy_level;
    int empathy_level;
    int has_mode_override;
    RuntimeWarSentienceBehaviorMode mode_override;
    int has_banana_mode_override;
    RuntimeWarSentienceBehaviorMode banana_mode_override;
    int has_bean_mode_override;
    RuntimeWarSentienceBehaviorMode bean_mode_override;
    int has_negotiate_streak_seed;
    int negotiate_streak_seed;
} RuntimeWarSentienceOverrides;

const RuntimeWarSentienceOverrides *runtime_gameplay_sentience_overrides_get(void);

#ifdef __cplusplus
}
#endif

#endif