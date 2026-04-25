#include "banana_core.h"
#include "banana_status.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int banana_core_predict_ripeness(const char* input_json, char** out_json) {
	int unripe;
	int ripe;
	int overripe;
	const char* label = "ripe";
	double confidence = 0.60;
	int strongest;
	char payload[320];
	int written;

	if (!input_json || !out_json) return BANANA_INVALID_ARGUMENT;

	unripe = count_term_ci(input_json, "green") +
			 count_term_ci(input_json, "unripe") +
			 count_term_ci(input_json, "firm") +
			 count_term_ci(input_json, "hard");
	ripe = count_term_ci(input_json, "ripe") +
		   count_term_ci(input_json, "yellow") +
		   count_term_ci(input_json, "sweet") +
		   count_term_ci(input_json, "ready");
	overripe = count_term_ci(input_json, "overripe") +
			   count_term_ci(input_json, "brown") +
			   count_term_ci(input_json, "soft") +
			   count_term_ci(input_json, "mushy");

	strongest = ripe;
	if (unripe >= strongest && unripe >= overripe) {
		label = "unripe";
		strongest = unripe;
	} else if (overripe > strongest) {
		label = "overripe";
		strongest = overripe;
	}

	confidence = 0.55 + (0.08 * (double)strongest);
	if (confidence > 0.95) confidence = 0.95;

	written = snprintf(
		payload,
		sizeof(payload),
		"{\"label\":\"%s\",\"confidence\":%.6f,\"signals\":{\"unripe\":%d,\"ripe\":%d,\"overripe\":%d}}",
		label,
		confidence,
		unripe,
		ripe,
		overripe);
	if (written < 0 || written >= (int)sizeof(payload)) return BANANA_OVERFLOW;

	return duplicate_json(payload, out_json);
}
