#include "cinterop_wrapper.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "../legacy/legacy_db.h"
#include "../legacy/legacy_points.h"

static int should_force_message_alloc_failure(void) {
    const char* flag = getenv("CINTEROP_FORCE_MESSAGE_ALLOC_FAIL");
    return flag != 0 && flag[0] == '1';
}

static int validate_input(int purchases, int multiplier) {
    if (purchases < 0 || multiplier < 0) {
        return CINTEROP_STATUS_INVALID_ARGUMENT;
    }

    if (purchases > (INT_MAX / 10)) {
        return CINTEROP_STATUS_OVERFLOW;
    }

    return CINTEROP_STATUS_OK;
}

int cinterop_calculate_points(int purchases, int multiplier, int* out_points) {
    int status = validate_input(purchases, multiplier);
    if (status != CINTEROP_STATUS_OK) {
        return status;
    }

    if (out_points == 0) {
        return CINTEROP_STATUS_INVALID_ARGUMENT;
    }

    *out_points = legacy_calculate_points(purchases, multiplier);
    return CINTEROP_STATUS_OK;
}

int cinterop_calculate_points_with_breakdown(
    int purchases,
    int multiplier,
    CInteropPointsBreakdown* out_breakdown
) {
    LegacyPointsSummary summary;
    int status = validate_input(purchases, multiplier);
    if (status != CINTEROP_STATUS_OK) {
        return status;
    }

    if (out_breakdown == 0) {
        return CINTEROP_STATUS_INVALID_ARGUMENT;
    }

    if (legacy_calculate_summary(purchases, multiplier, &summary) != 0) {
        return CINTEROP_STATUS_INTERNAL_ERROR;
    }

    out_breakdown->purchases = summary.purchases;
    out_breakdown->multiplier = summary.multiplier;
    out_breakdown->points = summary.total_points;

    return CINTEROP_STATUS_OK;
}

int cinterop_create_points_message(int purchases, int multiplier, char** out_message) {
    int points = 0;
    int status = CINTEROP_STATUS_OK;
    char* message = 0;
    int required_bytes = 0;

    if (out_message == 0) {
        return CINTEROP_STATUS_INVALID_ARGUMENT;
    }

    *out_message = 0;

    status = cinterop_calculate_points(purchases, multiplier, &points);
    if (status != CINTEROP_STATUS_OK) {
        return status;
    }

    required_bytes = snprintf(0, 0, "purchases=%d multiplier=%d points=%d", purchases, multiplier, points);
    if (required_bytes < 0) {
        return CINTEROP_STATUS_INTERNAL_ERROR; /* GCOVR_EXCL_LINE */
    }

    if (should_force_message_alloc_failure()) {
        message = 0;
    } else {
        message = (char*)malloc((size_t)required_bytes + 1U);
    }

    if (message == 0) {
        return CINTEROP_STATUS_INTERNAL_ERROR;
    }

    if (snprintf(message, (size_t)required_bytes + 1U, "purchases=%d multiplier=%d points=%d", purchases, multiplier, points) < 0) {
        free(message); /* GCOVR_EXCL_LINE */
        return CINTEROP_STATUS_INTERNAL_ERROR; /* GCOVR_EXCL_LINE */
    }

    *out_message = message;
    return CINTEROP_STATUS_OK;
}

int cinterop_db_query_points(
    int purchases,
    int multiplier,
    char** out_payload,
    int* out_row_count
) {
    int status = validate_input(purchases, multiplier);
    int db_result = 0;

    if (status != CINTEROP_STATUS_OK) {
        return status;
    }

    if (out_payload == 0 || out_row_count == 0) {
        return CINTEROP_STATUS_INVALID_ARGUMENT;
    }

    *out_payload = 0;
    *out_row_count = 0;

    db_result = legacy_db_query_points(purchases, multiplier, out_payload, out_row_count);
    if (db_result == 0) {
        return CINTEROP_STATUS_OK;
    }

    if (db_result == 2) {
        return CINTEROP_STATUS_DB_NOT_CONFIGURED;
    }

    if (db_result == 3) {
        return CINTEROP_STATUS_DB_ERROR;
    }

    return CINTEROP_STATUS_INTERNAL_ERROR;
}

void cinterop_free(void* pointer) {
    if (pointer != 0) {
        free(pointer);
    }
}
