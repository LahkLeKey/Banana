#include "banana_processing.h"

static double absolute_difference(double left, double right) {
    return (left > right) ? (left - right) : (right - left);
}

static int ripeness_distance(BananaRipenessStage left, BananaRipenessStage right) {
    return (left > right) ? (left - right) : (right - left);
}

static int ripeness_stage_is_valid(BananaRipenessStage stage) {
    return stage >= BANANA_STAGE_GREEN && stage <= BANANA_STAGE_BIODEGRADATION;
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