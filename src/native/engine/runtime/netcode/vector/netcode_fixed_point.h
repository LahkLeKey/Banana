#ifndef BANANA_ENGINE_RUNTIME_NETCODE_FIXED_POINT_H
#define BANANA_ENGINE_RUNTIME_NETCODE_FIXED_POINT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Banana uses signed Q16 fixed-point values for the k3h4 contract whenever it
 * needs deterministic, ABI-stable numeric payloads.
 *
 * Encoding:
 *   value_q16 = round(real_value * 2^16)
 *   real_value = value_q16 / 2^16
 *
 * So:
 *   1.0  -> 65536
 *   0.5  -> 32768
 *   1/16 -> 4096
 *
 * Arithmetic helpers keep results in the same Q16 domain so downstream code
 * can compare radii, distances, and scores without switching representations.
 */
#define NETCODE_Q16_SHIFT 16
#define NETCODE_Q16_ONE (1 << NETCODE_Q16_SHIFT)

    /* Encodes a real scalar into signed Q16 with nearest-integer rounding. */
    int32_t runtime_netcode_q16_from_float(float value);

    /* Decodes signed Q16 back to real-space scalar units. */
    float runtime_netcode_q16_to_float(int32_t value_q16);

    /*
     * Multiplies two Q16 values while preserving Q16 scale:
     *   q16_mul(a, b) = round((a * b) / 2^16)
     */
    int32_t runtime_netcode_q16_mul(int32_t lhs_q16, int32_t rhs_q16);

    /*
     * Divides one Q16 value by another and returns Q16:
     *   q16_div(a, b) = round((a * 2^16) / b)
     */
    int32_t runtime_netcode_q16_div(int32_t numerator_q16, int32_t denominator_q16);

    /*
     * Signed integer division with half-away-from-zero rounding and int32
     * saturation; used by Q16 helpers to keep deterministic edge handling.
     */
    int32_t runtime_netcode_q16_round_div(int64_t numerator, int64_t denominator);

#ifdef __cplusplus
}
#endif

#endif
