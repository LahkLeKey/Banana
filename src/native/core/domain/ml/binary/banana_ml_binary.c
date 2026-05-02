#include "banana_ml_binary.h"
#include "banana_status.h"
#include "domain/ml/shared/banana_ml_shared.h"

#include <math.h>

static int banana_ml_binary_classify_impl(const BananaMlFeatureVector *features, double banana_bias,
                                          double not_banana_bias, double threshold,
                                          BananaMlClassificationResult *out_result)
{
    double banana_linear;
    double not_banana_linear;
    double raw_banana_probability;
    double banana_probability;
    double not_banana_probability;
    int predicted_is_banana;
    int actual_is_banana;

    if (!features || !out_result)
    {
        return BANANA_INVALID_ARGUMENT;
    }
    /* US3: per-deployment threshold must be a finite value in [0.0, 1.0]. */
    if (!isfinite(threshold) || threshold < 0.0 || threshold > 1.0)
    {
        return BANANA_INVALID_ARGUMENT;
    }

    banana_linear =
        banana_bias + (1.80 * features->banana_signal_ratio) -
        (1.20 * features->not_banana_signal_ratio) + (0.45 * features->positive_bigram_ratio) -
        (0.20 * features->negative_bigram_ratio) + (0.35 * features->banana_attention_ratio) -
        (0.20 * features->not_banana_attention_ratio) + (0.10 * features->unique_token_ratio);

    not_banana_linear =
        not_banana_bias + (1.80 * features->not_banana_signal_ratio) -
        (1.20 * features->banana_signal_ratio) + (0.45 * features->negative_bigram_ratio) -
        (0.20 * features->positive_bigram_ratio) + (0.35 * features->not_banana_attention_ratio) -
        (0.20 * features->banana_attention_ratio) + (0.12 * (1.0 - features->unique_token_ratio));

    raw_banana_probability = banana_ml_sigmoid_approx((banana_linear - not_banana_linear) * 2.0);
    raw_banana_probability = banana_ml_clamp01(raw_banana_probability);

    /* US1: affine calibration of the raw sigmoid output before threshold compare.
     * Constants live in banana_ml_binary.h so they can be re-tuned in one place. */
    banana_probability = banana_ml_clamp01(
        (BANANA_ML_BINARY_CALIBRATION_A * raw_banana_probability) + BANANA_ML_BINARY_CALIBRATION_B);
    not_banana_probability = 1.0 - banana_probability;

    predicted_is_banana = banana_probability >= threshold;
    actual_is_banana =
        (features->banana_signal_ratio + (0.25 * features->positive_bigram_ratio)) >=
        (features->not_banana_signal_ratio + (0.25 * features->negative_bigram_ratio));

    banana_ml_fill_confusion(predicted_is_banana, actual_is_banana, &out_result->confusion);

    out_result->label = predicted_is_banana ? "banana" : "not_banana";
    out_result->banana_score = banana_probability;
    out_result->not_banana_score = not_banana_probability;
    out_result->confidence = predicted_is_banana ? banana_probability : not_banana_probability;
    out_result->jaccard = banana_ml_jaccard_for_banana(&out_result->confusion);

    return BANANA_OK;
}

int banana_ml_binary_classify(const BananaMlFeatureVector *features,
                              BananaMlClassificationResult *out_result)
{
    return banana_ml_binary_classify_impl(features, 0.10, 0.10, BANANA_ML_BINARY_DEFAULT_THRESHOLD,
                                          out_result);
}

int banana_ml_binary_classify_with_not_banana_bias(const BananaMlFeatureVector *features,
                                                   BananaMlClassificationResult *out_result)
{
    return banana_ml_binary_classify_impl(features, 0.00, 0.18, BANANA_ML_BINARY_DEFAULT_THRESHOLD,
                                          out_result);
}

int banana_ml_binary_classify_with_threshold(const BananaMlFeatureVector *features,
                                             double threshold,
                                             BananaMlClassificationResult *out_result)
{
    return banana_ml_binary_classify_impl(features, 0.10, 0.10, threshold, out_result);
}
