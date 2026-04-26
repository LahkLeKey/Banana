#include "banana_ml_regression.h"
#include "domain/ml/shared/banana_ml_shared.h"
#include "banana_status.h"

int banana_ml_regression_predict(const BananaMlFeatureVector* features, double* out_score) {
    double linear;
    double score;

    if (!features || !out_score) {
        return BANANA_INVALID_ARGUMENT;
    }

    /* Deterministic linear blend tuned for banana-vs-not-banana scoring. */
    linear = 0.20
        + (1.40 * features->banana_signal_ratio)
        - (1.10 * features->not_banana_signal_ratio)
        + (0.30 * features->unique_token_ratio)
        + (0.20 * features->length_ratio)
        + (0.40 * features->positive_bigram_ratio)
        - (0.30 * features->negative_bigram_ratio)
        + (0.35 * features->banana_attention_ratio)
        - (0.25 * features->not_banana_attention_ratio);

    score = banana_ml_sigmoid_approx((linear * 2.0) - 0.8);
    *out_score = banana_ml_clamp01(score);

    return BANANA_OK;
}