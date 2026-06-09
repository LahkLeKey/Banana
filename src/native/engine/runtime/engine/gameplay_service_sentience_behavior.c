#include "gameplay_service_sentience_behavior.h"

#include <math.h>

const char *runtime_gameplay_sentience_behavior_mode_label(RuntimeWarSentienceBehaviorMode mode)
{
    switch (mode)
    {
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
            return "flank";
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
            return "regroup";
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
            return "negotiate";
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
        default:
            return "hold";
    }
}

int runtime_gameplay_sentience_behavior_comeback_bonus(RuntimeWarSentienceBehaviorMode mode)
{
    if (mode == RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP)
        return 1;

    return 0;
}

int runtime_gameplay_sentience_negotiate_deescalation_trim_for_streak(int streak_ticks)
{
    int trim = 0;

    if (streak_ticks <= 0)
        return 0;

    trim = 1 + ((streak_ticks - 1) / 2);
    if (trim > 4)
        trim = 4;

    return trim;
}

void runtime_gameplay_sentience_behavior_spawn_offsets(RuntimeWarSentienceBehaviorMode mode,
                                                       int ordinal,
                                                       float *out_forward,
                                                       float *out_lateral)
{
    float forward = 1.75f + (float)(ordinal % 3) * 0.45f;
    float lateral = 0.60f + (float)(ordinal % 2) * 0.25f;

    switch (mode)
    {
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
            forward += 0.25f;
            lateral += 0.75f;
            break;
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
            forward = -(1.10f + (float)(ordinal % 3) * 0.25f);
            lateral = 0.18f + (float)(ordinal % 2) * 0.14f;
            break;
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
            forward = 0.95f + (float)(ordinal % 3) * 0.15f;
            lateral = 0.08f + (float)(ordinal % 2) * 0.06f;
            break;
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
        default:
            break;
    }

    if (out_forward)
        *out_forward = forward;
    if (out_lateral)
        *out_lateral = lateral;
}

void runtime_gameplay_sentience_behavior_apply_directionality(RuntimeWarSentienceBehaviorMode mode,
                                                              float lateral_sign,
                                                              float *inout_direction_x,
                                                              float *inout_direction_z)
{
    float dir_x = 0.0f;
    float dir_z = 0.0f;

    if (!inout_direction_x || !inout_direction_z)
        return;

    dir_x = *inout_direction_x;
    dir_z = *inout_direction_z;

    if (mode == RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK)
    {
        const float angle_radians = 0.5585053606f;
        float signed_angle = (lateral_sign >= 0.0f) ? angle_radians : -angle_radians;
        float cos_theta = cosf(signed_angle);
        float sin_theta = sinf(signed_angle);
        float rotated_x = (dir_x * cos_theta) - (dir_z * sin_theta);
        float rotated_z = (dir_x * sin_theta) + (dir_z * cos_theta);

        *inout_direction_x = rotated_x;
        *inout_direction_z = rotated_z;
    }
}