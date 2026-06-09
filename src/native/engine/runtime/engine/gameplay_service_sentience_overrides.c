#include "gameplay_service_sentience_overrides.h"

#include "gameplay_service_sentience_behavior.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static RuntimeWarSentienceOverrides s_war_sentience_overrides = {0};

static int runtime_gameplay_parse_env_int(const char *name, int *out_value)
{
    const char *raw = getenv(name);
    long parsed = 0;

    if (!raw || raw[0] == '\0')
        return 0;

    parsed = strtol(raw, NULL, 10);
    if (parsed < 0)
        parsed = 0;

    if (out_value)
        *out_value = (int)parsed;

    return 1;
}

static int runtime_gameplay_parse_sentience_mode(const char *raw, RuntimeWarSentienceBehaviorMode *out_mode)
{
    if (!raw || raw[0] == '\0' || !out_mode)
        return 0;

    if (strcmp(raw, "hold") == 0 ||
        strcmp(raw, "hold-line") == 0 ||
        strcmp(raw, "hold_line") == 0 ||
        strcmp(raw, "holdline") == 0)
    {
        *out_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
        return 1;
    }

    if (strcmp(raw, "flank") == 0)
    {
        *out_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK;
        return 1;
    }

    if (strcmp(raw, "regroup") == 0)
    {
        *out_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP;
        return 1;
    }

    if (strcmp(raw, "negotiate") == 0 || strcmp(raw, "truce") == 0)
    {
        *out_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE;
        return 1;
    }

    return 0;
}

static void runtime_gameplay_init_war_sentience_overrides(void)
{
    RuntimeWarSentienceBehaviorMode mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
    int value = 0;
    int has_override = 0;

    if (s_war_sentience_overrides.initialized)
        return;

    s_war_sentience_overrides.initialized = 1;

    if (runtime_gameplay_parse_env_int("BANANA_WAR_SENTIENCE_HUMANOID_INDEX", &value))
    {
        s_war_sentience_overrides.has_humanoid_index = 1;
        s_war_sentience_overrides.humanoid_index = value;
        has_override = 1;
    }

    if (runtime_gameplay_parse_env_int("BANANA_WAR_SENTIENCE_COORDINATION_LEVEL", &value))
    {
        s_war_sentience_overrides.has_coordination_level = 1;
        s_war_sentience_overrides.coordination_level = value;
        has_override = 1;
    }

    if (runtime_gameplay_parse_env_int("BANANA_WAR_SENTIENCE_EMPATHY_LEVEL", &value))
    {
        s_war_sentience_overrides.has_empathy_level = 1;
        s_war_sentience_overrides.empathy_level = value;
        has_override = 1;
    }

    if (runtime_gameplay_parse_env_int("BANANA_WAR_SENTIENCE_NEGOTIATE_STREAK_SEED", &value))
    {
        s_war_sentience_overrides.has_negotiate_streak_seed = 1;
        s_war_sentience_overrides.negotiate_streak_seed = value;
        has_override = 1;
    }

    if (runtime_gameplay_parse_sentience_mode(getenv("BANANA_WAR_SENTIENCE_MODE_OVERRIDE"), &mode))
    {
        s_war_sentience_overrides.has_mode_override = 1;
        s_war_sentience_overrides.mode_override = mode;
        has_override = 1;
    }

    if (runtime_gameplay_parse_sentience_mode(getenv("BANANA_WAR_SENTIENCE_MODE_OVERRIDE_BANANA"), &mode))
    {
        s_war_sentience_overrides.has_banana_mode_override = 1;
        s_war_sentience_overrides.banana_mode_override = mode;
        has_override = 1;
    }

    if (runtime_gameplay_parse_sentience_mode(getenv("BANANA_WAR_SENTIENCE_MODE_OVERRIDE_BEAN"), &mode))
    {
        s_war_sentience_overrides.has_bean_mode_override = 1;
        s_war_sentience_overrides.bean_mode_override = mode;
        has_override = 1;
    }

    if (has_override)
    {
        const char *mode_label = s_war_sentience_overrides.has_mode_override
                                     ? runtime_gameplay_sentience_behavior_mode_label(
                                           s_war_sentience_overrides.mode_override)
                                     : "default";
        const char *banana_label = s_war_sentience_overrides.has_banana_mode_override
                                       ? runtime_gameplay_sentience_behavior_mode_label(
                                             s_war_sentience_overrides.banana_mode_override)
                                       : "default";
        const char *bean_label = s_war_sentience_overrides.has_bean_mode_override
                                     ? runtime_gameplay_sentience_behavior_mode_label(
                                           s_war_sentience_overrides.bean_mode_override)
                                     : "default";

        printf("[runtime] war-sentience-overrides humanoid=%d coord=%d empathy=%d mode=%s banana=%s bean=%s negotiate=%d\n",
               s_war_sentience_overrides.has_humanoid_index ? s_war_sentience_overrides.humanoid_index : -1,
               s_war_sentience_overrides.has_coordination_level ? s_war_sentience_overrides.coordination_level : -1,
               s_war_sentience_overrides.has_empathy_level ? s_war_sentience_overrides.empathy_level : -1,
               mode_label,
               banana_label,
               bean_label,
               s_war_sentience_overrides.has_negotiate_streak_seed
                   ? s_war_sentience_overrides.negotiate_streak_seed
                   : -1);
    }
}

void runtime_gameplay_apply_sentience_overrides(EngineRuntimeState *runtime_state)
{
    runtime_gameplay_init_war_sentience_overrides();

    if (!runtime_state)
        return;

    if (s_war_sentience_overrides.has_humanoid_index)
        runtime_state->war_sentience_humanoid_index = s_war_sentience_overrides.humanoid_index;

    if (s_war_sentience_overrides.has_coordination_level)
        runtime_state->war_sentience_coordination_level = s_war_sentience_overrides.coordination_level;

    if (s_war_sentience_overrides.has_empathy_level)
        runtime_state->war_sentience_empathy_level = s_war_sentience_overrides.empathy_level;

    if (s_war_sentience_overrides.has_negotiate_streak_seed &&
        runtime_state->war_sentience_negotiate_streak_ticks < s_war_sentience_overrides.negotiate_streak_seed)
    {
        runtime_state->war_sentience_negotiate_streak_ticks = s_war_sentience_overrides.negotiate_streak_seed;
    }
}

RuntimeWarSentienceBehaviorMode runtime_gameplay_apply_sentience_mode_override(
    RuntimeWarSentienceBehaviorMode mode,
    int is_banana)
{
    runtime_gameplay_init_war_sentience_overrides();

    if (is_banana && s_war_sentience_overrides.has_banana_mode_override)
        return s_war_sentience_overrides.banana_mode_override;

    if (!is_banana && s_war_sentience_overrides.has_bean_mode_override)
        return s_war_sentience_overrides.bean_mode_override;

    if (s_war_sentience_overrides.has_mode_override)
        return s_war_sentience_overrides.mode_override;

    return mode;
}