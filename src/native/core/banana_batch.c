#include "banana_core.h"
#include "banana_status.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BANANA_MAX_BATCHES 128
#define BANANA_MAX_HARVEST_BATCHES 128

typedef struct BananaBatchRecord {
	char id[32];
	int item_count_estimate;
	char ripeness_hint[16];
} BananaBatchRecord;

typedef struct HarvestBatchRecord {
	char id[32];
	int bunch_count;
} HarvestBatchRecord;

static BananaBatchRecord g_batches[BANANA_MAX_BATCHES];
static HarvestBatchRecord g_harvest_batches[BANANA_MAX_HARVEST_BATCHES];
static int g_batch_count = 0;
static int g_harvest_batch_count = 0;
static int g_next_batch_id = 1;
static int g_next_harvest_id = 1;

static int count_term_ci(const char* text, const char* term) {
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

static int duplicate_json(const char* payload, char** out_json) {
	size_t n;
	char* buf;
	if (!payload || !out_json) return BANANA_INVALID_ARGUMENT;
	n = strlen(payload) + 1;
	buf = (char*)malloc(n);
	if (!buf) return BANANA_INTERNAL_ERROR;
	memcpy(buf, payload, n);
	*out_json = buf;
	return BANANA_OK;
}

static const char* infer_ripeness_hint(const char* text) {
	int unripe = count_term_ci(text, "green") + count_term_ci(text, "firm");
	int ripe = count_term_ci(text, "ripe") + count_term_ci(text, "yellow") +
			   count_term_ci(text, "sweet");
	int overripe = count_term_ci(text, "overripe") + count_term_ci(text, "brown") +
				   count_term_ci(text, "mushy");

	if (unripe >= ripe && unripe >= overripe) return "unripe";
	if (overripe > ripe) return "overripe";
	return "ripe";
}

static int estimate_item_count(const char* text) {
	int estimate = 0;
	estimate += count_term_ci(text, "bunch");
	estimate += count_term_ci(text, "crate");
	estimate += count_term_ci(text, "box");
	estimate += count_term_ci(text, "pallet");
	if (estimate <= 0) estimate = 1;
	if (estimate > 9999) estimate = 9999;
	return estimate;
}

static BananaBatchRecord* find_batch(const char* id) {
	if (!id) return NULL;
	for (int i = 0; i < g_batch_count; ++i) {
		if (strcmp(g_batches[i].id, id) == 0) return &g_batches[i];
	}
	return NULL;
}

static HarvestBatchRecord* find_harvest_batch(const char* id) {
	if (!id) return NULL;
	for (int i = 0; i < g_harvest_batch_count; ++i) {
		if (strcmp(g_harvest_batches[i].id, id) == 0) return &g_harvest_batches[i];
	}
	return NULL;
}

int banana_core_create_batch(const char* input_json, char** out_json) {
	BananaBatchRecord* record;
	char payload[320];
	int written;

	if (!input_json || !out_json) return BANANA_INVALID_ARGUMENT;
	if (g_batch_count >= BANANA_MAX_BATCHES) return BANANA_INTERNAL_ERROR;

	record = &g_batches[g_batch_count++];
	snprintf(record->id, sizeof(record->id), "batch-%04d", g_next_batch_id++);
	record->item_count_estimate = estimate_item_count(input_json);
	snprintf(record->ripeness_hint, sizeof(record->ripeness_hint), "%s", infer_ripeness_hint(input_json));

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"batch_id\":\"%s\",\"status\":\"created\",\"item_count_estimate\":%d,\"ripeness_hint\":\"%s\"}",
		record->id,
		record->item_count_estimate,
		record->ripeness_hint);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_get_batch_status(const char* batch_id, char** out_json) {
	BananaBatchRecord* record;
	char payload[320];
	int written;

	if (!batch_id || !out_json) return BANANA_INVALID_ARGUMENT;

	record = find_batch(batch_id);
	if (!record) return BANANA_NOT_FOUND;

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"batch_id\":\"%s\",\"status\":\"active\",\"item_count_estimate\":%d,\"ripeness_hint\":\"%s\"}",
		record->id,
		record->item_count_estimate,
		record->ripeness_hint);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_predict_batch_ripeness(const char* batch_id, char** out_json) {
	BananaBatchRecord* record;
	double confidence;
	char payload[256];
	int written;

	if (!batch_id || !out_json) return BANANA_INVALID_ARGUMENT;

	record = find_batch(batch_id);
	if (!record) return BANANA_NOT_FOUND;

	confidence = 0.60;
	if (strcmp(record->ripeness_hint, "ripe") == 0) confidence = 0.82;
	else if (strcmp(record->ripeness_hint, "overripe") == 0) confidence = 0.78;
	else if (strcmp(record->ripeness_hint, "unripe") == 0) confidence = 0.74;

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"batch_id\":\"%s\",\"label\":\"%s\",\"confidence\":%.6f}",
		record->id,
		record->ripeness_hint,
		confidence);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_create_harvest_batch(const char* input_json, char** out_json) {
	HarvestBatchRecord* record;
	char payload[256];
	int written;
	(void)input_json;

	if (!out_json) return BANANA_INVALID_ARGUMENT;
	if (g_harvest_batch_count >= BANANA_MAX_HARVEST_BATCHES) return BANANA_INTERNAL_ERROR;

	record = &g_harvest_batches[g_harvest_batch_count++];
	snprintf(record->id, sizeof(record->id), "harvest-%04d", g_next_harvest_id++);
	record->bunch_count = 0;

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"harvest_batch_id\":\"%s\",\"status\":\"created\",\"bunch_count\":%d}",
		record->id,
		record->bunch_count);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_add_bunch_to_harvest_batch(
	const char* batch_id, const char* input_json, char** out_json) {
	HarvestBatchRecord* record;
	int bunch_increment;
	char payload[256];
	int written;

	if (!batch_id || !input_json || !out_json) return BANANA_INVALID_ARGUMENT;

	record = find_harvest_batch(batch_id);
	if (!record) return BANANA_NOT_FOUND;

	bunch_increment = estimate_item_count(input_json);
	if (bunch_increment < 1) bunch_increment = 1;
	if (bunch_increment > 8) bunch_increment = 8;
	record->bunch_count += bunch_increment;

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"harvest_batch_id\":\"%s\",\"status\":\"updated\",\"bunch_count\":%d}",
		record->id,
		record->bunch_count);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_get_harvest_batch_status(const char* batch_id, char** out_json) {
	HarvestBatchRecord* record;
	char payload[256];
	int written;

	if (!batch_id || !out_json) return BANANA_INVALID_ARGUMENT;

	record = find_harvest_batch(batch_id);
	if (!record) return BANANA_NOT_FOUND;

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"harvest_batch_id\":\"%s\",\"status\":\"active\",\"bunch_count\":%d}",
		record->id,
		record->bunch_count);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}
