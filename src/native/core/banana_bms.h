#ifndef BANANA_BMS_H
#define BANANA_BMS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BananaStatus {
    BANANA_OK = 0,
    BANANA_ERROR_INVALID_INPUT = 1,
    BANANA_ERROR_OVERFLOW = 2,
    BANANA_ERROR_NOT_FOUND = 3
} BananaStatus;

typedef struct BananaInput {
    int purchases;
    int multiplier;
} BananaInput;

typedef struct BananaResult {
    int banana;
} BananaResult;

typedef struct BananaCalculationBreakdown {
    int base_banana;
    int bonus_banana;
} BananaCalculationBreakdown;

typedef struct BananaRules {
    int base_rate;
    int bonus_threshold;
    int bonus_rate;
} BananaRules;

typedef void (*BananaStep)(BananaResult* result);

typedef struct BananaPipeline {
    BananaStep* steps;
    int count;
} BananaPipeline;

typedef struct BananaExecutionContext {
    BananaInput input;
    BananaRules rules;
    BananaResult result;
    BananaCalculationBreakdown breakdown;
    BananaStatus status;
} BananaExecutionContext;

#define BANANA_BMS_ID_CAPACITY 37
#define BANANA_BMS_MAX_SUCKER_OFFSPRING 8
#define BANANA_BMS_MAX_BUNCHES_PER_BATCH 16
#define BANANA_BMS_MAX_BATCHES_PER_NODE 32
#define BANANA_BMS_MAX_BATCH_REGISTRY 64

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

typedef enum BananaExportStatus {
    BANANA_EXPORT_PACKED = 0,
    BANANA_EXPORT_SHIPPED = 1,
    BANANA_EXPORT_CUSTOMS = 2,
    BANANA_EXPORT_DISTRIBUTED = 3
} BananaExportStatus;

typedef enum DistributionNodeType {
    BANANA_NODE_PORT = 0,
    BANANA_NODE_WAREHOUSE = 1,
    BANANA_NODE_RETAIL = 2,
    BANANA_NODE_RIPENING_CENTER = 3
} DistributionNodeType;

typedef enum BananaRipenessStage {
    BANANA_STAGE_GREEN = 0,
    BANANA_STAGE_BREAKING = 1,
    BANANA_STAGE_YELLOW = 2,
    BANANA_STAGE_SPOTTED = 3,
    BANANA_STAGE_OVERRIPE = 4,
    BANANA_STAGE_BIODEGRADATION = 5
} BananaRipenessStage;

typedef struct BananaIdentifier {
    char value[BANANA_BMS_ID_CAPACITY];
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
    BananaIdentifier sucker_offspring[BANANA_BMS_MAX_SUCKER_OFFSPRING];
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

typedef struct BananaBatch {
    BananaIdentifier batch_id;
    BananaIdentifier origin_farm;
    int bunch_count;
    BananaIdentifier bunch_ids[BANANA_BMS_MAX_BUNCHES_PER_BATCH];
    BananaExportStatus export_status;
    double storage_temp_c;
    double ethylene_exposure;
    int estimated_shelf_life_days;
} BananaBatch;

typedef struct DistributionNode {
    BananaIdentifier node_id;
    DistributionNodeType type;
    BananaGeoCoordinates location;
    double capacity_kg;
    int inventory_batch_count;
    BananaIdentifier inventory_batches[BANANA_BMS_MAX_BATCHES_PER_NODE];
} DistributionNode;

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

BananaRules banana_default_rules(void);

BananaStatus banana_validate_input(
    const BananaInput* input,
    const BananaRules* rules
);

BananaStatus banana_prepare_execution_context(
    const BananaInput* input,
    const BananaRules* rules,
    BananaExecutionContext* context
);

BananaStatus banana_calculate_context(
    BananaExecutionContext* context
);

BananaStatus banana_calculate(
    const BananaInput* input,
    BananaResult* output
);

BananaStatus banana_calculate_with_rules(
    const BananaInput* input,
    const BananaRules* rules,
    BananaResult* output
);

void banana_run_pipeline(
    BananaResult* result,
    const BananaPipeline* pipeline
);

const char* banana_species_name(BananaSpecies species);

const char* banana_ripeness_stage_name(BananaRipenessStage stage);

BananaStatus banana_predict_ripeness(
    const BananaRipenessInput* input,
    BananaRipenessPrediction* prediction
);

BananaStatus banana_predict_ripeness_for_legacy_input(
    const BananaInput* input,
    BananaRipenessPrediction* prediction
);

BananaStatus banana_batch_register(
    const char* batch_id,
    const char* origin_farm,
    double storage_temp_c,
    double ethylene_exposure,
    int estimated_shelf_life_days,
    BananaBatch* batch
);

BananaStatus banana_batch_get(
    const char* batch_id,
    BananaBatch* batch
);

BananaStatus banana_batch_predict_ripeness(
    const char* batch_id,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double mechanical_damage,
    BananaRipenessPrediction* prediction
);

#ifdef __cplusplus
}
#endif

#endif