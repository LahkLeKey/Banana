#ifndef BANANA_ML_BINARY_SIMD_H
#define BANANA_ML_BINARY_SIMD_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * SIMD-accelerated variant of banana_ml_binary_classify (spec 258 KERN-002).
     *
     * Compiled with Emscripten -msimd128. The compiler auto-vectorizes parallel
     * banana/not-banana score accumulation using f64x2.mul + f64x2.add.
     *
     * Contract: identical to banana_ml_binary_classify — default bias (0.10, 0.10),
     * default threshold (BANANA_ML_BINARY_DEFAULT_THRESHOLD). Same status codes,
     * same BananaMlClassificationResult shape.
     * Numeric error vs scalar: |simd_banana_score - scalar_banana_score| <= 1e-9.
     * Label identity: SIMD and scalar produce identical labels for all valid inputs.
     *
     * NOT exported in the native shared library. Exported in the WASM artifact only.
     */
    int banana_ml_binary_classify_simd(const BananaMlFeatureVector *features,
                                       BananaMlClassificationResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
