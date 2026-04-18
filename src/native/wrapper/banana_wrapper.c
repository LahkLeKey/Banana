#include "banana_wrapper.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "../core/banana_bms.h"
#include "../dal/banana_db.h"
#include "../testing/native_test_hooks.h"

static int map_calculation_status(BananaStatus status) {
    if (status == BANANA_OK) {
        return BANANA_STATUS_OK;
    }

    if (status == BANANA_ERROR_INVALID_INPUT) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    if (status == BANANA_ERROR_OVERFLOW) {
        return BANANA_STATUS_OVERFLOW;
    }

    if (status == BANANA_ERROR_NOT_FOUND) {
        return BANANA_STATUS_NOT_FOUND;
    }

    return BANANA_STATUS_INTERNAL_ERROR;
}

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

static int try_calculate_legacy_projection_context(
    int purchases,
    int multiplier,
    BananaExecutionContext* context
) {
    BananaInput input;
    BananaRules rules = banana_default_rules();
    BananaStatus status = BANANA_OK;

    if (context == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    input.purchases = purchases;
    input.multiplier = multiplier;

    status = banana_prepare_execution_context(&input, &rules, context);
    if (status != BANANA_OK) {
        return map_calculation_status(status);
    }

    status = banana_calculate_context(context);
    return map_calculation_status(status);
}

int banana_calculate_banana(int purchases, int multiplier, int* out_banana) {
    BananaExecutionContext context;
    int status = 0;

    if (out_banana == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    status = try_calculate_legacy_projection_context(purchases, multiplier, &context);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    *out_banana = context.result.banana;
    return BANANA_STATUS_OK;
}

int banana_calculate_banana_with_breakdown(
    int purchases,
    int multiplier,
    CInteropBananaBreakdown* out_breakdown
) {
    BananaExecutionContext context;
    int status = 0;

    if (out_breakdown == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    status = try_calculate_legacy_projection_context(purchases, multiplier, &context);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    if (banana_test_hook_force_summary_failure()) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    out_breakdown->purchases = context.input.purchases;
    out_breakdown->multiplier = context.input.multiplier;
    out_breakdown->banana = context.result.banana;

    return BANANA_STATUS_OK;
}

int banana_create_banana_message(int purchases, int multiplier, char** out_message) {
    int banana = 0;
    int status = BANANA_STATUS_OK;
    char* message = 0;
    int required_bytes = 0;
    BananaInput input;
    BananaRipenessPrediction prediction;
    const char* cultivar = banana_species_name(BANANA_SPECIES_CAVENDISH);
    const char* stage_name = "UNKNOWN";

    if (out_message == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_message = 0;

    status = banana_calculate_banana(purchases, multiplier, &banana);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    input.purchases = purchases;
    input.multiplier = multiplier;
    if (banana_predict_ripeness_for_legacy_input(&input, &prediction) != BANANA_OK) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    stage_name = banana_ripeness_stage_name(prediction.predicted_stage);

    required_bytes = snprintf(
        0,
        0,
        "banana_profile cultivar=%s harvest_day=%d ethylene_index=%d stage=%s shelf_life_hours=%d spoilage_probability=%.2f banana=%d",
        cultivar,
        purchases,
        multiplier,
        stage_name,
        prediction.shelf_life_hours,
        prediction.spoilage_probability,
        banana);
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

    if (snprintf(
        message,
        (size_t)required_bytes + 1U,
        "banana_profile cultivar=%s harvest_day=%d ethylene_index=%d stage=%s shelf_life_hours=%d spoilage_probability=%.2f banana=%d",
        cultivar,
        purchases,
        multiplier,
        stage_name,
        prediction.shelf_life_hours,
        prediction.spoilage_probability,
        banana) < 0) {
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
    BananaInput input;
    BananaRules rules = banana_default_rules();
    int status = 0;
    int db_result = 0;

    input.purchases = purchases;
    input.multiplier = multiplier;

    status = map_calculation_status(banana_validate_input(&input, &rules));

    if (status != BANANA_STATUS_OK) {
        return status;
    }

    if (out_payload == 0 || out_row_count == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_payload = 0;
    *out_row_count = 0;

    db_result = banana_db_query(purchases, multiplier, out_payload, out_row_count);
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

int banana_predict_banana_ripeness(
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double ethylene_exposure,
    double mechanical_damage,
    double storage_temp_c,
    CInteropBananaRipenessPrediction* out_prediction
) {
    BananaRipenessInput input;
    BananaRipenessPrediction prediction;
    BananaStatus status = BANANA_OK;

    if (out_prediction == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    input.temperature_history_c = temperature_history_c;
    input.temperature_history_count = temperature_history_count;
    input.days_since_harvest = days_since_harvest;
    input.ethylene_exposure = ethylene_exposure;
    input.mechanical_damage = mechanical_damage;
    input.storage_temp_c = storage_temp_c;

    status = banana_predict_ripeness(&input, &prediction);
    if (status != BANANA_OK) {
        return map_calculation_status(status);
    }

    out_prediction->predicted_stage = (int)prediction.predicted_stage;
    out_prediction->shelf_life_hours = prediction.shelf_life_hours;
    out_prediction->ripening_index = prediction.ripening_index;
    out_prediction->spoilage_probability = prediction.spoilage_probability;
    out_prediction->cold_chain_risk = prediction.cold_chain_risk;

    return BANANA_STATUS_OK;
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
        return map_calculation_status(status);
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
        return map_calculation_status(status);
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
        return map_calculation_status(status);
    }

    out_prediction->predicted_stage = (int)prediction.predicted_stage;
    out_prediction->shelf_life_hours = prediction.shelf_life_hours;
    out_prediction->ripening_index = prediction.ripening_index;
    out_prediction->spoilage_probability = prediction.spoilage_probability;
    out_prediction->cold_chain_risk = prediction.cold_chain_risk;

    return BANANA_STATUS_OK;
}

void banana_free(void* pointer) {
    if (pointer != 0) {
        free(pointer);
    }
}
