#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_MODEL_PROFILE_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_MODEL_PROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_gameplay_model_vector_profile_for_model_id(const char *model_id,
                                                           float *out_radius_scale,
                                                           float *out_length_scale,
                                                           float *out_curve_scale,
                                                           float *out_tip_taper,
                                                           int *out_quality);

#ifdef __cplusplus
}
#endif

#endif
