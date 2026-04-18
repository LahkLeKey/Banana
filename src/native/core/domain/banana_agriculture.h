#ifndef BANANA_AGRICULTURE_H
#define BANANA_AGRICULTURE_H

#include "banana_cultivation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_MAX_FIELDS_PER_FARM 16

typedef struct BananaSoilConditions {
    double moisture_pct;
    double ph;
    double organic_matter_pct;
} BananaSoilConditions;

typedef struct BananaFarm {
    BananaIdentifier farm_id;
    char farm_name[BANANA_ID_CAPACITY];
    int field_count;
    BananaIdentifier field_ids[BANANA_MAX_FIELDS_PER_FARM];
} BananaFarm;

typedef struct BananaField {
    BananaIdentifier field_id;
    BananaIdentifier farm_id;
    BananaGeoCoordinates location;
    BananaSoilConditions soil_conditions;
    double area_hectares;
    int active_plant_count;
} BananaField;

typedef struct BananaSeedling {
    BananaIdentifier seedling_id;
    BananaSpecies cultivar;
    int nursery_day_ordinal;
    int transplant_day_ordinal;
} BananaSeedling;

BananaStatus banana_soil_conditions_validate(const BananaSoilConditions* soil_conditions);

BananaStatus banana_farm_register(
    const char* farm_id,
    const char* farm_name,
    BananaFarm* farm
);

BananaStatus banana_field_register(
    const char* field_id,
    const char* farm_id,
    BananaGeoCoordinates location,
    BananaSoilConditions soil_conditions,
    double area_hectares,
    BananaField* field
);

BananaStatus banana_farm_add_field(
    BananaFarm* farm,
    const BananaField* field
);

BananaStatus banana_seedling_register(
    const char* seedling_id,
    BananaSpecies cultivar,
    int nursery_day_ordinal,
    BananaSeedling* seedling
);

BananaStatus banana_seedling_transplant(
    BananaSeedling* seedling,
    BananaField* field,
    const char* plant_id,
    int transplant_day_ordinal,
    int pseudostem_count,
    BananaPlant* plant,
    BananaDomainEvent* event
);

BananaStatus banana_field_plan_planting(
    const BananaField* field,
    int* recommended_plant_count
);

BananaStatus banana_field_plan_harvest(
    const BananaField* field,
    int active_bunch_count,
    int* recommended_pick_count
);

#ifdef __cplusplus
}
#endif

#endif