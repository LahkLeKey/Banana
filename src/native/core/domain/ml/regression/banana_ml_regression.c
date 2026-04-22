#include "../shared/banana_ml_internal.h"

static const double k_regression_weights[BANANA_ML_FEATURE_COUNT] = {
    0.23, -0.19, 0.31, 0.11, -0.08, 0.17, 0.21, -0.14
};

BananaStatus banana_ml_predict_regression_score(
    const BananaMlFeatureVector* features,
    double* out_score
) {
    BananaStatus status = BANANA_OK;
    double score = 0.0;

    if (out_score == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_ml_internal_validate_feature_vector(features);
    if (status != BANANA_OK) {
        return status;
    }

    score = banana_ml_internal_linear_score(k_regression_weights, features, 0.12);
    *out_score = banana_ml_internal_clamp(score, 0.0, 1.0);

    return BANANA_OK;
}