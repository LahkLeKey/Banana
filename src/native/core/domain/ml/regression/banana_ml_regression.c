#include "banana_ml_regression.h"
#include "domain/ml/shared/banana_ml_shared.h"
#include "banana_status.h"

#include <math.h>

/*
 * Returns 1 if v is a finite double in [0.0, 1.0], 0 otherwise.
 * Used to enforce the Left Brain input contract documented in
 * banana_ml_regression.h.
 */
static int banana_ml_regression_is_valid_ratio(double v) {
    if (!isfinite(v)) {
        return 0;
    }
    if (v < 0.0 || v > 1.0) {
        return 0;
    }
    return 1;
}

int banana_ml_regression_predict(const BananaMlFeatureVector* features, double* out_score) {
    double linear;
    double score;

    if (!features || !out_score) {
        return BANANA_INVALID_ARGUMENT;
    }

    /* Input contract guard: every ratio slot must be finite and in [0.0, 1.0].
     * NaN/Inf/out-of-range inputs are rejected rather than silently clamped so
     * upstream callers learn about distribution drift (LB-R02). */
    if (!banana_ml_regression_is_valid_ratio(features->banana_signal_ratio) ||
        !banana_ml_regression_is_valid_ratio(features->not_banana_signal_ratio) ||
        !banana_ml_regression_is_valid_ratio(features->unique_token_ratio) ||
        !banana_ml_regression_is_valid_ratio(features->length_ratio) ||
        !banana_ml_regression_is_valid_ratio(features->positive_bigram_ratio) ||
        !banana_ml_regression_is_valid_ratio(features->negative_bigram_ratio) ||
        !banana_ml_regression_is_valid_ratio(features->banana_attention_ratio) ||
        !banana_ml_regression_is_valid_ratio(features->not_banana_attention_ratio)) {
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