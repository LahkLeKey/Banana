#include "gameplay_service_sentience_overrides.h"

#include "gameplay_service_sentience_override_config.h"

#include "gameplay_service_sentience_behavior.h"

void runtime_gameplay_apply_sentience_overrides(EngineRuntimeState *runtime_state)
{
    const RuntimeWarSentienceOverrides *overrides = runtime_gameplay_sentience_overrides_get();

    if (!runtime_state)
        return;

    if (overrides->has_humanoid_index)
        runtime_state->war_sentience_humanoid_index = overrides->humanoid_index;

    if (overrides->has_coordination_level)
        runtime_state->war_sentience_coordination_level = overrides->coordination_level;

    if (overrides->has_empathy_level)
        runtime_state->war_sentience_empathy_level = overrides->empathy_level;

    if (overrides->has_negotiate_streak_seed &&
        runtime_state->war_sentience_negotiate_streak_ticks < overrides->negotiate_streak_seed)
    {
        runtime_state->war_sentience_negotiate_streak_ticks = overrides->negotiate_streak_seed;
    }
}

RuntimeWarSentienceBehaviorMode runtime_gameplay_apply_sentience_mode_override(
    RuntimeWarSentienceBehaviorMode mode,
    int is_banana)
{
    const RuntimeWarSentienceOverrides *overrides = runtime_gameplay_sentience_overrides_get();

    if (is_banana && overrides->has_banana_mode_override)
        return overrides->banana_mode_override;

    if (!is_banana && overrides->has_bean_mode_override)
        return overrides->bean_mode_override;

    if (overrides->has_mode_override)
        return overrides->mode_override;

    return mode;
}