#include "banana_core.h"
#include "banana_status.h"

#include <limits.h>

int banana_core_calculate(int purchases, int multiplier,
                          int* out_total, int* out_base, int* out_bonus) {
    if (!out_total) return BANANA_INVALID_ARGUMENT;
    if (purchases < 0 || multiplier < 0) return BANANA_INVALID_ARGUMENT;

    /* Overflow-safe: int64 product, range-check before truncation. */
    int64_t product = (int64_t)purchases * (int64_t)multiplier;
    if (product > INT_MAX) return BANANA_OVERFLOW;

    int total = (int)product;
    *out_total = total;
    if (out_base)  *out_base  = purchases;
    if (out_bonus) *out_bonus = total - purchases;
    return BANANA_OK;
}
