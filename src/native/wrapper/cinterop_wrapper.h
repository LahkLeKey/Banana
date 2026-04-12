#ifndef CINTEROP_WRAPPER_H
#define CINTEROP_WRAPPER_H

#ifdef _WIN32
#define CINTEROP_API __declspec(dllexport)
#else
#define CINTEROP_API __attribute__((visibility("default")))
#endif

typedef enum CInteropStatus {
    CINTEROP_STATUS_OK = 0,
    CINTEROP_STATUS_INVALID_ARGUMENT = 1,
    CINTEROP_STATUS_OVERFLOW = 2,
    CINTEROP_STATUS_INTERNAL_ERROR = 3,
    CINTEROP_STATUS_DB_ERROR = 4,
    CINTEROP_STATUS_DB_NOT_CONFIGURED = 5
} CInteropStatus;

typedef struct CInteropPointsBreakdown {
    int purchases;
    int multiplier;
    int points;
} CInteropPointsBreakdown;

/*
 * Interop decision:
 * - Wrapper keeps ABI primitive (int + pointer out params).
 * - Legacy module remains isolated and untouched behind this boundary.
 */
CINTEROP_API int cinterop_calculate_points(int purchases, int multiplier, int* out_points);

/*
 * Interop decision:
 * - Struct is blittable and fixed-layout for safe marshaling.
 */
CINTEROP_API int cinterop_calculate_points_with_breakdown(
    int purchases,
    int multiplier,
    CInteropPointsBreakdown* out_breakdown
);

/*
 * Interop decision:
 * - Wrapper allocates UTF-8 bytes and caller must release with cinterop_free.
 */
CINTEROP_API int cinterop_create_points_message(int purchases, int multiplier, char** out_message);

/*
 * Executes one atomic DB stage operation.
 * - Inputs are mapped to one SQL execution.
 * - Wrapper allocates UTF-8 JSON and caller must release with cinterop_free.
 */
CINTEROP_API int cinterop_db_query_points(
    int purchases,
    int multiplier,
    char** out_payload,
    int* out_row_count
);

CINTEROP_API void cinterop_free(void* pointer);

#endif
