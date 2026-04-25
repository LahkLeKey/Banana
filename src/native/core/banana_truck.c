#include "banana_core.h"
#include "banana_status.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BANANA_MAX_TRUCKS 128

typedef struct TruckRecord {
	char id[32];
	int container_count;
	char location[32];
} TruckRecord;

static TruckRecord g_trucks[BANANA_MAX_TRUCKS];
static int g_truck_count = 0;
static int g_next_truck_id = 1;

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

static TruckRecord* find_truck(const char* truck_id) {
	if (!truck_id) return NULL;
	for (int i = 0; i < g_truck_count; ++i) {
		if (strcmp(g_trucks[i].id, truck_id) == 0) return &g_trucks[i];
	}
	return NULL;
}

static int contains_ci(const char* text, const char* token) {
	size_t token_len;

	if (!text || !token) return 0;
	token_len = strlen(token);
	if (token_len == 0) return 0;

	for (size_t i = 0; text[i] != '\0'; ++i) {
		size_t j = 0;
		while (j < token_len && text[i + j] != '\0') {
			char a = (char)tolower((unsigned char)text[i + j]);
			char b = (char)tolower((unsigned char)token[j]);
			if (a != b) break;
			++j;
		}
		if (j == token_len) return 1;
	}

	return 0;
}

static const char* infer_location(const char* text) {
	if (!text) return "yard";
	if (contains_ci(text, "warehouse")) return "warehouse";
	if (contains_ci(text, "port")) return "port";
	if (contains_ci(text, "farm")) return "farm";
	if (contains_ci(text, "market")) return "market";
	return "yard";
}

int banana_core_register_truck(const char* input_json, char** out_json) {
	TruckRecord* record;
	char payload[256];
	int written;

	if (!input_json || !out_json) return BANANA_INVALID_ARGUMENT;
	if (g_truck_count >= BANANA_MAX_TRUCKS) return BANANA_INTERNAL_ERROR;

	record = &g_trucks[g_truck_count++];
	snprintf(record->id, sizeof(record->id), "truck-%04d", g_next_truck_id++);
	record->container_count = 0;
	snprintf(record->location, sizeof(record->location), "%s", infer_location(input_json));

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"truck_id\":\"%s\",\"status\":\"registered\",\"location\":\"%s\",\"container_count\":%d}",
		record->id,
		record->location,
		record->container_count);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_load_truck_container(
	const char* truck_id, const char* input_json, char** out_json) {
	TruckRecord* record;
	char payload[256];
	int written;
	(void)input_json;

	if (!truck_id || !input_json || !out_json) return BANANA_INVALID_ARGUMENT;

	record = find_truck(truck_id);
	if (!record) return BANANA_NOT_FOUND;

	if (record->container_count < 9999) {
		record->container_count += 1;
	}

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"truck_id\":\"%s\",\"status\":\"loaded\",\"location\":\"%s\",\"container_count\":%d}",
		record->id,
		record->location,
		record->container_count);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_unload_truck_container(
	const char* truck_id, const char* container_id, char** out_json) {
	TruckRecord* record;
	char payload[256];
	int written;
	(void)container_id;

	if (!truck_id || !container_id || !out_json) return BANANA_INVALID_ARGUMENT;

	record = find_truck(truck_id);
	if (!record) return BANANA_NOT_FOUND;
	if (record->container_count <= 0) return BANANA_NOT_FOUND;

	record->container_count -= 1;

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"truck_id\":\"%s\",\"status\":\"unloaded\",\"location\":\"%s\",\"container_count\":%d}",
		record->id,
		record->location,
		record->container_count);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_relocate_truck(
	const char* truck_id, const char* input_json, char** out_json) {
	TruckRecord* record;
	char payload[256];
	int written;

	if (!truck_id || !input_json || !out_json) return BANANA_INVALID_ARGUMENT;

	record = find_truck(truck_id);
	if (!record) return BANANA_NOT_FOUND;

	snprintf(record->location, sizeof(record->location), "%s", infer_location(input_json));

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"truck_id\":\"%s\",\"status\":\"relocated\",\"location\":\"%s\",\"container_count\":%d}",
		record->id,
		record->location,
		record->container_count);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}

int banana_core_get_truck_status(const char* truck_id, char** out_json) {
	TruckRecord* record;
	char payload[256];
	int written;

	if (!truck_id || !out_json) return BANANA_INVALID_ARGUMENT;

	record = find_truck(truck_id);
	if (!record) return BANANA_NOT_FOUND;

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"truck_id\":\"%s\",\"status\":\"active\",\"location\":\"%s\",\"container_count\":%d}",
		record->id,
		record->location,
		record->container_count);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}
