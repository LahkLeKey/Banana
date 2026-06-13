#include "netcode_fixed_point.h"

#include <limits.h>
#include <math.h>

static int64_t clamp_i64(int64_t value, int64_t min_value, int64_t max_value)
{
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

int32_t runtime_netcode_q16_round_div(int64_t numerator, int64_t denominator)
{
    int64_t quotient;
    int64_t remainder;
    int64_t abs_remainder_twice;
    int64_t abs_denominator;

    if (denominator == 0)
    {
        return numerator >= 0 ? INT32_MAX : INT32_MIN;
    }

    quotient = numerator / denominator;
    remainder = numerator % denominator;

    if (remainder != 0)
    {
        abs_remainder_twice = llabs(remainder) * 2;
        abs_denominator = llabs(denominator);

        if (abs_remainder_twice >= abs_denominator)
        {
            if ((numerator > 0 && denominator > 0) || (numerator < 0 && denominator < 0))
            {
                quotient += 1;
            }
            else
            {
                quotient -= 1;
            }
        }
    }

    quotient = clamp_i64(quotient, (int64_t)INT32_MIN, (int64_t)INT32_MAX);
    return (int32_t)quotient;
}

int32_t runtime_netcode_q16_from_float(float value)
{
    double scaled = (double)value * (double)NETCODE_Q16_ONE;
    double adjusted = scaled >= 0.0 ? scaled + 0.5 : scaled - 0.5;
    int64_t rounded = (int64_t)adjusted;

    if (rounded > INT32_MAX) return INT32_MAX;
    if (rounded < INT32_MIN) return INT32_MIN;
    return (int32_t)rounded;
}

float runtime_netcode_q16_to_float(int32_t value_q16)
{
    return (float)value_q16 / (float)NETCODE_Q16_ONE;
}

int32_t runtime_netcode_q16_mul(int32_t lhs_q16, int32_t rhs_q16)
{
    int64_t product = (int64_t)lhs_q16 * (int64_t)rhs_q16;
    return runtime_netcode_q16_round_div(product, NETCODE_Q16_ONE);
}

int32_t runtime_netcode_q16_div(int32_t numerator_q16, int32_t denominator_q16)
{
    int64_t shifted;

    if (denominator_q16 == 0)
    {
        return numerator_q16 >= 0 ? INT32_MAX : INT32_MIN;
    }

    shifted = ((int64_t)numerator_q16) << NETCODE_Q16_SHIFT;
    return runtime_netcode_q16_round_div(shifted, (int64_t)denominator_q16);
}
