#include "banana_processing.h"

#include <string.h>

static double absolute_difference(double left, double right) {
    return (left > right) ? (left - right) : (right - left);
}

static int ripeness_distance(BananaRipenessStage left, BananaRipenessStage right) {
    return (left > right) ? (left - right) : (right - left);
}

static int ripeness_stage_is_valid(BananaRipenessStage stage) {
    return stage >= BANANA_STAGE_GREEN && stage <= BANANA_STAGE_BIODEGRADATION;
}

static int identifier_index(const BananaIdentifier* identifiers, int count, const char* value) {
    int index = 0;

    if (identifiers == 0 || value == 0) {
        return -1;
    }

    for (index = 0; index < count; index++) {
        if (strcmp(identifiers[index].value, value) == 0) {
            return index;
        }
    }

    return -1;
}

BananaStatus banana_dimensions_validate(const BananaDimensions* dimensions) {
    if (dimensions == 0
        || dimensions->length_cm <= 0.0
        || dimensions->girth_cm <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    return BANANA_OK;
}

BananaStatus banana_fruit_estimate_dimensions(
    const BananaFruit* fruit,
    BananaDimensions* dimensions
) {
    if (fruit == 0 || dimensions == 0 || fruit->weight_kg <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    dimensions->length_cm = 12.0 + (fruit->weight_kg * 18.0);
    dimensions->girth_cm = 7.0 + (fruit->weight_kg * 7.0);
    return banana_dimensions_validate(dimensions);
}

BananaStatus banana_bunch_factory_create(
    const BananaPlant* plant,
    const char* bunch_id,
    const BananaBunchSpec* spec,
    BananaSpecies cultivar,
    const BananaFruitSpec* fruit_specs,
    int fruit_count,
    BananaBunchRecord* record,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;
    double total_weight_kg = 0.0;
    int index = 0;

    if (fruit_specs == 0
        || record == 0
        || fruit_count <= 0
        || fruit_count > BANANA_MAX_FRUIT_PER_BUNCH) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_bunch_harvest(plant, bunch_id, spec, &record->aggregate, event);
    if (status != BANANA_OK) {
        return status;
    }

    record->cultivar = cultivar;
    record->banana_count = fruit_count;
    record->total_weight_kg = 0.0;

    for (index = 0; index < fruit_count; index++) {
        if (fruit_specs[index].fruit_id == 0
            || fruit_specs[index].cultivar != cultivar
            || fruit_specs[index].weight_kg <= 0.0
            || !ripeness_stage_is_valid(fruit_specs[index].ripeness_stage)
            || ripeness_distance(fruit_specs[index].ripeness_stage, spec->maturity_stage) > 1) {
            return BANANA_ERROR_INVALID_INPUT;
        }

        status = banana_identifier_copy(&record->bananas[index].fruit_id, fruit_specs[index].fruit_id);
        if (status != BANANA_OK) {
            return status;
        }

        record->bananas[index].cultivar = fruit_specs[index].cultivar;
        record->bananas[index].ripeness_stage = fruit_specs[index].ripeness_stage;
        record->bananas[index].weight_kg = fruit_specs[index].weight_kg;
        total_weight_kg += fruit_specs[index].weight_kg;
    }

    record->total_weight_kg = total_weight_kg;
    if (spec->finger_count != fruit_count || absolute_difference(total_weight_kg, spec->weight_kg) > 0.05) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    record->aggregate.bunch.finger_count = fruit_count;
    record->aggregate.bunch.weight_kg = total_weight_kg;
    return banana_bunch_record_validate(record);
}

BananaStatus banana_bunch_record_validate(const BananaBunchRecord* record) {
    double total_weight_kg = 0.0;
    int index = 0;

    if (record == 0 || record->banana_count <= 0 || record->banana_count > BANANA_MAX_FRUIT_PER_BUNCH) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (record->aggregate.bunch.finger_count != record->banana_count) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (index = 0; index < record->banana_count; index++) {
        if (record->bananas[index].fruit_id.value[0] == '\0'
            || record->bananas[index].cultivar != record->cultivar
            || record->bananas[index].weight_kg <= 0.0) {
            return BANANA_ERROR_INVALID_INPUT;
        }

        total_weight_kg += record->bananas[index].weight_kg;
    }

    if (!banana_bunch_record_all_same_cultivar(record)
        || !banana_bunch_record_uniform_ripeness(record)
        || absolute_difference(total_weight_kg, record->total_weight_kg) > 0.05
        || absolute_difference(total_weight_kg, record->aggregate.bunch.weight_kg) > 0.05) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    return BANANA_OK;
}

BananaStatus banana_bunch_record_update_ripeness(
    BananaBunchRecord* record,
    BananaRipenessStage new_stage,
    int event_day_ordinal,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;
    int index = 0;

    if (record == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_bunch_update_ripeness(&record->aggregate, new_stage, event_day_ordinal, event);
    if (status != BANANA_OK) {
        return status;
    }

    for (index = 0; index < record->banana_count; index++) {
        record->bananas[index].ripeness_stage = new_stage;
    }

    return BANANA_OK;
}

BananaStatus banana_crate_register(
    const char* crate_id,
    double capacity_kg,
    BananaCrate* crate
) {
    BananaStatus status = BANANA_OK;

    if (crate == 0 || capacity_kg <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(crate, 0, sizeof(*crate));
    status = banana_identifier_copy(&crate->crate_id, crate_id);
    if (status != BANANA_OK) {
        return status;
    }

    crate->capacity_kg = capacity_kg;
    crate->current_weight_kg = 0.0;
    crate->bunch_count = 0;
    return BANANA_OK;
}

BananaStatus banana_crate_pack_bunch(
    BananaCrate* crate,
    const BananaBunchRecord* bunch
) {
    BananaStatus status = BANANA_OK;

    if (crate == 0 || bunch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (crate->bunch_count >= BANANA_MAX_BUNCHES_PER_CRATE
        || identifier_index(crate->bunch_ids, crate->bunch_count, bunch->aggregate.bunch.bunch_id.value) >= 0
        || crate->current_weight_kg + bunch->total_weight_kg > crate->capacity_kg) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(&crate->bunch_ids[crate->bunch_count], bunch->aggregate.bunch.bunch_id.value);
    if (status != BANANA_OK) {
        return status;
    }

    crate->bunch_count++;
    crate->current_weight_kg += bunch->total_weight_kg;
    return BANANA_OK;
}

BananaStatus banana_inspection_record(
    const char* inspector_id,
    const BananaBunchRecord* bunch,
    double minimum_quality_score,
    int defect_count,
    BananaInspection* inspection
) {
    BananaStatus status = BANANA_OK;

    if (inspection == 0 || bunch == 0 || minimum_quality_score < 0.0 || minimum_quality_score > 1.0 || defect_count < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(inspection, 0, sizeof(*inspection));
    status = banana_identifier_copy(&inspection->inspector_id, inspector_id);
    if (status != BANANA_OK) {
        return status;
    }

    inspection->bunch_id = bunch->aggregate.bunch.bunch_id;
    inspection->quality_score = bunch->aggregate.bunch.quality_score;
    inspection->defect_count = defect_count;
    inspection->accepted = bunch->aggregate.bunch.quality_score >= minimum_quality_score && defect_count == 0;
    return BANANA_OK;
}

double banana_bunch_record_total_weight_kg(const BananaBunchRecord* record) {
    if (record == 0) {
        return 0.0;
    }

    return record->total_weight_kg;
}

int banana_bunch_record_all_same_cultivar(const BananaBunchRecord* record) {
    int index = 0;

    if (record == 0 || record->banana_count <= 0) {
        return 0;
    }

    for (index = 0; index < record->banana_count; index++) {
        if (record->bananas[index].cultivar != record->cultivar) {
            return 0;
        }
    }

    return 1;
}

int banana_bunch_record_uniform_ripeness(const BananaBunchRecord* record) {
    BananaRipenessStage min_stage;
    BananaRipenessStage max_stage;
    int index = 0;

    if (record == 0 || record->banana_count <= 0) {
        return 0;
    }

    min_stage = record->bananas[0].ripeness_stage;
    max_stage = record->bananas[0].ripeness_stage;
    for (index = 1; index < record->banana_count; index++) {
        if (record->bananas[index].ripeness_stage < min_stage) {
            min_stage = record->bananas[index].ripeness_stage;
        }

        if (record->bananas[index].ripeness_stage > max_stage) {
            max_stage = record->bananas[index].ripeness_stage;
        }
    }

    return ripeness_distance(min_stage, max_stage) <= 1;
}