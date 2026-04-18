#include "banana_cultivation.h"

#include <string.h>

static int coordinates_are_valid(BananaGeoCoordinates location) {
    return location.latitude >= -90.0
        && location.latitude <= 90.0
        && location.longitude >= -180.0
        && location.longitude <= 180.0;
}

static int ripeness_stage_is_valid(BananaRipenessStage stage) {
    return stage >= BANANA_STAGE_GREEN && stage <= BANANA_STAGE_BIODEGRADATION;
}

static int plant_contains_sucker(const BananaPlant* plant, const char* sucker_id) {
    int index = 0;

    if (plant == 0 || sucker_id == 0) {
        return 0;
    }

    for (index = 0; index < plant->sucker_offspring_count; index++) {
        if (strcmp(plant->sucker_offspring[index].value, sucker_id) == 0) {
            return 1;
        }
    }

    return 0;
}

BananaStatus banana_plant_register(
    const char* plant_id,
    BananaSpecies species,
    BananaGeoCoordinates location,
    int planted_day_ordinal,
    int pseudostem_count,
    BananaPlant* plant,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (plant == 0
        || planted_day_ordinal < 0
        || pseudostem_count <= 0
        || !coordinates_are_valid(location)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(plant, 0, sizeof(*plant));

    status = banana_identifier_copy(&plant->plant_id, plant_id);
    if (status != BANANA_OK) {
        return status;
    }

    plant->species = species;
    plant->location = location;
    plant->planted_day_ordinal = planted_day_ordinal;
    plant->health_status = BANANA_HEALTH_GOOD;
    plant->pseudostem_count = pseudostem_count;
    plant->sucker_offspring_count = 0;

    return banana_domain_event_record(
        event,
        BANANA_EVENT_PLANTED,
        plant_id,
        0,
        BANANA_STAGE_GREEN,
        pseudostem_count,
        planted_day_ordinal,
        0.0);
}

BananaStatus banana_plant_add_sucker(
    BananaPlant* plant,
    const char* sucker_id
) {
    BananaStatus status = BANANA_OK;

    if (plant == 0 || plant->health_status == BANANA_HEALTH_DEAD) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (plant->sucker_offspring_count >= BANANA_MAX_SUCKER_OFFSPRING
        || plant_contains_sucker(plant, sucker_id)) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(
        &plant->sucker_offspring[plant->sucker_offspring_count],
        sucker_id);
    if (status != BANANA_OK) {
        return status;
    }

    plant->sucker_offspring_count++;
    return BANANA_OK;
}

BananaStatus banana_bunch_harvest(
    const BananaPlant* plant,
    const char* bunch_id,
    const BananaBunchSpec* spec,
    BananaBunchAggregate* bunch,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (plant == 0
        || spec == 0
        || bunch == 0
        || spec->harvest_day_ordinal < plant->planted_day_ordinal
        || spec->weight_kg <= 0.0
        || spec->finger_count <= 0
        || spec->quality_score < 0.0
        || spec->quality_score > 1.0
        || !ripeness_stage_is_valid(spec->maturity_stage)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(bunch, 0, sizeof(*bunch));

    status = banana_identifier_copy(&bunch->bunch.bunch_id, bunch_id);
    if (status != BANANA_OK) {
        return status;
    }

    bunch->bunch.plant_id = plant->plant_id;
    bunch->bunch.harvest_day_ordinal = spec->harvest_day_ordinal;
    bunch->bunch.weight_kg = spec->weight_kg;
    bunch->bunch.finger_count = spec->finger_count;
    bunch->bunch.maturity_stage = spec->maturity_stage;
    bunch->bunch.quality_score = spec->quality_score;
    bunch->lifecycle_state = BANANA_BUNCH_STATE_HARVESTED;

    return banana_domain_event_record(
        event,
        BANANA_EVENT_HARVESTED,
        bunch_id,
        plant->plant_id.value,
        spec->maturity_stage,
        spec->finger_count,
        spec->harvest_day_ordinal,
        spec->weight_kg);
}

BananaStatus banana_bunch_update_ripeness(
    BananaBunchAggregate* bunch,
    BananaRipenessStage new_stage,
    int event_day_ordinal,
    BananaDomainEvent* event
) {
    if (bunch == 0
        || event_day_ordinal < bunch->bunch.harvest_day_ordinal
        || !ripeness_stage_is_valid(new_stage)
        || bunch->lifecycle_state == BANANA_BUNCH_STATE_SOLD
        || bunch->lifecycle_state == BANANA_BUNCH_STATE_SPOILED
        || new_stage < bunch->bunch.maturity_stage) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    bunch->bunch.maturity_stage = new_stage;
    bunch->lifecycle_state = (new_stage >= BANANA_STAGE_BREAKING)
        ? BANANA_BUNCH_STATE_RIPENING
        : BANANA_BUNCH_STATE_HARVESTED;

    return banana_domain_event_record(
        event,
        BANANA_EVENT_RIPENED,
        bunch->bunch.bunch_id.value,
        bunch->bunch.plant_id.value,
        new_stage,
        bunch->bunch.finger_count,
        event_day_ordinal,
        bunch->bunch.weight_kg);
}

BananaStatus banana_bunch_mark_spoiled(
    BananaBunchAggregate* bunch,
    int event_day_ordinal,
    BananaDomainEvent* event
) {
    if (bunch == 0 || event_day_ordinal < bunch->bunch.harvest_day_ordinal) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    bunch->bunch.maturity_stage = BANANA_STAGE_BIODEGRADATION;
    bunch->lifecycle_state = BANANA_BUNCH_STATE_SPOILED;

    return banana_domain_event_record(
        event,
        BANANA_EVENT_SPOILED,
        bunch->bunch.bunch_id.value,
        bunch->bunch.plant_id.value,
        bunch->bunch.maturity_stage,
        bunch->bunch.finger_count,
        event_day_ordinal,
        bunch->bunch.weight_kg);
}

int banana_bunch_is_market_ready(const BananaBunchAggregate* bunch) {
    if (bunch == 0
        || bunch->lifecycle_state == BANANA_BUNCH_STATE_SOLD
        || bunch->lifecycle_state == BANANA_BUNCH_STATE_SPOILED) {
        return 0;
    }

    return (bunch->bunch.maturity_stage == BANANA_STAGE_YELLOW
            || bunch->bunch.maturity_stage == BANANA_STAGE_SPOTTED)
        && bunch->bunch.quality_score >= 0.7;
}