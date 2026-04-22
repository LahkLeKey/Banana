#include "banana_ml_wrapper_shared.h"

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

int banana_ml_wrapper_copy_feature_vector(
    const double* features,
    int feature_count,
    BananaMlFeatureVector* out_vector
) {
    if (features == 0 || out_vector == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    if (feature_count != CINTEROP_BANANA_ML_FEATURE_COUNT) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    for (int index = 0; index < CINTEROP_BANANA_ML_FEATURE_COUNT; index++) {
        out_vector->values[index] = features[index];
    }

    return BANANA_STATUS_OK;
}

int banana_ml_wrapper_copy_transformer_input(
    const double* token_features,
    int token_feature_value_count,
    BananaMlTransformerInput* out_input
) {
    int token_count = 0;
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

    for (int token_index = 0; token_index < token_count; token_index++) {
        for (int feature_index = 0; feature_index < CINTEROP_BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            out_input->tokens[token_index].values[feature_index] = token_features[flat_index];
            flat_index++;
        }
    }

    return BANANA_STATUS_OK;
}