#include "../shared/banana_ml_internal.h"

static const double k_binary_weights[BANANA_ML_FEATURE_COUNT] = {
    1.40, -1.20, 1.10, 0.90, -1.30, 0.80, 1.00, -1.10
};

static double banana_ml_binary_jaccard_similarity(double tp, double fp, double fn) {
    double denominator = tp + fp + fn;

    if (denominator <= 0.0) {
        return 0.0;
    }

    return banana_ml_internal_clamp(tp / denominator, 0.0, 1.0);
}

static void banana_ml_binary_fill_soft_confusion_matrix(
    double banana_probability,
    double not_banana_probability,
    BananaMlBinaryClassification* out_classification
) {
    double tp = banana_probability * banana_probability;
    double fp = banana_probability * not_banana_probability;
    double fn = not_banana_probability * banana_probability;
    double tn = not_banana_probability * not_banana_probability;

    out_classification->confusion_true_positive = tp;
    out_classification->confusion_false_positive = fp;
    out_classification->confusion_false_negative = fn;
    out_classification->confusion_true_negative = tn;
    out_classification->jaccard_similarity = banana_ml_binary_jaccard_similarity(tp, fp, fn);
}

BananaStatus banana_ml_predict_binary_classification(
    const BananaMlFeatureVector* features,
    BananaMlBinaryClassification* out_classification
) {
    BananaStatus status = BANANA_OK;
    double margin = 0.0;
    double probability = 0.0;

    if (out_classification == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_ml_internal_validate_feature_vector(features);
    if (status != BANANA_OK) {
        return status;
    }

    margin = banana_ml_internal_linear_score(k_binary_weights, features, -0.20);

    probability = banana_ml_internal_clamp(
        banana_ml_internal_pseudo_sigmoid(margin),
        0.0,
        1.0
    );

    out_classification->predicted_label =
        probability >= 0.5 ? BANANA_ML_LABEL_BANANA : BANANA_ML_LABEL_NOT_BANANA;

    out_classification->banana_probability = probability;
    out_classification->not_banana_probability = 1.0 - probability;
    out_classification->decision_margin = margin;
    banana_ml_binary_fill_soft_confusion_matrix(
        out_classification->banana_probability,
        out_classification->not_banana_probability,
        out_classification);

    return BANANA_OK;
}