#include "banana_ml_transformer.h"
#include "domain/ml/shared/banana_ml_shared.h"
#include "banana_status.h"

int banana_ml_transformer_classify(const BananaMlFeatureVector* features,
                                   BananaMlClassificationResult* out_result) {
    double banana_context;
    double not_banana_context;
    double attention_delta;
    double banana_probability;
    double not_banana_probability;
    int predicted_is_banana;
    int actual_is_banana;

    if (!features || !out_result) {
        return BANANA_INVALID_ARGUMENT;
    }

    /*
     * Fixed-parameter attention blend to emulate a tiny deterministic
     * transformer head for banana-vs-not-banana routing.
     */
    banana_context =
        (1.20 * features->banana_signal_ratio) +
        (0.95 * features->banana_attention_ratio) +
        (0.55 * features->positive_bigram_ratio) +
        (0.15 * features->unique_token_ratio) -
        (0.70 * features->not_banana_attention_ratio) -
        (0.55 * features->not_banana_signal_ratio);

    not_banana_context =
        (1.20 * features->not_banana_signal_ratio) +
        (0.95 * features->not_banana_attention_ratio) +
        (0.55 * features->negative_bigram_ratio) +
        (0.15 * (1.0 - features->unique_token_ratio)) -
        (0.70 * features->banana_attention_ratio) -
        (0.55 * features->banana_signal_ratio);

    attention_delta = banana_context - not_banana_context;
    banana_probability = banana_ml_sigmoid_approx((attention_delta * 2.4) + 0.05);
    banana_probability = banana_ml_clamp01(banana_probability);
    not_banana_probability = 1.0 - banana_probability;

    predicted_is_banana = banana_probability >= 0.5;
    actual_is_banana =
        (features->banana_attention_ratio + (0.20 * features->banana_signal_ratio)) >=
        (features->not_banana_attention_ratio + (0.20 * features->not_banana_signal_ratio));

    banana_ml_fill_confusion(predicted_is_banana, actual_is_banana, &out_result->confusion);

    out_result->label = predicted_is_banana ? "banana" : "not_banana";
    out_result->banana_score = banana_probability;
    out_result->not_banana_score = not_banana_probability;
    out_result->confidence = predicted_is_banana ? banana_probability : not_banana_probability;
    out_result->jaccard = banana_ml_jaccard_for_banana(&out_result->confusion);

    return BANANA_OK;
}