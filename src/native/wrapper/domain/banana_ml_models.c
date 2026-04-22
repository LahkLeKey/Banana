#include "../banana_wrapper_internal.h"

#include "../../core/domain/banana_ml_models.h"

_Static_assert(
    CINTEROP_BANANA_ML_FEATURE_COUNT == BANANA_ML_FEATURE_COUNT,
    "Wrapper ABI feature count must match core model feature count.");
_Static_assert(
    CINTEROP_BANANA_ML_TOKEN_FEATURE_COUNT == BANANA_ML_TOKEN_FEATURE_COUNT,
    "Wrapper ABI token feature count must match core model token feature count.");
_Static_assert(
    CINTEROP_BANANA_ML_MAX_SEQUENCE_LENGTH == BANANA_ML_MAX_SEQUENCE_LENGTH,
    "Wrapper ABI max sequence length must match core model max sequence length.");
_Static_assert(
    CINTEROP_BANANA_ML_LABEL_NOT_BANANA == BANANA_ML_LABEL_NOT_BANANA,
    "Wrapper ABI NOT_BANANA label ordinal must match core model label ordinal.");
_Static_assert(
    CINTEROP_BANANA_ML_LABEL_BANANA == BANANA_ML_LABEL_BANANA,
    "Wrapper ABI BANANA label ordinal must match core model label ordinal.");

static int copy_feature_vector(
    const double* features,
    int feature_count,
    BananaMlFeatureVector* out_vector
) {
    int index = 0;

    if (features == 0 || out_vector == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    if (feature_count != CINTEROP_BANANA_ML_FEATURE_COUNT) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0; index < CINTEROP_BANANA_ML_FEATURE_COUNT; index++) {
        out_vector->values[index] = features[index];
    }

    return BANANA_STATUS_OK;
}

static int copy_transformer_input(
    const double* token_features,
    int token_feature_value_count,
    BananaMlTransformerInput* out_input
) {
    int token_count = 0;
    int token_index = 0;
    int feature_index = 0;
    int flat_index = 0;

    if (token_features == 0 || out_input == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    if (token_feature_value_count <= 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    if ((token_feature_value_count % CINTEROP_BANANA_ML_TOKEN_FEATURE_COUNT) != 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    token_count = token_feature_value_count / CINTEROP_BANANA_ML_TOKEN_FEATURE_COUNT;
    if (token_count > CINTEROP_BANANA_ML_MAX_SEQUENCE_LENGTH) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    out_input->token_count = token_count;

    for (token_index = 0; token_index < token_count; token_index++) {
        for (feature_index = 0; feature_index < CINTEROP_BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            out_input->tokens[token_index].values[feature_index] = token_features[flat_index];
            flat_index++;
        }
    }

    return BANANA_STATUS_OK;
}

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

    copy_status = copy_feature_vector(features, feature_count, &feature_vector);
    if (copy_status != BANANA_STATUS_OK) {
        return copy_status;
    }

    status = banana_ml_predict_regression_score(&feature_vector, out_score);
    return banana_wrapper_map_status(status);
}

int banana_classify_banana_binary(
    const double* features,
    int feature_count,
    CInteropBananaMlBinaryClassification* out_classification
) {
    BananaMlFeatureVector feature_vector;
    BananaMlBinaryClassification classification;
    BananaStatus status = BANANA_OK;
    int copy_status = BANANA_STATUS_OK;

    if (out_classification == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    copy_status = copy_feature_vector(features, feature_count, &feature_vector);
    if (copy_status != BANANA_STATUS_OK) {
        return copy_status;
    }

    status = banana_ml_predict_binary_classification(&feature_vector, &classification);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    out_classification->predicted_label = (CInteropBananaMlLabel)classification.predicted_label;
    out_classification->banana_probability = classification.banana_probability;
    out_classification->not_banana_probability = classification.not_banana_probability;
    out_classification->decision_margin = classification.decision_margin;
    return BANANA_STATUS_OK;
}

int banana_classify_banana_transformer(
    const double* token_features,
    int token_feature_value_count,
    CInteropBananaMlTransformerClassification* out_classification
) {
    BananaMlTransformerInput input;
    BananaMlTransformerClassification classification;
    BananaStatus status = BANANA_OK;
    int copy_status = BANANA_STATUS_OK;

    if (out_classification == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    copy_status = copy_transformer_input(token_features, token_feature_value_count, &input);
    if (copy_status != BANANA_STATUS_OK) {
        return copy_status;
    }

    status = banana_ml_predict_transformer_classification(&input, &classification);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    out_classification->predicted_label = (CInteropBananaMlLabel)classification.predicted_label;
    out_classification->banana_probability = classification.banana_probability;
    out_classification->not_banana_probability = classification.not_banana_probability;
    out_classification->attention_focus = classification.attention_focus;
    return BANANA_STATUS_OK;
}
