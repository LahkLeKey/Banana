#ifndef BANANA_ML_REGRESSION_SIMD_H
#define BANANA_ML_REGRESSION_SIMD_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * SIMD-accelerated variant of banana_ml_regression_predict (spec 258 KERN-001).
     *
     * Compiled with Emscripten -msimd128. The compiler auto-vectorizes the 8-element
     * dot product using f64x2.mul + f64x2.add WASM SIMD instructions.
     *
     * Contract: identical to banana_ml_regression_predict — same inputs, same status
     * codes, same output range [0.0, 1.0]. Numeric error vs scalar <= 1e-9 (spec 258 T005).
     *
     * NOT exported in the native shared library. Exported in the WASM artifact only.
     */
    int banana_ml_regression_predict_simd(const BananaMlFeatureVector *features, double *out_score);

#ifdef __cplusplus
}
#endif

#endif
