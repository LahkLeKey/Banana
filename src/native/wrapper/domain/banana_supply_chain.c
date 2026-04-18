#include "../banana_wrapper_internal.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../core/domain/banana_supply_chain.h"

static char* create_batch_json(const BananaBatch* batch) {
    int required = 0;
    char* payload = 0;
    const char* export_status = "PACKED";

    if (batch == 0) {
        return 0;
    }

    switch (batch->export_status) {
        case BANANA_EXPORT_PACKED:
            export_status = "PACKED";
            break;
        case BANANA_EXPORT_SHIPPED:
            export_status = "SHIPPED";
            break;
        case BANANA_EXPORT_CUSTOMS:
            export_status = "CUSTOMS";
            break;
        case BANANA_EXPORT_DISTRIBUTED:
            export_status = "DISTRIBUTED";
            break;
    }

    required = snprintf(
        0,
        0,
        "{\"batchId\":\"%s\",\"originFarm\":\"%s\",\"exportStatus\":\"%s\",\"storageTempC\":%.2f,\"ethyleneExposure\":%.2f,\"estimatedShelfLifeDays\":%d}",
        batch->batch_id.value,
        batch->origin_farm.value,
        export_status,
        batch->storage_temp_c,
        batch->ethylene_exposure,
        batch->estimated_shelf_life_days);
    if (required < 0) {
        return 0;
    }

    payload = (char*)malloc((size_t)required + 1U);
    if (payload == 0) {
        return 0;
    }

    if (snprintf(
        payload,
        (size_t)required + 1U,
        "{\"batchId\":\"%s\",\"originFarm\":\"%s\",\"exportStatus\":\"%s\",\"storageTempC\":%.2f,\"ethyleneExposure\":%.2f,\"estimatedShelfLifeDays\":%d}",
        batch->batch_id.value,
        batch->origin_farm.value,
        export_status,
        batch->storage_temp_c,
        batch->ethylene_exposure,
        batch->estimated_shelf_life_days) < 0) {
        free(payload);
        return 0;
    }

    return payload;
}

int banana_create_batch(
    const char* batch_id,
    const char* origin_farm,
    double storage_temp_c,
    double ethylene_exposure,
    int estimated_shelf_life_days,
    char** out_batch_json
) {
    BananaBatch batch;
    BananaStatus status = BANANA_OK;

    if (out_batch_json == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_batch_json = 0;
    status = banana_batch_register(
        batch_id,
        origin_farm,
        storage_temp_c,
        ethylene_exposure,
        estimated_shelf_life_days,
        &batch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_batch_json = create_batch_json(&batch);
    if (*out_batch_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}

int banana_get_batch_status(
    const char* batch_id,
    char** out_batch_json
) {
    BananaBatch batch;
    BananaStatus status = BANANA_OK;

    if (out_batch_json == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_batch_json = 0;
    status = banana_batch_get(batch_id, &batch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_batch_json = create_batch_json(&batch);
    if (*out_batch_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}

int banana_predict_batch_ripeness(
    const char* batch_id,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double mechanical_damage,
    CInteropBananaRipenessPrediction* out_prediction
) {
    BananaRipenessPrediction prediction;
    BananaStatus status = BANANA_OK;

    if (out_prediction == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    status = banana_batch_predict_ripeness(
        batch_id,
        temperature_history_c,
        temperature_history_count,
        days_since_harvest,
        mechanical_damage,
        &prediction);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    banana_wrapper_copy_ripeness_prediction(&prediction, out_prediction);
    return BANANA_STATUS_OK;
}