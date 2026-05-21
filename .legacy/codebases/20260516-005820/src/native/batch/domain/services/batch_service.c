#include "../entities/batch.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static BananaBatchRecord g_batches[BANANA_MAX_BATCHES];
static HarvestBatchRecord g_harvest_batches[BANANA_MAX_HARVEST_BATCHES];
static int g_batch_count = 0;
static int g_harvest_batch_count = 0;
static int g_next_batch_id = 1;
static int g_next_harvest_id = 1;

static int count_term_ci(const char *text, const char *term) {
    size_t term_len;
    int matches = 0;
    if (!text || !term) return 0;
    term_len = strlen(term);
    if (term_len == 0) return 0;
    for (size_t i = 0; text[i] != '\0'; ++i) {
        size_t j = 0;
        while (j < term_len && text[i + j] != '\0') {
            char a = (char)tolower((unsigned char)text[i + j]);
            char b = (char)tolower((unsigned char)term[j]);
            if (a != b) break;
            ++j;
        }
        if (j == term_len) {
            ++matches;
            i += term_len - 1;
        }
    }
    return matches;
}

static const char *infer_ripeness_hint(const char *text) {
    int unripe = count_term_ci(text, "green") + count_term_ci(text, "firm");
    int ripe = count_term_ci(text, "ripe") + count_term_ci(text, "yellow") + count_term_ci(text, "sweet");
    int overripe = count_term_ci(text, "overripe") + count_term_ci(text, "brown") + count_term_ci(text, "mushy");
    if (unripe >= ripe && unripe >= overripe) return "unripe";
    if (overripe > ripe) return "overripe";
    return "ripe";
}

static int estimate_item_count(const char *text) {
    int estimate = 0;
    estimate += count_term_ci(text, "bunch");
    estimate += count_term_ci(text, "crate");
    estimate += count_term_ci(text, "box");
    estimate += count_term_ci(text, "pallet");
    if (estimate <= 0) estimate = 1;
    if (estimate > 9999) estimate = 9999;
    return estimate;
}

static BananaBatchRecord *find_batch(const char *id) {
    if (!id) return NULL;
    for (int i = 0; i < g_batch_count; ++i) {
        if (strcmp(g_batches[i].id, id) == 0) return &g_batches[i];
    }
    return NULL;
}

static HarvestBatchRecord *find_harvest_batch(const char *id) {
    if (!id) return NULL;
    for (int i = 0; i < g_harvest_batch_count; ++i) {
        if (strcmp(g_harvest_batches[i].id, id) == 0) return &g_harvest_batches[i];
    }
    return NULL;
}

static int json_copy_out(char **out_json, const char *json)
{
    size_t len;
    char *buf;

    if (!out_json || !json)
        return -1;

    len = strlen(json);
    buf = (char *)malloc(len + 1u);
    if (!buf)
        return -1;

    memcpy(buf, json, len + 1u);
    *out_json = buf;
    return 0;
}

int batch_create(const char *input_json, char **out_json)
{
    BananaBatchRecord *record;
    char payload[256];

    if (!input_json || !out_json)
        return -1;
    if (g_batch_count >= BANANA_MAX_BATCHES)
        return -1;

    record = &g_batches[g_batch_count++];
    memset(record, 0, sizeof(*record));
    snprintf(record->id, sizeof(record->id), "batch-%04d", g_next_batch_id++);
    record->item_count_estimate = estimate_item_count(input_json);
    snprintf(record->ripeness_hint, sizeof(record->ripeness_hint), "%s",
             infer_ripeness_hint(input_json));

    snprintf(payload, sizeof(payload),
             "{\"id\":\"%s\",\"status\":\"created\",\"item_count_estimate\":%d,\"ripeness_hint\":\"%s\"}",
             record->id, record->item_count_estimate, record->ripeness_hint);
    return json_copy_out(out_json, payload);
}

int batch_get_status(const char *batch_id, char **out_json)
{
    BananaBatchRecord *record;
    char payload[256];

    if (!batch_id || !out_json)
        return -1;

    record = find_batch(batch_id);
    if (!record)
        return -2;

    snprintf(payload, sizeof(payload),
             "{\"id\":\"%s\",\"status\":\"active\",\"item_count_estimate\":%d,\"ripeness_hint\":\"%s\"}",
             record->id, record->item_count_estimate, record->ripeness_hint);
    return json_copy_out(out_json, payload);
}

int batch_predict_ripeness(const char *batch_id, char **out_json)
{
    BananaBatchRecord *record;
    char payload[192];

    if (!batch_id || !out_json)
        return -1;

    record = find_batch(batch_id);
    if (!record)
        return -2;

    snprintf(payload, sizeof(payload),
             "{\"id\":\"%s\",\"predicted_ripeness\":\"%s\"}", record->id,
             record->ripeness_hint);
    return json_copy_out(out_json, payload);
}

int harvest_create(const char *input_json, char **out_json)
{
    HarvestBatchRecord *record;
    char payload[192];

    (void)input_json;

    if (!out_json)
        return -1;
    if (g_harvest_batch_count >= BANANA_MAX_HARVEST_BATCHES)
        return -1;

    record = &g_harvest_batches[g_harvest_batch_count++];
    memset(record, 0, sizeof(*record));
    snprintf(record->id, sizeof(record->id), "harvest-%04d", g_next_harvest_id++);

    snprintf(payload, sizeof(payload),
             "{\"id\":\"%s\",\"status\":\"created\",\"bunch_count\":%d}", record->id,
             record->bunch_count);
    return json_copy_out(out_json, payload);
}

int harvest_add_bunch(const char *batch_id, const char *input_json, char **out_json)
{
    HarvestBatchRecord *record;
    int bump = 1;
    char payload[192];

    if (!batch_id || !out_json)
        return -1;

    record = find_harvest_batch(batch_id);
    if (!record)
        return -2;

    if (input_json)
    {
        int estimated = estimate_item_count(input_json);
        if (estimated > 0)
            bump = estimated;
    }
    record->bunch_count += bump;

    snprintf(payload, sizeof(payload),
             "{\"id\":\"%s\",\"status\":\"updated\",\"bunch_count\":%d}", record->id,
             record->bunch_count);
    return json_copy_out(out_json, payload);
}

int harvest_get_status(const char *batch_id, char **out_json)
{
    HarvestBatchRecord *record;
    char payload[192];

    if (!batch_id || !out_json)
        return -1;

    record = find_harvest_batch(batch_id);
    if (!record)
        return -2;

    snprintf(payload, sizeof(payload),
             "{\"id\":\"%s\",\"status\":\"active\",\"bunch_count\":%d}", record->id,
             record->bunch_count);
    return json_copy_out(out_json, payload);
}
