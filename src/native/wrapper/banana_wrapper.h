#ifndef BANANA_WRAPPER_H
#define BANANA_WRAPPER_H

#include <stddef.h>

#ifdef _WIN32
#define BANANA_API __declspec(dllexport)
#else
#define BANANA_API __attribute__((visibility("default")))
#endif

typedef enum CInteropStatus {
    BANANA_STATUS_OK = 0,
    BANANA_STATUS_INVALID_ARGUMENT = 1,
    BANANA_STATUS_OVERFLOW = 2,
    BANANA_STATUS_INTERNAL_ERROR = 3,
    BANANA_STATUS_DB_ERROR = 4,
    BANANA_STATUS_DB_NOT_CONFIGURED = 5,
    BANANA_STATUS_NOT_FOUND = 6
} CInteropStatus;

typedef struct CInteropBananaBreakdown {
    int purchases;
    int multiplier;
    int banana;
} CInteropBananaBreakdown;

typedef struct CInteropBananaRipenessPrediction {
    int predicted_stage;
    int shelf_life_hours;
    double ripening_index;
    double spoilage_probability;
    double cold_chain_risk;
} CInteropBananaRipenessPrediction;

_Static_assert(sizeof(int) == 4, "CInterop ABI requires 32-bit int.");
_Static_assert(sizeof(CInteropBananaBreakdown) == 12, "CInteropBananaBreakdown ABI size must be 12 bytes.");
_Static_assert(sizeof(double) == 8, "CInterop ABI requires 64-bit double.");
_Static_assert(sizeof(CInteropBananaRipenessPrediction) == 32, "CInteropBananaRipenessPrediction ABI size must be 32 bytes.");
_Static_assert(offsetof(CInteropBananaBreakdown, purchases) == 0, "CInteropBananaBreakdown.purchases ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaBreakdown, multiplier) == 4, "CInteropBananaBreakdown.multiplier ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaBreakdown, banana) == 8, "CInteropBananaBreakdown.banana ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaRipenessPrediction, predicted_stage) == 0, "CInteropBananaRipenessPrediction.predicted_stage ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaRipenessPrediction, shelf_life_hours) == 4, "CInteropBananaRipenessPrediction.shelf_life_hours ABI offset mismatch.");

/*
 * Interop decision:
 * - Wrapper keeps ABI primitive (int + pointer out params).
 * - Legacy module remains isolated and untouched behind this boundary.
 */
BANANA_API int banana_calculate_banana(int purchases, int multiplier, int* out_banana);

/*
 * Interop decision:
 * - Struct is blittable and fixed-layout for safe marshaling.
 */
BANANA_API int banana_calculate_banana_with_breakdown(
    int purchases,
    int multiplier,
    CInteropBananaBreakdown* out_breakdown
);

/*
 * Interop decision:
 * - Wrapper allocates UTF-8 bytes and caller must release with banana_free.
 */
BANANA_API int banana_create_banana_message(int purchases, int multiplier, char** out_message);

/*
 * Executes one atomic DB stage operation.
 * - Inputs are mapped to one SQL execution.
 * - Wrapper allocates UTF-8 JSON and caller must release with banana_free.
 */
BANANA_API int banana_db_query_banana(
    int purchases,
    int multiplier,
    char** out_payload,
    int* out_row_count
);

BANANA_API int banana_predict_banana_ripeness(
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double ethylene_exposure,
    double mechanical_damage,
    double storage_temp_c,
    CInteropBananaRipenessPrediction* out_prediction
);

BANANA_API int banana_create_batch(
    const char* batch_id,
    const char* origin_farm,
    double storage_temp_c,
    double ethylene_exposure,
    int estimated_shelf_life_days,
    char** out_batch_json
);

BANANA_API int banana_get_batch_status(
    const char* batch_id,
    char** out_batch_json
);

BANANA_API int banana_predict_batch_ripeness(
    const char* batch_id,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double mechanical_damage,
    CInteropBananaRipenessPrediction* out_prediction
);

BANANA_API void banana_free(void* pointer);

#endif
