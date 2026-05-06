/*
 * banana_ml_regression_simd.c — SIMD-accelerated Left Brain regression scorer.
 *
 * Spec 258 KERN-001: f64x2 WASM SIMD dot-product variant of banana_ml_regression_predict.
 *
 * When compiled with Emscripten and -msimd128, the compiler auto-vectorizes the
 * 8-element double dot product in the inner scoring expression using f64x2 SIMD ops.
 * The exported symbol is banana_ml_regression_predict_simd so the WASM host can
 * dispatch to SIMD or scalar based on capability detection (spec 258 T004).
 *
 * Numeric contract (spec 258 T005):
 *   |simd_score - scalar_score| <= 1e-9 for all valid inputs.
 *
 * This file is NOT included in the native shared library CMake target.
 * It is compiled only by scripts/build-wasm.sh with -msimd128.
 */
#include "banana_ml_regression.h"
#include "banana_ml_regression_simd.h"
#include "banana_status.h"
#include "domain/ml/shared/banana_ml_shared.h"

#include <math.h>

/*
 * SIMD hint: the 8-ratio array layout below allows the compiler to emit
 * f64x2.mul + f64x2.add pairs for the dot product when -msimd128 is active.
 * Manual intrinsics are intentionally avoided — emcc auto-vectorization is
 * sufficient and keeps the code portable.
 */
int banana_ml_regression_predict_simd(const BananaMlFeatureVector *features, double *out_score)
{
    /* Identical input validation to the scalar path (LB-R02). */
    if (!features || !out_score)
        return BANANA_INVALID_ARGUMENT;

    if (!isfinite(features->banana_signal_ratio) || features->banana_signal_ratio < 0.0 || features->banana_signal_ratio > 1.0 ||
        !isfinite(features->not_banana_signal_ratio) || features->not_banana_signal_ratio < 0.0 || features->not_banana_signal_ratio > 1.0 ||
        !isfinite(features->unique_token_ratio) || features->unique_token_ratio < 0.0 || features->unique_token_ratio > 1.0 ||
        !isfinite(features->length_ratio) || features->length_ratio < 0.0 || features->length_ratio > 1.0 ||
        !isfinite(features->positive_bigram_ratio) || features->positive_bigram_ratio < 0.0 || features->positive_bigram_ratio > 1.0 ||
        !isfinite(features->negative_bigram_ratio) || features->negative_bigram_ratio < 0.0 || features->negative_bigram_ratio > 1.0 ||
        !isfinite(features->banana_attention_ratio) || features->banana_attention_ratio < 0.0 || features->banana_attention_ratio > 1.0 ||
        !isfinite(features->not_banana_attention_ratio) || features->not_banana_attention_ratio < 0.0 || features->not_banana_attention_ratio > 1.0)
    {
        return BANANA_INVALID_ARGUMENT;
    }

    /*
     * Array layout exposes contiguous doubles for SIMD auto-vectorization.
     * Pairs: [banana_signal, not_banana_signal], [unique_token, length],
     *        [positive_bigram, negative_bigram], [banana_attn, not_banana_attn]
     */
    const double ratios[8] = {
        features->banana_signal_ratio,
        features->not_banana_signal_ratio,
        features->unique_token_ratio,
        features->length_ratio,
        features->positive_bigram_ratio,
        features->negative_bigram_ratio,
        features->banana_attention_ratio,
        features->not_banana_attention_ratio,
    };
    const double weights[8] = {
         1.40,  /* banana_signal_ratio */
        -1.10,  /* not_banana_signal_ratio */
         0.30,  /* unique_token_ratio */
         0.20,  /* length_ratio */
         0.40,  /* positive_bigram_ratio */
        -0.30,  /* negative_bigram_ratio */
         0.35,  /* banana_attention_ratio */
        -0.25,  /* not_banana_attention_ratio */
    };

    double dot = 0.0;
    /* Unrolled 8-element dot product — compiler emits 4× f64x2 pairs. */
    dot += ratios[0] * weights[0];
    dot += ratios[1] * weights[1];
    dot += ratios[2] * weights[2];
    dot += ratios[3] * weights[3];
    dot += ratios[4] * weights[4];
    dot += ratios[5] * weights[5];
    dot += ratios[6] * weights[6];
    dot += ratios[7] * weights[7];

    double linear = 0.20 + dot;
    double score  = banana_ml_sigmoid_approx((linear * 2.0) - 0.8);
    *out_score    = banana_ml_clamp01(score);
    return BANANA_OK;
}
