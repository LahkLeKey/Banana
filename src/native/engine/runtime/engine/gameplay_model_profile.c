#include "gameplay_model_profile.h"

#include <string.h>

int runtime_gameplay_model_vector_profile_for_model_id(const char *model_id,
                                                       float *out_radius_scale,
                                                       float *out_length_scale,
                                                       float *out_curve_scale,
                                                       float *out_tip_taper,
                                                       int *out_quality)
{
    const int has_flank_tag = model_id && strstr(model_id, "flank") != NULL;
    const int has_regroup_tag = model_id && strstr(model_id, "regroup") != NULL;
    const int has_envoy_tag = model_id && strstr(model_id, "envoy") != NULL;
    const int has_truce_tag = model_id && strstr(model_id, "truce") != NULL;
    const int has_diplomacy_tag = has_envoy_tag || has_truce_tag;
    const int has_banana_tag = model_id && strstr(model_id, "banana-") != NULL;
    const int has_bean_tag = model_id && strstr(model_id, "bean-") != NULL;

    if (!model_id || model_id[0] == '\0' ||
        !out_radius_scale || !out_length_scale || !out_curve_scale ||
        !out_tip_taper || !out_quality)
    {
        return 0;
    }

    if (strstr(model_id, "archon") != NULL || strstr(model_id, "leviathan") != NULL)
    {
        *out_radius_scale = 2.06f;
        *out_length_scale = 1.34f;
        *out_curve_scale = 0.24f;
        *out_tip_taper = 0.06f;
        *out_quality = 3;

        if (strstr(model_id, "volcanic") != NULL)
        {
            *out_radius_scale += 0.12f;
            *out_curve_scale += 0.04f;
        }
        else if (strstr(model_id, "glacier") != NULL)
        {
            *out_curve_scale -= 0.06f;
            *out_tip_taper += 0.02f;
        }
        else if (strstr(model_id, "urban") != NULL)
        {
            *out_length_scale += 0.06f;
            *out_curve_scale -= 0.04f;
        }
        else if (strstr(model_id, "tropical") != NULL)
        {
            *out_curve_scale += 0.08f;
            *out_tip_taper += 0.02f;
        }

        if (has_flank_tag)
        {
            *out_length_scale += 0.10f;
            *out_curve_scale += 0.16f;
        }
        else if (has_regroup_tag)
        {
            *out_radius_scale += 0.14f;
            *out_curve_scale -= 0.18f;
            *out_tip_taper += 0.03f;
        }
        else if (has_diplomacy_tag)
        {
            *out_length_scale += 0.08f;
            *out_curve_scale -= 0.34f;
            *out_tip_taper += 0.09f;

            if (has_truce_tag)
            {
                *out_radius_scale += 0.06f;
                *out_curve_scale -= 0.06f;
                *out_tip_taper += 0.02f;
            }
        }

        if (has_banana_tag)
        {
            *out_length_scale += 0.05f;
            *out_curve_scale += 0.03f;
        }
        else if (has_bean_tag)
        {
            *out_radius_scale += 0.05f;
            *out_tip_taper += 0.02f;
        }

        return 1;
    }

    if (strstr(model_id, "phalanx") != NULL || strstr(model_id, "colossus") != NULL)
    {
        *out_radius_scale = 1.78f;
        *out_length_scale = 1.22f;
        *out_curve_scale = 0.32f;
        *out_tip_taper = 0.08f;
        *out_quality = 3;

        if (strstr(model_id, "volcanic") != NULL)
        {
            *out_radius_scale += 0.08f;
            *out_curve_scale += 0.05f;
        }
        else if (strstr(model_id, "glacier") != NULL)
        {
            *out_curve_scale -= 0.07f;
            *out_tip_taper += 0.03f;
        }
        else if (strstr(model_id, "urban") != NULL)
        {
            *out_length_scale += 0.05f;
            *out_curve_scale -= 0.03f;
        }
        else if (strstr(model_id, "tropical") != NULL)
        {
            *out_curve_scale += 0.07f;
            *out_tip_taper += 0.02f;
        }

        if (has_flank_tag)
        {
            *out_length_scale += 0.08f;
            *out_curve_scale += 0.14f;
        }
        else if (has_regroup_tag)
        {
            *out_radius_scale += 0.12f;
            *out_curve_scale -= 0.16f;
            *out_tip_taper += 0.02f;
        }
        else if (has_diplomacy_tag)
        {
            *out_length_scale += 0.07f;
            *out_curve_scale -= 0.28f;
            *out_tip_taper += 0.07f;

            if (has_truce_tag)
            {
                *out_radius_scale += 0.05f;
                *out_curve_scale -= 0.05f;
                *out_tip_taper += 0.02f;
            }
        }

        if (has_banana_tag)
        {
            *out_length_scale += 0.04f;
            *out_curve_scale += 0.02f;
        }
        else if (has_bean_tag)
        {
            *out_radius_scale += 0.04f;
            *out_tip_taper += 0.02f;
        }

        return 1;
    }

    if (strstr(model_id, "siege") != NULL ||
        strstr(model_id, "warbrute") != NULL ||
        strstr(model_id, "commander") != NULL)
    {
        *out_radius_scale = 1.52f;
        *out_length_scale = 1.08f;
        *out_curve_scale = 0.38f;
        *out_tip_taper = 0.10f;
        *out_quality = 3;

        if (strstr(model_id, "volcanic") != NULL)
        {
            *out_radius_scale += 0.10f;
            *out_curve_scale += 0.06f;
        }
        else if (strstr(model_id, "glacier") != NULL)
        {
            *out_curve_scale -= 0.08f;
            *out_tip_taper += 0.03f;
        }
        else if (strstr(model_id, "urban") != NULL)
        {
            *out_length_scale += 0.04f;
            *out_curve_scale -= 0.02f;
        }
        else if (strstr(model_id, "tropical") != NULL)
        {
            *out_curve_scale += 0.06f;
            *out_tip_taper += 0.02f;
        }

        if (has_banana_tag)
        {
            *out_length_scale += 0.04f;
            *out_curve_scale += 0.02f;
        }
        else if (has_bean_tag)
        {
            *out_radius_scale += 0.06f;
            *out_tip_taper += 0.02f;
        }

        if (has_flank_tag)
        {
            *out_length_scale += 0.08f;
            *out_curve_scale += 0.13f;
        }
        else if (has_regroup_tag)
        {
            *out_radius_scale += 0.12f;
            *out_curve_scale -= 0.14f;
            *out_tip_taper += 0.02f;
        }
        else if (has_diplomacy_tag)
        {
            *out_length_scale += 0.06f;
            *out_curve_scale -= 0.24f;
            *out_tip_taper += 0.06f;

            if (has_truce_tag)
            {
                *out_radius_scale += 0.04f;
                *out_curve_scale -= 0.04f;
                *out_tip_taper += 0.02f;
            }
        }

        return 1;
    }

    if (strstr(model_id, "scout") != NULL || strstr(model_id, "raider") != NULL)
    {
        *out_radius_scale = 0.92f;
        *out_length_scale = 1.42f;
        *out_curve_scale = 1.12f;
        *out_tip_taper = 0.20f;
        *out_quality = 3;

        if (has_flank_tag)
        {
            *out_length_scale += 0.06f;
            *out_curve_scale += 0.18f;
        }
        else if (has_regroup_tag)
        {
            *out_radius_scale += 0.10f;
            *out_curve_scale -= 0.22f;
        }
        else if (has_diplomacy_tag)
        {
            *out_length_scale += 0.05f;
            *out_curve_scale -= 0.36f;
            *out_tip_taper += 0.08f;

            if (has_truce_tag)
            {
                *out_radius_scale += 0.03f;
                *out_curve_scale -= 0.06f;
                *out_tip_taper += 0.02f;
            }
        }

        if (strstr(model_id, "urban") != NULL)
        {
            *out_curve_scale -= 0.10f;
            *out_length_scale += 0.08f;
        }
        else if (strstr(model_id, "tropical") != NULL)
        {
            *out_curve_scale += 0.12f;
            *out_tip_taper += 0.02f;
        }

        if (has_banana_tag)
        {
            *out_length_scale += 0.08f;
            *out_curve_scale += 0.06f;
            *out_tip_taper -= 0.02f;
        }
        else if (has_bean_tag)
        {
            *out_radius_scale += 0.09f;
            *out_curve_scale -= 0.08f;
            *out_tip_taper += 0.04f;
        }

        return 1;
    }

    if (strstr(model_id, "banana-bean-green") != NULL)
    {
        *out_radius_scale = 1.26f;
        *out_length_scale = 0.84f;
        *out_curve_scale = 0.70f;
        *out_tip_taper = 0.18f;
        *out_quality = 3;
        return 1;
    }

    if (strstr(model_id, "bean") != NULL)
    {
        *out_radius_scale = 1.38f;
        *out_length_scale = 0.68f;
        *out_curve_scale = 0.54f;
        *out_tip_taper = 0.26f;
        *out_quality = 3;
        return 1;
    }

    if (strstr(model_id, "reference/banana-basic") != NULL)
    {
        *out_radius_scale = 1.05f;
        *out_length_scale = 1.55f;
        *out_curve_scale = 1.65f;
        *out_tip_taper = 0.15f;
        *out_quality = 3;
        return 1;
    }

    if (strstr(model_id, "landmark") != NULL ||
        strstr(model_id, "arch") != NULL ||
        strstr(model_id, "gate") != NULL ||
        strstr(model_id, "hub") != NULL)
    {
        *out_radius_scale = 1.26f;
        *out_length_scale = 0.82f;
        *out_curve_scale = 0.20f;
        *out_tip_taper = 0.07f;
        *out_quality = 2;
        return 1;
    }

    if (strstr(model_id, "traversal") != NULL ||
        strstr(model_id, "cluster") != NULL ||
        strstr(model_id, "barrier") != NULL ||
        strstr(model_id, "stall") != NULL)
    {
        *out_radius_scale = 1.48f;
        *out_length_scale = 0.66f;
        *out_curve_scale = 0.46f;
        *out_tip_taper = 0.17f;
        *out_quality = 2;
        return 1;
    }

    *out_radius_scale = 1.12f;
    *out_length_scale = 1.10f;
    *out_curve_scale = 0.72f;
    *out_tip_taper = 0.12f;
    *out_quality = 2;
    return 1;
}
