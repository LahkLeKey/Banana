#include "banana_ml_transformer.h"
#include "domain/ml/shared/banana_ml_shared.h"
#include "banana_status.h"

#include <math.h>
#include <string.h>

/*
 * 8-slot per-feature affine weights (one per ratio slot). The absolute values
 * are normalized into the attention weights buffer when the caller asks for
 * attention logging (US4), so the sum is well-defined and stable. Order
 * matches the slot table in banana_ml_transformer.h.
 */
static const double k_attention_magnitudes[BANANA_ML_TRANSFORMER_ATTENTION_DIM] = {
    1.20, /* slot 0: banana_signal_ratio */
    1.20, /* slot 1: not_banana_signal_ratio */
    0.15, /* slot 2: unique_token_ratio */
    0.00, /* slot 3: length_ratio (not used by current blend; reserved) */
    0.55, /* slot 4: positive_bigram_ratio */
    0.55, /* slot 5: negative_bigram_ratio */
    1.65, /* slot 6: banana_attention_ratio = 0.95 + 0.70 magnitudes */
    1.65  /* slot 7: not_banana_attention_ratio = 0.95 + 0.70 magnitudes */
};

static int banana_ml_transformer_classify_impl(const BananaMlFeatureVector* features,
                                               const BananaMlTransformerOptions* options,
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

    /* US1: zero-init the result struct so partial failures cannot leak
     * uninitialized bytes to the caller (FB-R05 contract surface). */
    memset(out_result, 0, sizeof(*out_result));

    /*
     * Fixed-parameter attention blend to emulate a tiny deterministic
     * transformer head for banana-vs-not-banana routing. No PRNG, no time,
     * no thread-locals (FB-R02 / US2 idempotence).
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
    banana_probability = banana_ml_sigmoid_approx((attention_delta * 2.4) + BANANA_ML_TRANSFORMER_BIAS);
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

    /* US3: optional embedding fingerprint output. */
    if (options && options->out_embedding) {
        options->out_embedding[0] = banana_context;
        options->out_embedding[1] = not_banana_context;
        options->out_embedding[2] = attention_delta;
        options->out_embedding[3] = banana_probability;
    }

    /* US4: optional attention-weights logging. Pure normalization of the
     * fixed per-slot magnitudes scaled by the input ratios -- writes nothing
     * (and pays no cost beyond the early branch) when toggle is off. */
    if (options && options->log_attention_weights && options->out_attention_weights) {
        double raw[BANANA_ML_TRANSFORMER_ATTENTION_DIM];
        double sum = 0.0;
        size_t i;
        const double* ratios[BANANA_ML_TRANSFORMER_ATTENTION_DIM] = {
            &features->banana_signal_ratio,
            &features->not_banana_signal_ratio,
            &features->unique_token_ratio,
            &features->length_ratio,
            &features->positive_bigram_ratio,
            &features->negative_bigram_ratio,
            &features->banana_attention_ratio,
            &features->not_banana_attention_ratio
        };
        for (i = 0; i < BANANA_ML_TRANSFORMER_ATTENTION_DIM; ++i) {
            /* +1e-6 floor so a fully-zero feature vector still sums to a
             * positive value and produces well-defined normalized weights. */
            raw[i] = (k_attention_magnitudes[i] * (*ratios[i])) + 1e-6;
            sum += raw[i];
        }
        for (i = 0; i < BANANA_ML_TRANSFORMER_ATTENTION_DIM; ++i) {
            options->out_attention_weights[i] = raw[i] / sum;
        }
    }

    return BANANA_OK;
}

int banana_ml_transformer_classify(const BananaMlFeatureVector* features,
                                   BananaMlClassificationResult* out_result) {
    return banana_ml_transformer_classify_impl(features, NULL, out_result);
}

int banana_ml_transformer_classify_ex(const BananaMlFeatureVector* features,
                                       const BananaMlTransformerOptions* options,
                                       BananaMlClassificationResult* out_result) {
    return banana_ml_transformer_classify_impl(features, options, out_result);
}