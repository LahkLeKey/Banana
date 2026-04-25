/*
 * banana_core.h — Internal core API consumed by the wrapper layer.
 * Not part of the public ABI. Module count budget: ≤15 (spec 006 FR-003).
 */

#ifndef BANANA_CORE_H
#define BANANA_CORE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Calculate banana = purchases * multiplier with overflow guard.
 * Optional out_base/out_bonus may be NULL. Returns BananaStatusCode. */
int banana_core_calculate(int purchases, int multiplier,
                          int* out_total, int* out_base, int* out_bonus);

#ifdef __cplusplus
}
#endif

#endif
