#include "banana_bms.h"

#include <string.h>

static BananaBatch batch_registry[BANANA_BMS_MAX_BATCH_REGISTRY];
static int batch_registry_count = 0;

static int identifier_length_valid(const char* value) {
    size_t length = 0U;

    if (value == 0 || value[0] == '\0') {
        return 0;
    }

    length = strlen(value);
    return length > 0U && length < BANANA_BMS_ID_CAPACITY;
}

static BananaStatus copy_identifier(BananaIdentifier* identifier, const char* value) {
    if (identifier == 0 || !identifier_length_valid(value)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(identifier->value, 0, sizeof(identifier->value));
    memcpy(identifier->value, value, strlen(value));
    return BANANA_OK;
}

static int find_batch_index(const char* batch_id) {
    int index = 0;

    if (!identifier_length_valid(batch_id)) {
        return -1;
    }

    for (index = 0; index < batch_registry_count; index++) {
        if (strcmp(batch_registry[index].batch_id.value, batch_id) == 0) {
            return index;
        }
    }

    return -1;
}

BananaStatus banana_batch_register(
    const char* batch_id,
    const char* origin_farm,
    double storage_temp_c,
    double ethylene_exposure,
    int estimated_shelf_life_days,
    BananaBatch* batch
) {
    BananaBatch candidate;
    BananaStatus status = BANANA_OK;
    int index = -1;

    if (batch == 0
        || storage_temp_c < 0.0
        || storage_temp_c > 60.0
        || ethylene_exposure < 0.0
        || estimated_shelf_life_days < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(&candidate, 0, sizeof(candidate));

    status = copy_identifier(&candidate.batch_id, batch_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = copy_identifier(&candidate.origin_farm, origin_farm);
    if (status != BANANA_OK) {
        return status;
    }

    candidate.bunch_count = 0;
    candidate.export_status = BANANA_EXPORT_PACKED;
    candidate.storage_temp_c = storage_temp_c;
    candidate.ethylene_exposure = ethylene_exposure;
    candidate.estimated_shelf_life_days = estimated_shelf_life_days;

    index = find_batch_index(batch_id);
    if (index >= 0) {
        batch_registry[index] = candidate;
        *batch = candidate;
        return BANANA_OK;
    }

    if (batch_registry_count >= BANANA_BMS_MAX_BATCH_REGISTRY) {
        return BANANA_ERROR_OVERFLOW;
    }

    batch_registry[batch_registry_count] = candidate;
    *batch = candidate;
    batch_registry_count++;
    return BANANA_OK;
}

BananaStatus banana_batch_get(
    const char* batch_id,
    BananaBatch* batch
) {
    int index = -1;

    if (batch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = find_batch_index(batch_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    *batch = batch_registry[index];
    return BANANA_OK;
}

BananaStatus banana_batch_predict_ripeness(
    const char* batch_id,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double mechanical_damage,
    BananaRipenessPrediction* prediction
) {
    BananaBatch batch;
    BananaRipenessInput input;
    BananaStatus status = BANANA_OK;

    if (prediction == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_batch_get(batch_id, &batch);
    if (status != BANANA_OK) {
        return status;
    }

    input.temperature_history_c = temperature_history_c;
    input.temperature_history_count = temperature_history_count;
    input.days_since_harvest = days_since_harvest;
    input.ethylene_exposure = batch.ethylene_exposure;
    input.mechanical_damage = mechanical_damage;
    input.storage_temp_c = batch.storage_temp_c;

    status = banana_predict_ripeness(&input, prediction);
    if (status != BANANA_OK) {
        return status;
    }

    if (batch.estimated_shelf_life_days > 0) {
        int batch_limit_hours = batch.estimated_shelf_life_days * 24;
        if (prediction->shelf_life_hours > batch_limit_hours) {
            prediction->shelf_life_hours = batch_limit_hours;
        }
    }

    return BANANA_OK;
}