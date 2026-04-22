#include "../shared/banana_ml_wrapper_shared.h"

int banana_predict_banana_regression_score(
    const double* features,
    int feature_count,
    double* out_score
) {
    BananaMlFeatureVector feature_vector;
    BananaStatus status = BANANA_OK;
    int copy_status = BANANA_STATUS_OK;

    if (out_score == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    copy_status = banana_ml_wrapper_copy_feature_vector(features, feature_count, &feature_vector);
    if (copy_status != BANANA_STATUS_OK) {
        return copy_status;
    }

    status = banana_ml_predict_regression_score(&feature_vector, out_score);
    return banana_wrapper_map_status(status);
}