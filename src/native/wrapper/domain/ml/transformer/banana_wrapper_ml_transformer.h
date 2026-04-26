#ifndef BANANA_WRAPPER_ML_TRANSFORMER_H
#define BANANA_WRAPPER_ML_TRANSFORMER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Wrapper-ABI entry for the Full Brain transformer classifier. Accepts a
 * UTF-8 JSON payload `{"text":"..."}` and returns a freshly allocated
 * UTF-8 JSON string via `*out_json`. Caller MUST free with `banana_free()`.
 *
 * JSON contract (FIELD-LOCKED for downstream interop, FB-R05):
 *   {
 *     "model": "transformer",                // string, always "transformer"
 *     "label": "banana" | "not_banana",    // string, classifier verdict
 *     "confidence": 0.0..1.0,                // number, picked-side probability
 *     "banana_score": 0.0..1.0,              // number, P(banana)
 *     "not_banana_score": 0.0..1.0,          // number, 1 - banana_score
 *     "jaccard": 0.0..1.0,                   // number, Jaccard for banana class
 *     "confusion_matrix": {
 *       "tp": 0|1, "fp": 0|1, "fn": 0|1, "tn": 0|1
 *     }
 *   }
 *
 * The field order above is the ordering produced by the wrapper and is part
 * of the contract for snapshot tests in downstream consumers.
 *
 * For attention-weights or embedding diagnostics, drop down to the core
 * entrypoint banana_ml_transformer_classify_ex (see banana_ml_transformer.h).
 * The wrapper layer is intentionally diagnostics-free so the hot JSON path
 * stays cheap (FB-R03).
 */
int banana_wrapper_ml_classify_transformer(const char* input_json, char** out_json);

/*
 * Diagnostics variant exposing the embedding fingerprint and the optional
 * attention weights buffer (slice 012). Mirrors the public ABI entry
 * banana_classify_banana_transformer_ex declared in banana_wrapper.h.
 */
int banana_wrapper_ml_classify_transformer_ex(const char* input_json,
                                                int log_attention,
                                                double* out_embedding,
                                                double* out_attention_weights,
                                                char** out_json);

/*
 * Slice 016 -- additive int8-quantized embedding variant of the
 * transformer wrapper. Mirrors the public ABI export
 * `banana_classify_banana_transformer_quant_embedding` declared in
 * `banana_wrapper.h`.
 */
int banana_wrapper_ml_classify_transformer_quant_embedding(const char* input_json,
                                                             signed char* out_quant,
                                                             double* out_scale,
                                                             signed char* out_zero,
                                                             char** out_json);

#ifdef __cplusplus
}
#endif

#endif