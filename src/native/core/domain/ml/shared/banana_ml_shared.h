#ifndef BANANA_ML_SHARED_H
#define BANANA_ML_SHARED_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int banana_ml_build_feature_vector(const char *input_json, BananaMlFeatureVector *out_features);

    double banana_ml_clamp01(double value);
    double banana_ml_sigmoid_approx(double value);

    void banana_ml_fill_confusion(int predicted_is_banana, int actual_is_banana,
                                  BananaMlConfusionMatrix *out_confusion);
    double banana_ml_jaccard_for_banana(const BananaMlConfusionMatrix *confusion);

#ifdef __cplusplus
}
#endif

#endif
