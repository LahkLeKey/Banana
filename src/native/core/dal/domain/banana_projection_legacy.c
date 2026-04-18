#include "banana_projection_legacy.h"

#include "../internal/banana_postgres.h"

#include "../../domain/banana_lifecycle.h"
#include "../../domain/banana_projection_legacy.h"
#include "../../testing/native_test_hooks.h"

#include <stdio.h>
#include <stdlib.h>

static char* build_json_payload(int purchases, int multiplier, int banana) {
    BananaInput input;
    BananaRipenessPrediction prediction;
    BananaStatus status;
    const char* cultivar = banana_species_name(BANANA_SPECIES_CAVENDISH);
    const char* stage_name = "UNKNOWN";
    int required = 0;
    char* payload = 0;

    input.purchases = purchases;
    input.multiplier = multiplier;
    status = banana_predict_ripeness_for_legacy_input(&input, &prediction);
    if (status != BANANA_OK) {
        return 0;
    }

    stage_name = banana_ripeness_stage_name(prediction.predicted_stage);
    required = snprintf(0, 0,
        "{\"purchases\":%d,\"multiplier\":%d,\"banana\":%d,\"cultivar\":\"%s\",\"predicted_stage\":\"%s\",\"shelf_life_hours\":%d,\"spoilage_probability\":%.3f}",
        purchases,
        multiplier,
        banana,
        cultivar,
        stage_name,
        prediction.shelf_life_hours,
        prediction.spoilage_probability);

    if (required < 0) {
        return 0; /* GCOVR_EXCL_LINE */
    }

    if (banana_test_hook_force_payload_alloc_failure()) {
        return 0;
    }

    if (banana_test_hook_force_payload_malloc_null()) {
        payload = 0;
    } else {
        payload = (char*)malloc((size_t)required + 1U);
    }

    if (payload == 0) {
        return 0;
    }

    if (snprintf(payload, (size_t)required + 1U,
        "{\"purchases\":%d,\"multiplier\":%d,\"banana\":%d,\"cultivar\":\"%s\",\"predicted_stage\":\"%s\",\"shelf_life_hours\":%d,\"spoilage_probability\":%.3f}",
        purchases,
        multiplier,
        banana,
        cultivar,
        stage_name,
        prediction.shelf_life_hours,
        prediction.spoilage_probability) < 0) {
        free(payload); /* GCOVR_EXCL_LINE */
        return 0; /* GCOVR_EXCL_LINE */
    }

    return payload;
}

int banana_projection_db_query_banana_profile(int purchases, int multiplier, char** out_payload, int* out_row_count) {
    int forced_result = 0;

    if (out_payload == 0 || out_row_count == 0) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    if (banana_test_hook_try_forced_db_result(&forced_result)) {
        return forced_result;
    }

    *out_payload = 0;
    *out_row_count = 0;

#if defined(BANANA_ENABLE_POSTGRES)
    {
        int banana = 0;
        int status = banana_postgres_query_banana_profile(purchases, multiplier, &banana);

        if (status != BANANA_DB_OK) {
            return status;
        }

        *out_payload = build_json_payload(purchases, multiplier, banana);
        *out_row_count = 1;
        if (*out_payload == 0) {
            return BANANA_DB_ERROR;
        }

        return BANANA_DB_OK;
    }
#else
    {
        BananaInput input;
        BananaResult result;

        input.purchases = purchases;
        input.multiplier = multiplier;
        if (banana_calculate(&input, &result) != BANANA_OK) {
            return BANANA_DB_ERROR;
        }

        *out_payload = build_json_payload(purchases, multiplier, result.banana);
        *out_row_count = 1;
        if (*out_payload == 0) {
            return BANANA_DB_ERROR;
        }

        return BANANA_DB_OK;
    }
#endif
}