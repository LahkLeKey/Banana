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

/* === Classifier (stub: deterministic placeholders; spec 006 FR-003 says
       v2 keeps modules with consumers — these are consumed by ASP.NET 007). === */

int banana_predict_banana_regression_score(const char* input_json, double* out_score) {
    if (!input_json || !out_score) return BANANA_INVALID_ARGUMENT;
    *out_score = 0.5;
    return BANANA_OK;
}

static int classifier_stub_json(const char* label, char** out_json) {
    if (!out_json) return BANANA_INVALID_ARGUMENT;
    const char* fmt = "{\"label\":\"%s\",\"confidence\":0.5}";
    size_t n = strlen(fmt) + strlen(label) + 1;
    char* buf = (char*)malloc(n);
    if (!buf) return BANANA_INTERNAL_ERROR;
    snprintf(buf, n, fmt, label);
    *out_json = buf;
    return BANANA_OK;
}

int banana_classify_banana_binary(const char* input_json, char** out_json) {
    (void)input_json;
    return classifier_stub_json("banana", out_json);
}

int banana_classify_banana_transformer(const char* input_json, char** out_json) {
    (void)input_json;
    return classifier_stub_json("banana", out_json);
}

int banana_classify_not_banana_junk(const char* input_json, char** out_json) {
    (void)input_json;
    return classifier_stub_json("not_banana", out_json);
}

int banana_predict_banana_ripeness(const char* input_json, char** out_json) {
    (void)input_json;
    return classifier_stub_json("ripe", out_json);
}

/* === Batch / harvest / truck (stub passthroughs; spec 006 keeps surface). === */

static int json_passthrough(const char* label, char** out_json) {
    if (!out_json) return BANANA_INVALID_ARGUMENT;
    size_t n = strlen(label) + 32;
    char* buf = (char*)malloc(n);
    if (!buf) return BANANA_INTERNAL_ERROR;
    snprintf(buf, n, "{\"status\":\"%s\"}", label);
    *out_json = buf;
    return BANANA_OK;
}

int banana_create_batch(const char* input_json, char** out_json) { (void)input_json; return json_passthrough("created", out_json); }
int banana_get_batch_status(const char* batch_id, char** out_json) { (void)batch_id; return json_passthrough("ok", out_json); }
int banana_predict_batch_ripeness(const char* batch_id, char** out_json) { (void)batch_id; return json_passthrough("ripe", out_json); }
int banana_create_harvest_batch(const char* input_json, char** out_json) { (void)input_json; return json_passthrough("created", out_json); }
int banana_add_bunch_to_harvest_batch(const char* batch_id, const char* input_json, char** out_json) { (void)batch_id; (void)input_json; return json_passthrough("added", out_json); }
int banana_get_harvest_batch_status(const char* batch_id, char** out_json) { (void)batch_id; return json_passthrough("ok", out_json); }
int banana_register_truck(const char* input_json, char** out_json) { (void)input_json; return json_passthrough("registered", out_json); }
int banana_load_truck_container(const char* truck_id, const char* input_json, char** out_json) { (void)truck_id; (void)input_json; return json_passthrough("loaded", out_json); }
int banana_unload_truck_container(const char* truck_id, const char* container_id, char** out_json) { (void)truck_id; (void)container_id; return json_passthrough("unloaded", out_json); }
int banana_relocate_truck(const char* truck_id, const char* input_json, char** out_json) { (void)truck_id; (void)input_json; return json_passthrough("relocated", out_json); }
int banana_get_truck_status(const char* truck_id, char** out_json) { (void)truck_id; return json_passthrough("ok", out_json); }

/* === Memory === */
void banana_free(void* pointer) {
    free(pointer);
}
