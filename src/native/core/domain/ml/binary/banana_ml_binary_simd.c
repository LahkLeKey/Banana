/*
 * banana_ml_binary_simd.c — SIMD-accelerated Right Brain binary classifier.
 *
 * Spec 258 KERN-002: f64x2 WASM SIMD parallel score accumulation variant of
 * banana_ml_binary_classify_impl.
 *
 * When compiled with Emscripten and -msimd128, the compiler auto-vectorizes the
 * banana_linear / not_banana_linear accumulation using f64x2 ops (two scores
 * computed in parallel across opposing weight arrays).
 *
 * Exported symbol: banana_ml_binary_classify_simd (maps to banana_ml_classify_binary
 * entry point for WASM dispatch; see scripts/build-wasm.sh).
 *
 * Numeric contract (spec 258 T005):
 *   |simd_banana_score - scalar_banana_score| <= 1e-9 for all valid inputs.
 *   Label identity: SIMD and scalar must produce the same label for all valid inputs.
 *
 * NOT included in the native shared library CMake target.
 * Compiled only by scripts/build-wasm.sh with -msimd128.
 */
#include "banana_ml_binary.h"
#include "banana_ml_binary_simd.h"
#include "banana_status.h"
#include "domain/ml/shared/banana_ml_shared.h"

#include <math.h>

int banana_ml_binary_classify_simd(const BananaMlFeatureVector *features,
                                   BananaMlClassificationResult *out_result)
{
    if (!features || !out_result)
        return BANANA_INVALID_ARGUMENT;

    const double threshold = BANANA_ML_BINARY_DEFAULT_THRESHOLD;
    if (!isfinite(threshold) || threshold < 0.0 || threshold > 1.0)
        return BANANA_INVALID_ARGUMENT;

    /*
     * Interleaved feature layout: banana score terms in even indices,
     * not_banana score terms in odd indices. This layout allows the compiler
     * to emit f64x2.mul pairs that compute both scores simultaneously.
     *
     * banana_linear    = bias_b + w[0]*f[0] + w[2]*f[2] + w[4]*f[4] + w[6]*f[6]
     * not_banana_linear= bias_nb+ w[1]*f[1] + w[3]*f[3] + w[5]*f[5] + w[7]*f[7]
     */
    const double feats_b[8] = {
        features->banana_signal_ratio,         /* +1.80 */
        features->not_banana_signal_ratio,     /* -1.20 */
        features->positive_bigram_ratio,       /* +0.45 */
        features->negative_bigram_ratio,       /* -0.20 */
        features->banana_attention_ratio,      /* +0.35 */
        features->not_banana_attention_ratio,  /* -0.20 */
        features->unique_token_ratio,          /* +0.10 */
        0.0,
    };
    const double weights_b[8] = {
         1.80, -1.20,
         0.45, -0.20,
         0.35, -0.20,
         0.10,  0.0,
    };

    const double feats_nb[8] = {
        features->not_banana_signal_ratio,         /* +1.80 */
        features->banana_signal_ratio,             /* -1.20 */
        features->negative_bigram_ratio,           /* +0.45 */
        features->positive_bigram_ratio,           /* -0.20 */
        features->not_banana_attention_ratio,      /* +0.35 */
        features->banana_attention_ratio,          /* -0.20 */
        1.0 - features->unique_token_ratio,        /* +0.12 */
        0.0,
    };
    const double weights_nb[8] = {
         1.80, -1.20,
         0.45, -0.20,
         0.35, -0.20,
         0.12,  0.0,
    };

    /* Unrolled dot products — compiler emits 4× f64x2 pairs per score. */
    double banana_dot = 0.0;
    double not_banana_dot = 0.0;
    banana_dot    += feats_b[0]  * weights_b[0];
    not_banana_dot+= feats_nb[0] * weights_nb[0];
    banana_dot    += feats_b[1]  * weights_b[1];
    not_banana_dot+= feats_nb[1] * weights_nb[1];
    banana_dot    += feats_b[2]  * weights_b[2];
    not_banana_dot+= feats_nb[2] * weights_nb[2];
    banana_dot    += feats_b[3]  * weights_b[3];
    not_banana_dot+= feats_nb[3] * weights_nb[3];
    banana_dot    += feats_b[4]  * weights_b[4];
    not_banana_dot+= feats_nb[4] * weights_nb[4];
    banana_dot    += feats_b[5]  * weights_b[5];
    not_banana_dot+= feats_nb[5] * weights_nb[5];
    banana_dot    += feats_b[6]  * weights_b[6];
    not_banana_dot+= feats_nb[6] * weights_nb[6];

    double banana_linear     = 0.10 + banana_dot;
    double not_banana_linear = 0.10 + not_banana_dot;

    double raw = banana_ml_sigmoid_approx((banana_linear - not_banana_linear) * 2.0);
    raw        = banana_ml_clamp01(raw);

    double banana_prob     = banana_ml_clamp01((BANANA_ML_BINARY_CALIBRATION_A * raw) + BANANA_ML_BINARY_CALIBRATION_B);
    double not_banana_prob = 1.0 - banana_prob;

    int predicted_is_banana = banana_prob >= threshold;
    int actual_is_banana =
        (features->banana_signal_ratio + (0.25 * features->positive_bigram_ratio)) >=
        (features->not_banana_signal_ratio + (0.25 * features->negative_bigram_ratio));

    banana_ml_fill_confusion(predicted_is_banana, actual_is_banana, &out_result->confusion);

    out_result->label          = predicted_is_banana ? "banana" : "not_banana";
    out_result->banana_score   = banana_prob;
    out_result->not_banana_score = not_banana_prob;
    out_result->confidence     = predicted_is_banana ? banana_prob : not_banana_prob;
    out_result->jaccard        = banana_ml_jaccard_for_banana(&out_result->confusion);

    return BANANA_OK;
}
