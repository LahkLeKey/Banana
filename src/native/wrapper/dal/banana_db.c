#include "../banana_wrapper_internal.h"

#include "../../core/dal/banana_db.h"
#include "../../core/domain/banana_profile.h"

int banana_db_query_banana_profile(
    int purchases,
    int multiplier,
    char** out_payload,
    int* out_row_count
) {
    BananaInput input;
    BananaRules rules = banana_default_rules();
    int status = BANANA_STATUS_OK;
    int db_result = 0;

    input.purchases = purchases;
    input.multiplier = multiplier;

    status = banana_wrapper_map_status(banana_validate_input(&input, &rules));
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    if (out_payload == 0 || out_row_count == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_payload = 0;
    *out_row_count = 0;

    db_result = banana_db_query(purchases, multiplier, out_payload, out_row_count);
    if (db_result == BANANA_DB_OK) {
        return BANANA_STATUS_OK;
    }

    if (db_result == BANANA_DB_NOT_CONFIGURED) {
        return BANANA_STATUS_DB_NOT_CONFIGURED;
    }

    if (db_result == BANANA_DB_ERROR) {
        return BANANA_STATUS_DB_ERROR;
    }

    return BANANA_STATUS_INTERNAL_ERROR;
}

int banana_db_query_banana(
    int purchases,
    int multiplier,
    char** out_payload,
    int* out_row_count
) {
    return banana_db_query_banana_profile(purchases, multiplier, out_payload, out_row_count);
}