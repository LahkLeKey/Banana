#include "banana_agriculture.h"

#include <string.h>

static int coordinates_are_valid(BananaGeoCoordinates location) {
    return location.latitude >= -90.0
        && location.latitude <= 90.0
        && location.longitude >= -180.0
        && location.longitude <= 180.0;
}

static BananaStatus copy_label(char* destination, int capacity, const char* value) {
    size_t length = 0U;

    if (destination == 0 || value == 0 || value[0] == '\0') {
        return BANANA_ERROR_INVALID_INPUT;
    }

    length = strlen(value);
    if ((int)length >= capacity) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(destination, 0, (size_t)capacity);
    memcpy(destination, value, length);
    return BANANA_OK;
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

BananaStatus banana_soil_conditions_validate(const BananaSoilConditions* soil_conditions) {
    if (soil_conditions == 0
        || soil_conditions->moisture_pct < 0.0
        || soil_conditions->moisture_pct > 100.0
        || soil_conditions->ph < 0.0
        || soil_conditions->ph > 14.0
        || soil_conditions->organic_matter_pct < 0.0
        || soil_conditions->organic_matter_pct > 100.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    return BANANA_OK;
}

BananaStatus banana_farm_register(
    const char* farm_id,
    const char* farm_name,
    BananaFarm* farm
) {
    BananaStatus status = BANANA_OK;

    if (farm == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(farm, 0, sizeof(*farm));
    status = banana_identifier_copy(&farm->farm_id, farm_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = copy_label(farm->farm_name, BANANA_ID_CAPACITY, farm_name);
    if (status != BANANA_OK) {
        return status;
    }

    return BANANA_OK;
}

BananaStatus banana_field_register(
    const char* field_id,
    const char* farm_id,
    BananaGeoCoordinates location,
    BananaSoilConditions soil_conditions,
    double area_hectares,
    BananaField* field
) {
    BananaStatus status = BANANA_OK;

    if (field == 0 || area_hectares <= 0.0 || !coordinates_are_valid(location)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_soil_conditions_validate(&soil_conditions);
    if (status != BANANA_OK) {
        return status;
    }

    memset(field, 0, sizeof(*field));
    status = banana_identifier_copy(&field->field_id, field_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&field->farm_id, farm_id);
    if (status != BANANA_OK) {
        return status;
    }

    field->location = location;
    field->soil_conditions = soil_conditions;
    field->area_hectares = area_hectares;
    field->active_plant_count = 0;
    return BANANA_OK;
}

BananaStatus banana_farm_add_field(
    BananaFarm* farm,
    const BananaField* field
) {
    BananaStatus status = BANANA_OK;

    if (farm == 0 || field == 0 || strcmp(farm->farm_id.value, field->farm_id.value) != 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (farm->field_count >= BANANA_MAX_FIELDS_PER_FARM
        || identifier_index(farm->field_ids, farm->field_count, field->field_id.value) >= 0) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(&farm->field_ids[farm->field_count], field->field_id.value);
    if (status != BANANA_OK) {
        return status;
    }

    farm->field_count++;
    return BANANA_OK;
}

BananaStatus banana_seedling_register(
    const char* seedling_id,
    BananaSpecies cultivar,
    int nursery_day_ordinal,
    BananaSeedling* seedling
) {
    BananaStatus status = BANANA_OK;

    if (seedling == 0 || nursery_day_ordinal < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(seedling, 0, sizeof(*seedling));
    status = banana_identifier_copy(&seedling->seedling_id, seedling_id);
    if (status != BANANA_OK) {
        return status;
    }

    seedling->cultivar = cultivar;
    seedling->nursery_day_ordinal = nursery_day_ordinal;
    seedling->transplant_day_ordinal = -1;
    return BANANA_OK;
}

BananaStatus banana_seedling_transplant(
    BananaSeedling* seedling,
    BananaField* field,
    const char* plant_id,
    int transplant_day_ordinal,
    int pseudostem_count,
    BananaPlant* plant,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (seedling == 0
        || field == 0
        || transplant_day_ordinal < seedling->nursery_day_ordinal) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_plant_register(
        plant_id,
        seedling->cultivar,
        field->location,
        transplant_day_ordinal,
        pseudostem_count,
        plant,
        event);
    if (status != BANANA_OK) {
        return status;
    }

    seedling->transplant_day_ordinal = transplant_day_ordinal;
    field->active_plant_count++;
    return BANANA_OK;
}

BananaStatus banana_field_plan_planting(
    const BananaField* field,
    int* recommended_plant_count
) {
    double soil_multiplier = 1.0;
    int recommended = 0;

    if (field == 0 || recommended_plant_count == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (field->soil_conditions.organic_matter_pct >= 4.0) {
        soil_multiplier += 0.1;
    }

    if (field->soil_conditions.moisture_pct < 30.0) {
        soil_multiplier -= 0.2;
    }

    recommended = (int)(field->area_hectares * 1200.0 * soil_multiplier);
    if (recommended < 0) {
        recommended = 0;
    }

    *recommended_plant_count = recommended;
    return BANANA_OK;
}

BananaStatus banana_field_plan_harvest(
    const BananaField* field,
    int active_bunch_count,
    int* recommended_pick_count
) {
    int recommended = 0;

    if (field == 0 || recommended_pick_count == 0 || active_bunch_count < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    recommended = (field->soil_conditions.moisture_pct >= 35.0 && field->soil_conditions.ph >= 5.2 && field->soil_conditions.ph <= 7.0)
        ? (active_bunch_count * 3) / 4
        : active_bunch_count / 2;

    if (recommended <= 0 && active_bunch_count > 0) {
        recommended = 1;
    }

    *recommended_pick_count = recommended;
    return BANANA_OK;
}