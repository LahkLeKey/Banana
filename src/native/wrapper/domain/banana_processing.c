#include "../banana_wrapper_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../core/domain/banana_lifecycle.h"
#include "../../core/domain/banana_processing.h"

#define BANANA_MAX_WRAPPER_HARVEST_BATCHES 64

static BananaHarvestBatch harvest_batch_registry[BANANA_MAX_WRAPPER_HARVEST_BATCHES];
static int harvest_batch_registry_count = 0;

static int find_harvest_batch_index(const char* harvest_batch_id) {
    int index = 0;

    if (!banana_identifier_is_valid(harvest_batch_id)) {
        return -1;
    }

    for (index = 0; index < harvest_batch_registry_count; index++) {
        if (strcmp(harvest_batch_registry[index].harvest_batch_id.value, harvest_batch_id) == 0) {
            return index;
        }
    }

    return -1;
}

static BananaStatus save_harvest_batch(const BananaHarvestBatch* harvest_batch) {
    int index = -1;

    if (harvest_batch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = find_harvest_batch_index(harvest_batch->harvest_batch_id.value);
    if (index >= 0) {
        harvest_batch_registry[index] = *harvest_batch;
        return BANANA_OK;
    }

    if (harvest_batch_registry_count >= BANANA_MAX_WRAPPER_HARVEST_BATCHES) {
        return BANANA_ERROR_OVERFLOW;
    }

    harvest_batch_registry[harvest_batch_registry_count] = *harvest_batch;
    harvest_batch_registry_count++;
    return BANANA_OK;
}

static BananaStatus load_harvest_batch(const char* harvest_batch_id, BananaHarvestBatch* harvest_batch) {
    int index = -1;

    if (harvest_batch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = find_harvest_batch_index(harvest_batch_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    *harvest_batch = harvest_batch_registry[index];
    return BANANA_OK;
}

static BananaStatus create_bunch_record_snapshot(
    const char* bunch_id,
    int harvest_day_ordinal,
    double bunch_weight_kg,
    BananaBunchRecord* bunch
) {
    BananaStatus status = BANANA_OK;

    if (bunch == 0 || harvest_day_ordinal < 0 || bunch_weight_kg <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(bunch, 0, sizeof(*bunch));
    status = banana_identifier_copy(&bunch->aggregate.bunch.bunch_id, bunch_id);
    if (status != BANANA_OK) {
        return status;
    }

    bunch->aggregate.bunch.harvest_day_ordinal = harvest_day_ordinal;
    bunch->aggregate.bunch.weight_kg = bunch_weight_kg;
    bunch->total_weight_kg = bunch_weight_kg;
    return BANANA_OK;
}

static char* create_harvest_batch_json(const BananaHarvestBatch* harvest_batch) {
    int required = 0;
    char* payload = 0;

    if (harvest_batch == 0) {
        return 0;
    }

    required = snprintf(
        0,
        0,
        "{\"harvestBatchId\":\"%s\",\"fieldId\":\"%s\",\"harvestDayOrdinal\":%d,\"bunchCount\":%d,\"totalWeightKg\":%.2f}",
        harvest_batch->harvest_batch_id.value,
        harvest_batch->field_id.value,
        harvest_batch->harvest_day_ordinal,
        harvest_batch->bunch_count,
        harvest_batch->total_weight_kg);
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
        "{\"harvestBatchId\":\"%s\",\"fieldId\":\"%s\",\"harvestDayOrdinal\":%d,\"bunchCount\":%d,\"totalWeightKg\":%.2f}",
        harvest_batch->harvest_batch_id.value,
        harvest_batch->field_id.value,
        harvest_batch->harvest_day_ordinal,
        harvest_batch->bunch_count,
        harvest_batch->total_weight_kg) < 0) {
        free(payload);
        return 0;
    }

    return payload;
}

int banana_create_harvest_batch(
    const char* harvest_batch_id,
    const char* field_id,
    int harvest_day_ordinal,
    char** out_harvest_batch_json
) {
    BananaHarvestBatch harvest_batch;
    BananaStatus status = BANANA_OK;

    if (out_harvest_batch_json == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_harvest_batch_json = 0;
    status = banana_harvest_batch_create(harvest_batch_id, field_id, harvest_day_ordinal, &harvest_batch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = save_harvest_batch(&harvest_batch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_harvest_batch_json = create_harvest_batch_json(&harvest_batch);
    if (*out_harvest_batch_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}

int banana_add_bunch_to_harvest_batch(
    const char* harvest_batch_id,
    const char* bunch_id,
    int harvest_day_ordinal,
    double bunch_weight_kg,
    char** out_harvest_batch_json
) {
    BananaHarvestBatch harvest_batch;
    BananaBunchRecord bunch;
    BananaStatus status = BANANA_OK;

    if (out_harvest_batch_json == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_harvest_batch_json = 0;
    status = load_harvest_batch(harvest_batch_id, &harvest_batch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = create_bunch_record_snapshot(bunch_id, harvest_day_ordinal, bunch_weight_kg, &bunch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = banana_harvest_batch_add_bunch(&harvest_batch, &bunch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = save_harvest_batch(&harvest_batch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_harvest_batch_json = create_harvest_batch_json(&harvest_batch);
    if (*out_harvest_batch_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}

int banana_get_harvest_batch_status(
    const char* harvest_batch_id,
    char** out_harvest_batch_json
) {
    BananaHarvestBatch harvest_batch;
    BananaStatus status = BANANA_OK;

    if (out_harvest_batch_json == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_harvest_batch_json = 0;
    status = load_harvest_batch(harvest_batch_id, &harvest_batch);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_harvest_batch_json = create_harvest_batch_json(&harvest_batch);
    if (*out_harvest_batch_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}