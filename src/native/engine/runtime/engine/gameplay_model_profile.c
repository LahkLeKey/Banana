#include "gameplay_model_profile.h"

#include <string.h>

int runtime_gameplay_model_vector_profile_for_model_id(const char *model_id,
                                                       float *out_radius_scale,
                                                       float *out_length_scale,
                                                       float *out_curve_scale,
                                                       float *out_tip_taper,
                                                       int *out_quality)
{
    if (!model_id || model_id[0] == '\0' ||
        !out_radius_scale || !out_length_scale || !out_curve_scale ||
        !out_tip_taper || !out_quality)
    {
        return 0;
    }

    if (strstr(model_id, "banana-bean-green") != NULL || strstr(model_id, "bean") != NULL)
    {
        *out_radius_scale = 1.32f;
        *out_length_scale = 0.72f;
        *out_curve_scale = 0.62f;
        *out_tip_taper = 0.22f;
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
