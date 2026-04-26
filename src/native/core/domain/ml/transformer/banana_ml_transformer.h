#ifndef BANANA_ML_TRANSFORMER_H
#define BANANA_ML_TRANSFORMER_H

#include <stddef.h>

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Full Brain (tiny deterministic transformer head) banana / not-banana router.
 *
 * Domain: Full Brain (cross-feature attention blend over the BananaMlFeatureVector).
 * Source SPIKE: 009-ml-brain-domain-spike.
 * Implementation slice: 012-full-brain-transformer-runnable.
 *
 * --- Context token schema (FROZEN, FB-R05) ---
 *
 * The transformer interprets the BananaMlFeatureVector as 8 ratio-valued
 * "context tokens" plus 6 informational count tokens. The ordered token slots are:
 *
 *   slot  | source field                          | role
 *   ------|---------------------------------------|----------------------------------
 *    0    | banana_signal_ratio                    | Left-Brain banana score proxy
 *    1    | not_banana_signal_ratio                | Left-Brain not-banana score proxy
 *    2    | unique_token_ratio                     | lexical diversity
 *    3    | length_ratio                           | normalized length
 *    4    | positive_bigram_ratio                  | banana-bigram density
 *    5    | negative_bigram_ratio                  | not-banana-bigram density
 *    6    | banana_attention_ratio                 | Right-Brain banana attention
 *    7    | not_banana_attention_ratio             | Right-Brain not-banana attention
 *
 * Each ratio MUST be a finite double in [0.0, 1.0]. Out-of-range values are
 * NOT clamped here -- callers should validate at the wrapper layer (the JSON
 * builder always produces in-range values).
 *
 * --- Embedding output shape ---
 *
 * The transformer exposes a small fingerprint embedding of dimension
 * BANANA_ML_TRANSFORMER_EMBEDDING_DIM = 4 with the components:
 *
 *   index | meaning
 *   ------|----------------------------------
 *     0   | banana_context (signed)
 *     1   | not_banana_context (signed)
 *     2   | attention_delta = banana_context - not_banana_context (signed)
 *     3   | calibrated banana_probability (in [0.0, 1.0])
 *
 * Expected upper bound on L2 norm under the locked weights:
 *   BANANA_ML_TRANSFORMER_EMBEDDING_MAX_L2 = 8.0
 * (loose; the worst-case ratio of 1.0 across all slots produces a vector well
 * under this bound; the bound is here so future changes can fail loudly).
 *
 * --- Attention output shape (optional, US4) ---
 *
 * When `options.log_attention_weights` is non-zero AND
 * `options.out_attention_weights` is non-NULL, the function writes
 * BANANA_ML_TRANSFORMER_ATTENTION_DIM = 8 weights -- one per ratio slot --
 * which sum to ~1.0 (within 1e-9). When the toggle is OFF the buffer is
 * untouched and there is bit-for-bit equivalence with the legacy entrypoint
 * (FB-R03: pay no cost when not asked).
 *
 * --- Determinism (FB-R02, US2) ---
 *
 * No PRNG, no time, no thread-locals. Repeated calls with the same input
 * MUST return byte-identical BananaMlClassificationResult.label string,
 * banana_score, not_banana_score, confidence, jaccard, and confusion_matrix
 * fields. The seed-constant analog is BANANA_ML_TRANSFORMER_BIAS = 0.05.
 *
 * --- Status codes ---
 *   BANANA_OK                 - success.
 *   BANANA_INVALID_ARGUMENT   - features or out_result is NULL.
 */

#define BANANA_ML_TRANSFORMER_EMBEDDING_DIM     4u
#define BANANA_ML_TRANSFORMER_EMBEDDING_MAX_L2  8.0
#define BANANA_ML_TRANSFORMER_ATTENTION_DIM     8u
#define BANANA_ML_TRANSFORMER_BIAS              0.05

/*
 * Optional diagnostics requested by the caller. Both buffers, if non-NULL
 * and the corresponding flag is set, are filled by the callee.
 */
typedef struct BananaMlTransformerOptions {
    int log_attention_weights;             /* 0 = disabled (default), non-zero = enabled */
    double* out_embedding;                 /* nullable; when non-NULL must point to BANANA_ML_TRANSFORMER_EMBEDDING_DIM doubles */
    double* out_attention_weights;         /* nullable; when non-NULL AND log flag set, BANANA_ML_TRANSFORMER_ATTENTION_DIM doubles */
} BananaMlTransformerOptions;

int banana_ml_transformer_classify(const BananaMlFeatureVector* features,
                                   BananaMlClassificationResult* out_result);

/*
 * Same operating point as banana_ml_transformer_classify but exposes the
 * embedding fingerprint and the optional attention-weights buffer for
 * downstream diagnostics and tests (US3, US4). When `options` is NULL the
 * behavior is identical to banana_ml_transformer_classify.
 */
int banana_ml_transformer_classify_ex(const BananaMlFeatureVector* features,
                                       const BananaMlTransformerOptions* options,
                                       BananaMlClassificationResult* out_result);

#ifdef __cplusplus
}
#endif

#endif