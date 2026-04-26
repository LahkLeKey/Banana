#ifndef BANANA_WRAPPER_ML_BINARY_H
#define BANANA_WRAPPER_ML_BINARY_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Wrapper-ABI entries for the Right Brain binary classifier. All entries
 * accept a UTF-8 JSON payload `{"text":"..."}` and return a freshly
 * allocated UTF-8 JSON string via `*out_json`. Caller MUST free the result
 * with `banana_free()`.
 *
 * JSON contract (FIELD-LOCKED for downstream interop):
 *   {
 *     "model": "binary",                    // string, always "binary"
 *     "label": "banana" | "not_banana",   // string, classifier verdict
 *     "confidence": 0.0..1.0,               // number, picked-side probability
 *     "banana_score": 0.0..1.0,             // number, P(banana)
 *     "not_banana_score": 0.0..1.0,         // number, 1 - banana_score
 *     "jaccard": 0.0..1.0,                  // number, Jaccard for banana class
 *     "confusion_matrix": {
 *       "tp": 0|1, "fp": 0|1, "fn": 0|1, "tn": 0|1
 *     }
 *   }
 *
 * Field ordering above is the ordering produced by the wrapper and is part
 * of the contract for snapshot tests in downstream consumers.
 *
 * Variants:
 *   - banana_wrapper_ml_classify_binary             : default operating point.
 *   - banana_wrapper_ml_classify_not_banana_junk    : strict not-banana bias
 *                                                     for false-positive
 *                                                     sensitive consumers.
 *   - banana_wrapper_ml_classify_binary_with_threshold : per-deployment
 *                                                        threshold tuning
 *                                                        (US3). Threshold MUST
 *                                                        be in [0.0, 1.0].
 */
int banana_wrapper_ml_classify_binary(const char* input_json, char** out_json);
int banana_wrapper_ml_classify_not_banana_junk(const char* input_json, char** out_json);
int banana_wrapper_ml_classify_binary_with_threshold(const char* input_json,
                                                      double threshold,
                                                      char** out_json);

#ifdef __cplusplus
}
#endif

#endif