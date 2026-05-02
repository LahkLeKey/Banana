/*
 * banana_wrapper.h — Public ABI for the Banana native shared library.
 *
 * Spec: .specify/specs/006-native-core/contracts/native-wrapper-abi.md
 * Stability:
 *   - Symbol names listed below are stable.
 *   - NativeStatusCode numeric values are stable (consumed by managed P/Invoke).
 *   - Calling convention: platform default for shared libs (__cdecl on Windows).
 *
 * Memory ownership conventions:
 *   - Out-pointer status fields (`int*`, `double*`, struct out-fields) are
 *     CALLER-OWNED stack/heap; the callee writes through.
 *   - String/buffer outputs returned via `char**` are CALLEE-ALLOCATED;
 *     the caller MUST release them via `banana_free()`.
 *   - All UTF-8 string inputs are caller-owned and not retained past the call.
 *
 * Error reporting: every fallible export returns NativeStatusCode. No errno,
 * no thread-local last-error, no string-channel side errors.
 */

#ifndef BANANA_WRAPPER_H
#define BANANA_WRAPPER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#  if defined(BANANA_BUILDING_DLL)
#    define BANANA_API __declspec(dllexport)
#  else
#    define BANANA_API __declspec(dllimport)
#  endif
#else
#  define BANANA_API __attribute__((visibility("default")))
#endif

/* Monotonic ABI version. Adding symbols increments minor; breaking changes
 * require a major bump and a parent-spec (005) update.
 * Feature 072: banana_native_version() exposes this at runtime so consumers
 * (C# interop, CI abi-diff script) can assert compatibility. */
#define BANANA_WRAPPER_ABI_VERSION_MAJOR 2
/* Minor 1: additive `banana_classify_banana_binary_with_threshold` (slice 011).
 * Minor 2: additive `banana_classify_banana_transformer_ex` (slice 012).
 * Minor 3: additive `banana_classify_banana_transformer_quant_embedding` (slice 016).
 * Minor 4: additive `banana_native_version` (feature 072). */
#define BANANA_WRAPPER_ABI_VERSION_MINOR 4

/* ABI version query — always succeeds.
 * out_major and out_minor are CALLER-OWNED and must be non-NULL.
 * Returns BANANA_OK (0). */
BANANA_API int banana_native_version(int* out_major, int* out_minor);

/* NativeStatusCode — stable numeric values; mirrored by managed enum.
 * DO NOT renumber. v1 used 0..6; v2 preserves them (FR-001).
 */
typedef enum BananaStatusCode {
    BANANA_OK                  = 0,
    BANANA_INVALID_ARGUMENT    = 1,
    BANANA_OVERFLOW            = 2,
    BANANA_INTERNAL_ERROR      = 3,
    BANANA_DB_ERROR            = 4,
    BANANA_DB_NOT_CONFIGURED   = 5,  /* loud DAL gate; do not silently skip */
    BANANA_NOT_FOUND           = 6,
    /* v2 additions begin at 7 */
    BANANA_BUFFER_TOO_SMALL    = 7
} BananaStatusCode;

/* === Calculation domain === */
BANANA_API int banana_calculate_banana(int purchases, int multiplier, int* out_banana);
BANANA_API int banana_calculate_banana_with_breakdown(int purchases, int multiplier,
                                                       int* out_banana,
                                                       int* out_base,
                                                       int* out_bonus);
BANANA_API int banana_create_banana_message(int purchases, int multiplier, char** out_message);

/* === DAL / profile (Postgres-gated; loud BANANA_DB_NOT_CONFIGURED) === */
BANANA_API int banana_db_query_banana_profile(const char* profile_id, char** out_json);

/* === Classifier / ML === */
BANANA_API int banana_predict_banana_regression_score(const char* input_json, double* out_score);
BANANA_API int banana_classify_banana_binary(const char* input_json, char** out_json);
BANANA_API int banana_classify_banana_binary_with_threshold(const char* input_json,
                                                             double threshold,
                                                             char** out_json);
