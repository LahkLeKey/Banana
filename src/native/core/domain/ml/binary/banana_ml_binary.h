#ifndef BANANA_ML_BINARY_H
#define BANANA_ML_BINARY_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Right Brain (Binary) banana / not-banana classifier.
     *
     * Domain: Right Brain (binary decision with calibrated confidence).
     * Source SPIKE: 009-ml-brain-domain-spike.
     * Implementation slice: 011-right-brain-binary-runnable.
     *
     * Operating point (FROZEN for this slice):
     *   - Default threshold: BANANA_ML_BINARY_DEFAULT_THRESHOLD = 0.50
     *   - Precision target  : >= 0.85 on the calibration anchor set (US1)
     *   - Recall target     : >= 0.80 on the calibration anchor set (US1)
     *   - The threshold is exposed via banana_ml_binary_classify_with_threshold so
     *     deployments can shift toward higher precision (raise threshold) or
     *     higher recall (lower threshold) without recompiling consumers (US3).
     *
     * Affine calibration constants:
     *   - The raw sigmoid output is post-processed as
     *       calibrated = clamp01(BANANA_ML_BINARY_CALIBRATION_A * raw
     *                            + BANANA_ML_BINARY_CALIBRATION_B)
     *     and then compared against the threshold to produce the label.
     *   - The current values (A=1.0, B=0.0) implement IDENTITY calibration. They
     *     are exposed as named constants so a future calibration sweep can adjust
     *     them in one place without rewriting the test anchors.
     *
     * Result contract (BananaMlClassificationResult):
     *   - label              -> "banana" or "not_banana"
     *   - banana_score       -> calibrated probability of banana, in [0.0, 1.0]
     *   - not_banana_score   -> 1.0 - banana_score
     *   - confidence         -> banana_score if label == "banana", else not_banana_score
     *   - jaccard            -> Jaccard index of predicted vs. observed (in [0,1])
     *   - confusion          -> single-sample confusion matrix (tp/fp/fn/tn each in {0,1})
     *
     * Status codes:
     *   BANANA_OK                 - success; *out_result is populated.
     *   BANANA_INVALID_ARGUMENT   - features or out_result is NULL.
     */

#define BANANA_ML_BINARY_DEFAULT_THRESHOLD 0.50
#define BANANA_ML_BINARY_CALIBRATION_A 1.00
#define BANANA_ML_BINARY_CALIBRATION_B 0.00

    int banana_ml_binary_classify(const BananaMlFeatureVector *features,
                                  BananaMlClassificationResult *out_result);
    int banana_ml_binary_classify_with_not_banana_bias(const BananaMlFeatureVector *features,
                                                       BananaMlClassificationResult *out_result);

    /*
     * Same operating point as banana_ml_binary_classify but with an operator-tunable
     * threshold (US3). `threshold` MUST be in [0.0, 1.0]; out-of-range values
     * return BANANA_INVALID_ARGUMENT.
     */
    int banana_ml_binary_classify_with_threshold(const BananaMlFeatureVector *features,
                                                 double threshold,
                                                 BananaMlClassificationResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
