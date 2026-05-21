#include <limits.h>
#include <stdint.h>

int calc_calculate(int purchases, int multiplier, int *out_total, int *out_base, int *out_bonus)
{
    if (!out_total)
        return -1;
    if (purchases < 0 || multiplier < 0)
        return -1;
    int64_t product = (int64_t)purchases * (int64_t)multiplier;
    if (product > INT_MAX)
        return -2;
    int total = (int)product;
    *out_total = total;
    if (out_base)
        *out_base = purchases;
    if (out_bonus)
        *out_bonus = total - purchases;
    return 0;
}
