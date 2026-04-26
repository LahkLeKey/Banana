#include "banana_ml_models.h"
#include "banana_status.h"
#include "domain/ml/shared/banana_ml_shared.h"
#include "domain/ml/regression/banana_ml_regression.h"
#include "domain/ml/binary/banana_ml_binary.h"
#include "domain/ml/transformer/banana_ml_transformer.h"

#include <string.h>

int banana_ml_predict_regression_score(const char* input_json, double* out_score) {
    BananaMlFeatureVector features;
    int rc;

    if (!input_json || !out_score) {
        return BANANA_INVALID_ARGUMENT;
    }

    rc = banana_ml_build_feature_vector(input_json, &features);
    if (rc != BANANA_OK) {
        return rc;
    }

    return banana_ml_regression_predict(&features, out_score);
}

int banana_ml_classify_binary(const char* input_json, BananaMlClassificationResult* out_result) {
    BananaMlFeatureVector features;
    int rc;

    if (!input_json || !out_result) {
        return BANANA_INVALID_ARGUMENT;
    }

    memset(out_result, 0, sizeof(*out_result));

    rc = banana_ml_build_feature_vector(input_json, &features);
    if (rc != BANANA_OK) {
        return rc;
    }

    return banana_ml_binary_classify(&features, out_result);
}

int banana_ml_classify_not_banana_junk(const char* input_json,
                                       BananaMlClassificationResult* out_result) {
    BananaMlFeatureVector features;
    int rc;

    if (!input_json || !out_result) {
        return BANANA_INVALID_ARGUMENT;
    }

    memset(out_result, 0, sizeof(*out_result));

    rc = banana_ml_build_feature_vector(input_json, &features);
    if (rc != BANANA_OK) {
        return rc;
    }

    return banana_ml_binary_classify_with_not_banana_bias(&features, out_result);
}

int banana_ml_classify_transformer(const char* input_json, BananaMlClassificationResult* out_result) {
    BananaMlFeatureVector features;
    int rc;

    if (!input_json || !out_result) {
        return BANANA_INVALID_ARGUMENT;
    }

    memset(out_result, 0, sizeof(*out_result));

    rc = banana_ml_build_feature_vector(input_json, &features);
    if (rc != BANANA_OK) {
        return rc;
    }

    return banana_ml_transformer_classify(&features, out_result);
}