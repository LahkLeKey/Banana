#ifndef BANANA_ML_REGRESSION_H
#define BANANA_ML_REGRESSION_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * banana_ml_regression_predict — Left Brain (Regression) ripeness/banana scorer.
     *
     * Domain: Left Brain (continuous estimation; ordinal magnitude only).
     * Source SPIKE: 009-ml-brain-domain-spike.
     * Implementation slice: 010-left-brain-regression-runnable.
     *
     * Input contract (BananaMlFeatureVector — see domain/banana_ml_models.h):
     *   All `*_ratio` fields MUST be finite (non-NaN, non-Inf) doubles in [0.0, 1.0].
     *   Counts (token_count, *_hits) are size_t and informational only for this scorer.
     *   Out-of-range or non-finite ratios cause BANANA_INVALID_ARGUMENT rejection.
     *
     * Output contract:
     *   On BANANA_OK, *out_score is in the closed range [0.0, 1.0] (clamped).
     *   The score is ordinal-only: higher score => more banana-like signal.
     *   It is NOT a calibrated probability; consumers must pin their own threshold
     *   against the calibration anchors documented in tests/native/test_ml_regression.c.
     *
     * Status codes:
     *   BANANA_OK                 - success; *out_score is populated.
     *   BANANA_INVALID_ARGUMENT   - features or out_score is NULL, OR any *_ratio
     *                               input is non-finite or outside [0.0, 1.0].
     */
    int banana_ml_regression_predict(const BananaMlFeatureVector *features, double *out_score);

#ifdef __cplusplus
}
#endif

#endif
