#include "../shared/banana_ml_wrapper_shared.h"

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

    copy_status = banana_ml_wrapper_copy_transformer_input(token_features, token_feature_value_count, &input);
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