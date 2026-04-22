#ifndef BANANA_ML_WRAPPER_SHARED_H
#define BANANA_ML_WRAPPER_SHARED_H

#include "../../../banana_wrapper_internal.h"

#include "../../../../core/domain/banana_ml_models.h"

int banana_ml_wrapper_copy_feature_vector(
    const double* features,
    int feature_count,
    BananaMlFeatureVector* out_vector
);

int banana_ml_wrapper_copy_transformer_input(
    const double* token_features,
    int token_feature_value_count,
    BananaMlTransformerInput* out_input
);

#endif