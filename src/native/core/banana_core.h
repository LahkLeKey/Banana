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

/* Ripeness domain. */
int banana_core_predict_ripeness(const char* input_json, char** out_json);

/* Batch domain. */
int banana_core_create_batch(const char* input_json, char** out_json);
int banana_core_get_batch_status(const char* batch_id, char** out_json);
int banana_core_predict_batch_ripeness(const char* batch_id, char** out_json);

/* Harvest batch domain. */
int banana_core_create_harvest_batch(const char* input_json, char** out_json);
int banana_core_add_bunch_to_harvest_batch(
    const char* batch_id, const char* input_json, char** out_json);
int banana_core_get_harvest_batch_status(const char* batch_id, char** out_json);

/* Truck/logistics domain. */
int banana_core_register_truck(const char* input_json, char** out_json);
int banana_core_load_truck_container(
    const char* truck_id, const char* input_json, char** out_json);
int banana_core_unload_truck_container(
    const char* truck_id, const char* container_id, char** out_json);
int banana_core_relocate_truck(
    const char* truck_id, const char* input_json, char** out_json);
int banana_core_get_truck_status(const char* truck_id, char** out_json);

#ifdef __cplusplus
}
#endif

#endif
