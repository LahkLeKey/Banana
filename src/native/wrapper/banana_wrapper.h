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
    BANANA_STATUS_DB_NOT_CONFIGURED = 5
} CInteropStatus;

typedef struct CInteropBananaBreakdown {
    int purchases;
    int multiplier;
    int banana;
} CInteropBananaBreakdown;

_Static_assert(sizeof(int) == 4, "CInterop ABI requires 32-bit int.");
_Static_assert(sizeof(CInteropBananaBreakdown) == 12, "CInteropBananaBreakdown ABI size must be 12 bytes.");
_Static_assert(offsetof(CInteropBananaBreakdown, purchases) == 0, "CInteropBananaBreakdown.purchases ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaBreakdown, multiplier) == 4, "CInteropBananaBreakdown.multiplier ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaBreakdown, banana) == 8, "CInteropBananaBreakdown.banana ABI offset mismatch.");

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

BANANA_API void banana_free(void* pointer);

#endif
