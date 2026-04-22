#include "../shared/banana_ml_wrapper_shared.h"

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

    copy_status = banana_ml_wrapper_copy_feature_vector(features, feature_count, &feature_vector);
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
    out_classification->jaccard_similarity = classification.jaccard_similarity;
    out_classification->confusion_true_positive = classification.confusion_true_positive;
    out_classification->confusion_false_positive = classification.confusion_false_positive;
    out_classification->confusion_false_negative = classification.confusion_false_negative;
    out_classification->confusion_true_negative = classification.confusion_true_negative;
    return BANANA_STATUS_OK;
}