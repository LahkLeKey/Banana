/*
 * banana_wrapper.c — Wrapper-layer ABI implementation. Thin marshalling
 * over banana_core (no business logic here). See spec 006 + ABI contract.
 *
 * Ownership: callee-allocated `char** out_json` outputs MUST be released by
 * the caller via banana_free().
 */

#include "banana_wrapper.h"
#include "banana_core.h"
#include "banana_dal.h"
#include "domain/ml/regression/banana_wrapper_ml_regression.h"
#include "domain/ml/binary/banana_wrapper_ml_binary.h"
#include "domain/ml/transformer/banana_wrapper_ml_transformer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* === Calculation === */

int banana_calculate_banana(int purchases, int multiplier, int* out_banana) {
    return banana_core_calculate(purchases, multiplier, out_banana, NULL, NULL);
}

int banana_calculate_banana_with_breakdown(int purchases, int multiplier,
                                            int* out_banana, int* out_base, int* out_bonus) {
    return banana_core_calculate(purchases, multiplier, out_banana, out_base, out_bonus);
}

int banana_create_banana_message(int purchases, int multiplier, char** out_message) {
    if (!out_message) return BANANA_INVALID_ARGUMENT;
    int total = 0;
    int rc = banana_core_calculate(purchases, multiplier, &total, NULL, NULL);
    if (rc != BANANA_OK) return rc;

    /* 96 bytes covers worst-case formatted message under int32 bounds. */
    char* buf = (char*)malloc(96);
    if (!buf) return BANANA_INTERNAL_ERROR;
    int written = snprintf(buf, 96, "purchases=%d multiplier=%d banana=%d", purchases, multiplier, total);
    if (written < 0 || written >= 96) { free(buf); return BANANA_OVERFLOW; }
    *out_message = buf;
    return BANANA_OK;
}

/* === DAL === */

int banana_db_query_banana_profile(const char* profile_id, char** out_json) {
    return banana_dal_query_profile(profile_id, out_json);
}

/* === Classifier / ML === */

int banana_predict_banana_regression_score(const char* input_json, double* out_score) {
    return banana_wrapper_ml_predict_regression_score(input_json, out_score);
}

int banana_classify_banana_binary(const char* input_json, char** out_json) {
    return banana_wrapper_ml_classify_binary(input_json, out_json);
}

int banana_classify_banana_binary_with_threshold(const char* input_json,
                                                  double threshold,
                                                  char** out_json) {
    return banana_wrapper_ml_classify_binary_with_threshold(input_json, threshold, out_json);
}

int banana_classify_banana_transformer(const char* input_json, char** out_json) {
    return banana_wrapper_ml_classify_transformer(input_json, out_json);
}

int banana_classify_banana_transformer_ex(const char* input_json,
                                           int log_attention,
                                           double* out_embedding,
                                           double* out_attention_weights,
                                           char** out_json) {
    return banana_wrapper_ml_classify_transformer_ex(input_json, log_attention,
                                                      out_embedding,
                                                      out_attention_weights,
                                                      out_json);
}

int banana_classify_banana_transformer_quant_embedding(const char* input_json,
                                                        signed char* out_quant,
                                                        double* out_scale,
                                                        signed char* out_zero,
                                                        char** out_json) {
    return banana_wrapper_ml_classify_transformer_quant_embedding(input_json,
                                                                    out_quant,
                                                                    out_scale,
                                                                    out_zero,
                                                                    out_json);
}

int banana_classify_not_banana_junk(const char* input_json, char** out_json) {
    return banana_wrapper_ml_classify_not_banana_junk(input_json, out_json);
}

int banana_predict_banana_ripeness(const char* input_json, char** out_json) {
    return banana_core_predict_ripeness(input_json, out_json);
}

/* === Batch / harvest / truck. === */

int banana_create_batch(const char* input_json, char** out_json) {
    return banana_core_create_batch(input_json, out_json);
}

int banana_get_batch_status(const char* batch_id, char** out_json) {
    return banana_core_get_batch_status(batch_id, out_json);
}

int banana_predict_batch_ripeness(const char* batch_id, char** out_json) {
    return banana_core_predict_batch_ripeness(batch_id, out_json);
}

int banana_create_harvest_batch(const char* input_json, char** out_json) {
    return banana_core_create_harvest_batch(input_json, out_json);
}

int banana_add_bunch_to_harvest_batch(
    const char* batch_id, const char* input_json, char** out_json) {
    return banana_core_add_bunch_to_harvest_batch(batch_id, input_json, out_json);
}

int banana_get_harvest_batch_status(const char* batch_id, char** out_json) {
    return banana_core_get_harvest_batch_status(batch_id, out_json);
}

int banana_register_truck(const char* input_json, char** out_json) {
    return banana_core_register_truck(input_json, out_json);
}

int banana_load_truck_container(
    const char* truck_id, const char* input_json, char** out_json) {
    return banana_core_load_truck_container(truck_id, input_json, out_json);
}

int banana_unload_truck_container(
    const char* truck_id, const char* container_id, char** out_json) {
    return banana_core_unload_truck_container(truck_id, container_id, out_json);
}

int banana_relocate_truck(
    const char* truck_id, const char* input_json, char** out_json) {
    return banana_core_relocate_truck(truck_id, input_json, out_json);
}

int banana_get_truck_status(const char* truck_id, char** out_json) {
    return banana_core_get_truck_status(truck_id, out_json);
}

/* === Memory === */
void banana_free(void* pointer) {
    free(pointer);
}

/* === ABI version query (feature 072) === */
int banana_native_version(int* out_major, int* out_minor) {
    *out_major = BANANA_WRAPPER_ABI_VERSION_MAJOR;
    *out_minor = BANANA_WRAPPER_ABI_VERSION_MINOR;
    return 0; /* BANANA_OK */
}
