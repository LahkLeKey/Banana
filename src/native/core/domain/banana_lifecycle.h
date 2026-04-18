#ifndef BANANA_LIFECYCLE_H
#define BANANA_LIFECYCLE_H

#include "banana_domain_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_ID_CAPACITY 37
#define BANANA_MAX_SUCKER_OFFSPRING 8

typedef enum BananaSpecies {
    BANANA_SPECIES_CAVENDISH = 0,
    BANANA_SPECIES_PLANTAIN = 1,
    BANANA_SPECIES_OTHER = 2
} BananaSpecies;

typedef enum BananaPlantHealthStatus {
    BANANA_HEALTH_GOOD = 0,
    BANANA_HEALTH_STRESSED = 1,
    BANANA_HEALTH_INFECTED = 2,
    BANANA_HEALTH_DEAD = 3
} BananaPlantHealthStatus;

typedef enum BananaRipenessStage {
    BANANA_STAGE_GREEN = 0,
    BANANA_STAGE_BREAKING = 1,
    BANANA_STAGE_YELLOW = 2,
    BANANA_STAGE_SPOTTED = 3,
    BANANA_STAGE_OVERRIPE = 4,
    BANANA_STAGE_BIODEGRADATION = 5
} BananaRipenessStage;

typedef struct BananaIdentifier {
    char value[BANANA_ID_CAPACITY];
} BananaIdentifier;

typedef struct BananaGeoCoordinates {
    double latitude;
    double longitude;
} BananaGeoCoordinates;

typedef struct BananaPlant {
    BananaIdentifier plant_id;
    BananaSpecies species;
    BananaGeoCoordinates location;
    int planted_day_ordinal;
    BananaPlantHealthStatus health_status;
    int pseudostem_count;
    int sucker_offspring_count;
    BananaIdentifier sucker_offspring[BANANA_MAX_SUCKER_OFFSPRING];
} BananaPlant;

typedef struct BananaBunch {
    BananaIdentifier bunch_id;
    BananaIdentifier plant_id;
    int harvest_day_ordinal;
    double weight_kg;
    int finger_count;
    BananaRipenessStage maturity_stage;
    double quality_score;
} BananaBunch;

typedef struct BananaRipenessInput {
    const double* temperature_history_c;
    int temperature_history_count;
    int days_since_harvest;
    double ethylene_exposure;
    double mechanical_damage;
    double storage_temp_c;
} BananaRipenessInput;

typedef struct BananaRipenessPrediction {
    BananaRipenessStage predicted_stage;
    int shelf_life_hours;
    double ripening_index;
    double spoilage_probability;
    double cold_chain_risk;
} BananaRipenessPrediction;

int banana_identifier_is_valid(const char* value);

BananaStatus banana_identifier_copy(BananaIdentifier* identifier, const char* value);

const char* banana_species_name(BananaSpecies species);

const char* banana_ripeness_stage_name(BananaRipenessStage stage);

BananaStatus banana_predict_ripeness(
    const BananaRipenessInput* input,
    BananaRipenessPrediction* prediction
);

#ifdef __cplusplus
}
#endif

#endif