#ifndef BANANA_WRAPPER_ML_REGRESSION_H
#define BANANA_WRAPPER_ML_REGRESSION_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * banana_wrapper_ml_predict_regression_score — wrapper-ABI entry for Left Brain
     * regression scoring driven from a JSON payload (string -> scalar score).
     *
     * Domain: Left Brain (Regression) — see banana_ml_regression.h for the core
     * input/output contract. This wrapper builds the feature vector from input_json
     * and forwards to banana_ml_regression_predict.
     *
     * Output (*out_score) is in [0.0, 1.0] and is ORDINAL ONLY:
     *   - Higher value => more banana-like signal.
     *   - Lower value  => more not-banana / off-domain signal.
     *
     * Threshold guidance for downstream consumers (010 contract):
     *   - Do NOT invent a threshold per consumer; pin it against the calibration
     *     anchors documented in tests/native/test_ml_regression.c.
     *   - Reference anchors (deterministic for the current model coefficients):
     *       clearly-banana payload      -> score > 0.65 (observed ~0.90)
     *       ambiguous payload           -> 0.30 < score < 0.70 (observed ~0.59)
     *       clearly-not-banana payload  -> score < 0.35 (observed ~0.19)
     *   - A neutral default threshold of 0.50 is acceptable when the consumer needs
     *     a binary cut; for higher-precision use cases shift toward the
     *     clearly-banana anchor (e.g., 0.65) instead.
     *
     * Status codes are inherited from banana_ml_predict_regression_score and
     * documented in banana_wrapper.h.
     */
    int banana_wrapper_ml_predict_regression_score(const char *input_json, double *out_score);

#ifdef __cplusplus
}
#endif

#endif
