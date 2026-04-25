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
 * require a major bump and a parent-spec (005) update. */
#define BANANA_WRAPPER_ABI_VERSION_MAJOR 2
#define BANANA_WRAPPER_ABI_VERSION_MINOR 0

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
BANANA_API int banana_classify_banana_transformer(const char* input_json, char** out_json);
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
