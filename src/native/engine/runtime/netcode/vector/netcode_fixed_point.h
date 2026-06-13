#ifndef BANANA_ENGINE_RUNTIME_NETCODE_FIXED_POINT_H
#define BANANA_ENGINE_RUNTIME_NETCODE_FIXED_POINT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define NETCODE_Q16_SHIFT 16
#define NETCODE_Q16_ONE (1 << NETCODE_Q16_SHIFT)

    int32_t runtime_netcode_q16_from_float(float value);
    float runtime_netcode_q16_to_float(int32_t value_q16);
    int32_t runtime_netcode_q16_mul(int32_t lhs_q16, int32_t rhs_q16);
    int32_t runtime_netcode_q16_div(int32_t numerator_q16, int32_t denominator_q16);
    int32_t runtime_netcode_q16_round_div(int64_t numerator, int64_t denominator);

#ifdef __cplusplus
}
#endif

#endif