BANANA_API int banana_classify_banana_transformer(const char* input_json, char** out_json);

/*
 * Diagnostics variant of banana_classify_banana_transformer (slice 012, FB-R03/FB-R04).
 *
 * `out_embedding` (nullable) MUST point to 4 doubles (BANANA_ML_TRANSFORMER_EMBEDDING_DIM)
 * when non-NULL. The function writes the (banana_context, not_banana_context,
 * attention_delta, banana_probability) fingerprint vector.
 *
 * `out_attention_weights` (nullable) MUST point to 8 doubles
 * (BANANA_ML_TRANSFORMER_ATTENTION_DIM) when non-NULL AND `log_attention != 0`.
 * The function writes per-feature normalized attention weights summing to ~1.0.
 * When `log_attention == 0` the buffer is left untouched and the hot path is
 * bit-for-bit equivalent to banana_classify_banana_transformer.
 *
 * `out_json` MUST be non-NULL; same JSON shape as the legacy entry. Caller
 * frees the JSON string via banana_free().
 */
BANANA_API int banana_classify_banana_transformer_ex(const char* input_json,
                                                      int log_attention,
                                                      double* out_embedding,
                                                      double* out_attention_weights,
                                                      char** out_json);

/*
 * Slice 016 -- additive int8-quantized embedding variant of the Full Brain
 * transformer (Q-R01..Q-R04). Emits the same JSON shape as the legacy
 * entries via `out_json` (caller frees with `banana_free()`), AND fills
 * the int8 quantized embedding plus its scale and zero-point so a
 * downstream cross-process consumer can reconstruct the original double
 * embedding within bounded error.
 *
 * `out_quant`  MUST point to BANANA_ML_TRANSFORMER_EMBEDDING_DIM int8s.
 * `out_scale`  MUST be non-NULL; set to a positive finite double.
 * `out_zero`   MUST be non-NULL; set to the int8 zero-point such that
 *              reconstruct[i] = (out_quant[i] - out_zero) * out_scale.
 * `out_json`   MUST be non-NULL; same shape as the legacy ABI.
 *
 * The quantization helper is opt-in via this export only; the existing
 * `_ex` ABI is unchanged (Q-R01).
 */
BANANA_API int banana_classify_banana_transformer_quant_embedding(const char* input_json,
                                                                    signed char* out_quant,
                                                                    double* out_scale,
                                                                    signed char* out_zero,
                                                                    char** out_json);
BANANA_API int banana_classify_not_banana_junk(const char* input_json, char** out_json);
BANANA_API int banana_predict_banana_ripeness(const char* input_json, char** out_json);

/* === Batch domain (UTF-8 strings) === */
BANANA_API int banana_create_batch(const char* input_json, char** out_json);
BANANA_API int banana_get_batch_status(const char* batch_id, char** out_json);
BANANA_API int banana_predict_batch_ripeness(const char* batch_id, char** out_json);

/* === Harvest batch domain === */
BANANA_API int banana_create_harvest_batch(const char* input_json, char** out_json);
BANANA_API int banana_add_bunch_to_harvest_batch(const char* batch_id, const char* input_json, char** out_json);
BANANA_API int banana_get_harvest_batch_status(const char* batch_id, char** out_json);

/* === Truck / logistics domain === */
BANANA_API int banana_register_truck(const char* input_json, char** out_json);
BANANA_API int banana_load_truck_container(const char* truck_id, const char* input_json, char** out_json);
BANANA_API int banana_unload_truck_container(const char* truck_id, const char* container_id, char** out_json);
BANANA_API int banana_relocate_truck(const char* truck_id, const char* input_json, char** out_json);
BANANA_API int banana_get_truck_status(const char* truck_id, char** out_json);

/* === Memory === */
BANANA_API void banana_free(void* pointer);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BANANA_WRAPPER_H */
