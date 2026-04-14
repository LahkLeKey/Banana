#include "banana_wrapper.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "../legacy/legacy_db.h"
#include "../legacy/legacy_banana.h"
#include "../testing/native_test_hooks.h"

static int validate_input(int purchases, int multiplier) {
    int base_banana = 0;
    int bonus_banana = 0;

    if (purchases < 0 || multiplier < 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    if (purchases > (INT_MAX / 10)) {
        return BANANA_STATUS_OVERFLOW;
    }

    if (purchases >= 10 && multiplier > (INT_MAX / 25)) {
        return BANANA_STATUS_OVERFLOW;
    }

    base_banana = purchases * 10;
    bonus_banana = (purchases >= 10) ? (multiplier * 25) : 0;
    if (base_banana > (INT_MAX - bonus_banana)) {
        return BANANA_STATUS_OVERFLOW;
    }

    return BANANA_STATUS_OK;
}

int banana_calculate_banana(int purchases, int multiplier, int* out_banana) {
    int status = validate_input(purchases, multiplier);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    if (out_banana == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_banana = legacy_calculate_banana(purchases, multiplier);
    return BANANA_STATUS_OK;
}

int banana_calculate_banana_with_breakdown(
    int purchases,
    int multiplier,
    CInteropBananaBreakdown* out_breakdown
) {
    LegacyBananaSummary summary;
    int status = validate_input(purchases, multiplier);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    if (out_breakdown == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    if (banana_test_hook_force_summary_failure()) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    if (legacy_calculate_summary(purchases, multiplier, &summary) != 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    out_breakdown->purchases = summary.purchases;
    out_breakdown->multiplier = summary.multiplier;
    out_breakdown->banana = summary.banana;

    return BANANA_STATUS_OK;
}

int banana_create_banana_message(int purchases, int multiplier, char** out_message) {
    int banana = 0;
    int status = BANANA_STATUS_OK;
    char* message = 0;
    int required_bytes = 0;

    if (out_message == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_message = 0;

    status = banana_calculate_banana(purchases, multiplier, &banana);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    required_bytes = snprintf(0, 0, "purchases=%d multiplier=%d banana=%d", purchases, multiplier, banana);
    if (required_bytes < 0) {
        return BANANA_STATUS_INTERNAL_ERROR; /* GCOVR_EXCL_LINE */
    }

    if (banana_test_hook_force_message_alloc_failure()) {
        message = 0;
    } else {
        message = (char*)malloc((size_t)required_bytes + 1U);
    }

    if (message == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    if (snprintf(message, (size_t)required_bytes + 1U, "purchases=%d multiplier=%d banana=%d", purchases, multiplier, banana) < 0) {
        free(message); /* GCOVR_EXCL_LINE */
        return BANANA_STATUS_INTERNAL_ERROR; /* GCOVR_EXCL_LINE */
    }

    *out_message = message;
    return BANANA_STATUS_OK;
}

int banana_db_query_banana(
    int purchases,
    int multiplier,
    char** out_payload,
    int* out_row_count
) {
    int status = validate_input(purchases, multiplier);
    int db_result = 0;

    if (status != BANANA_STATUS_OK) {
        return status;
    }

    if (out_payload == 0 || out_row_count == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_payload = 0;
    *out_row_count = 0;

    db_result = legacy_db_query_banana(purchases, multiplier, out_payload, out_row_count);
    if (db_result == 0) {
        return BANANA_STATUS_OK;
    }

    if (db_result == 2) {
        return BANANA_STATUS_DB_NOT_CONFIGURED;
    }

    if (db_result == 3) {
        return BANANA_STATUS_DB_ERROR;
    }

    return BANANA_STATUS_INTERNAL_ERROR;
}

void banana_free(void* pointer) {
    if (pointer != 0) {
        free(pointer);
    }
}
