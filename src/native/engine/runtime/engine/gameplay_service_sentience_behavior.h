#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_SENTIENCE_BEHAVIOR_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_SENTIENCE_BEHAVIOR_H

#include "engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

const char *runtime_gameplay_sentience_behavior_mode_label(RuntimeWarSentienceBehaviorMode mode);

int runtime_gameplay_sentience_behavior_comeback_bonus(RuntimeWarSentienceBehaviorMode mode);

int runtime_gameplay_sentience_negotiate_deescalation_trim_for_streak(int streak_ticks);

void runtime_gameplay_sentience_behavior_spawn_offsets(RuntimeWarSentienceBehaviorMode mode,
                                                       int ordinal,
                                                       float *out_forward,
                                                       float *out_lateral);

void runtime_gameplay_sentience_behavior_apply_directionality(RuntimeWarSentienceBehaviorMode mode,
                                                              float lateral_sign,
                                                              float *inout_direction_x,
                                                              float *inout_direction_z);

#ifdef __cplusplus
}
#endif

#endif