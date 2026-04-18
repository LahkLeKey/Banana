#ifndef BANANA_WRAPPER_H
#define BANANA_WRAPPER_H

#include <stddef.h>

#ifdef _WIN32
#define BANANA_API __declspec(dllexport)
#else
#define BANANA_API __attribute__((visibility("default")))
#endif

typedef enum CInteropStatus {
    BANANA_STATUS_OK = 0,
    BANANA_STATUS_INVALID_ARGUMENT = 1,
    BANANA_STATUS_OVERFLOW = 2,
    BANANA_STATUS_INTERNAL_ERROR = 3,
    BANANA_STATUS_DB_ERROR = 4,
    BANANA_STATUS_DB_NOT_CONFIGURED = 5,
    BANANA_STATUS_NOT_FOUND = 6
} CInteropStatus;

typedef enum CInteropBananaRipenessStage {
    CINTEROP_RIPENESS_STAGE_GREEN = 0,
    CINTEROP_RIPENESS_STAGE_BREAKING = 1,
    CINTEROP_RIPENESS_STAGE_YELLOW = 2,
    CINTEROP_RIPENESS_STAGE_SPOTTED = 3,
    CINTEROP_RIPENESS_STAGE_OVERRIPE = 4,
    CINTEROP_RIPENESS_STAGE_BIODEGRADATION = 5
} CInteropBananaRipenessStage;

typedef enum CInteropDistributionNodeType {
    CINTEROP_DISTRIBUTION_NODE_PORT = 0,
    CINTEROP_DISTRIBUTION_NODE_WAREHOUSE = 1,
    CINTEROP_DISTRIBUTION_NODE_RETAIL = 2,
    CINTEROP_DISTRIBUTION_NODE_RIPENING_CENTER = 3
} CInteropDistributionNodeType;

typedef struct CInteropBananaBreakdown {
    int purchases;
    int multiplier;
    int banana;
} CInteropBananaBreakdown;

typedef struct CInteropBananaRipenessPrediction {
    CInteropBananaRipenessStage predicted_stage;
    int shelf_life_hours;
    double ripening_index;
    double spoilage_probability;
    double cold_chain_risk;
} CInteropBananaRipenessPrediction;

_Static_assert(sizeof(int) == 4, "CInterop ABI requires 32-bit int.");
_Static_assert(sizeof(CInteropStatus) == 4, "CInteropStatus ABI size must be 4 bytes.");
_Static_assert(sizeof(CInteropBananaRipenessStage) == 4, "CInteropBananaRipenessStage ABI size must be 4 bytes.");
_Static_assert(sizeof(CInteropDistributionNodeType) == 4, "CInteropDistributionNodeType ABI size must be 4 bytes.");
_Static_assert(sizeof(CInteropBananaBreakdown) == 12, "CInteropBananaBreakdown ABI size must be 12 bytes.");
_Static_assert(sizeof(double) == 8, "CInterop ABI requires 64-bit double.");
_Static_assert(sizeof(CInteropBananaRipenessPrediction) == 32, "CInteropBananaRipenessPrediction ABI size must be 32 bytes.");
_Static_assert(BANANA_STATUS_OK == 0, "CInteropStatus.Ok ordinal mismatch.");
_Static_assert(BANANA_STATUS_INVALID_ARGUMENT == 1, "CInteropStatus.InvalidArgument ordinal mismatch.");
_Static_assert(BANANA_STATUS_OVERFLOW == 2, "CInteropStatus.Overflow ordinal mismatch.");
_Static_assert(BANANA_STATUS_INTERNAL_ERROR == 3, "CInteropStatus.InternalError ordinal mismatch.");
_Static_assert(BANANA_STATUS_DB_ERROR == 4, "CInteropStatus.DbError ordinal mismatch.");
_Static_assert(BANANA_STATUS_DB_NOT_CONFIGURED == 5, "CInteropStatus.DbNotConfigured ordinal mismatch.");
_Static_assert(BANANA_STATUS_NOT_FOUND == 6, "CInteropStatus.NotFound ordinal mismatch.");
_Static_assert(CINTEROP_RIPENESS_STAGE_GREEN == 0, "CInteropBananaRipenessStage.Green ordinal mismatch.");
_Static_assert(CINTEROP_RIPENESS_STAGE_BREAKING == 1, "CInteropBananaRipenessStage.Breaking ordinal mismatch.");
_Static_assert(CINTEROP_RIPENESS_STAGE_YELLOW == 2, "CInteropBananaRipenessStage.Yellow ordinal mismatch.");
_Static_assert(CINTEROP_RIPENESS_STAGE_SPOTTED == 3, "CInteropBananaRipenessStage.Spotted ordinal mismatch.");
_Static_assert(CINTEROP_RIPENESS_STAGE_OVERRIPE == 4, "CInteropBananaRipenessStage.Overripe ordinal mismatch.");
_Static_assert(CINTEROP_RIPENESS_STAGE_BIODEGRADATION == 5, "CInteropBananaRipenessStage.Biodegradation ordinal mismatch.");
_Static_assert(CINTEROP_DISTRIBUTION_NODE_PORT == 0, "CInteropDistributionNodeType.Port ordinal mismatch.");
_Static_assert(CINTEROP_DISTRIBUTION_NODE_WAREHOUSE == 1, "CInteropDistributionNodeType.Warehouse ordinal mismatch.");
_Static_assert(CINTEROP_DISTRIBUTION_NODE_RETAIL == 2, "CInteropDistributionNodeType.Retail ordinal mismatch.");
_Static_assert(CINTEROP_DISTRIBUTION_NODE_RIPENING_CENTER == 3, "CInteropDistributionNodeType.RipeningCenter ordinal mismatch.");
_Static_assert(offsetof(CInteropBananaBreakdown, purchases) == 0, "CInteropBananaBreakdown.purchases ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaBreakdown, multiplier) == 4, "CInteropBananaBreakdown.multiplier ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaBreakdown, banana) == 8, "CInteropBananaBreakdown.banana ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaRipenessPrediction, predicted_stage) == 0, "CInteropBananaRipenessPrediction.predicted_stage ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaRipenessPrediction, shelf_life_hours) == 4, "CInteropBananaRipenessPrediction.shelf_life_hours ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaRipenessPrediction, ripening_index) == 8, "CInteropBananaRipenessPrediction.ripening_index ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaRipenessPrediction, spoilage_probability) == 16, "CInteropBananaRipenessPrediction.spoilage_probability ABI offset mismatch.");
_Static_assert(offsetof(CInteropBananaRipenessPrediction, cold_chain_risk) == 24, "CInteropBananaRipenessPrediction.cold_chain_risk ABI offset mismatch.");

/*
 * Interop decision:
 * - Wrapper keeps ABI primitive (int + pointer out params).
 * - Profile calculation logic remains isolated behind this boundary.
 */
BANANA_API int banana_calculate_banana(int purchases, int multiplier, int* out_banana);

/*
 * Interop decision:
 * - Struct is blittable and fixed-layout for safe marshaling.
 */
BANANA_API int banana_calculate_banana_with_breakdown(
    int purchases,
    int multiplier,
    CInteropBananaBreakdown* out_breakdown
);

/*
 * Interop decision:
 * - Wrapper allocates UTF-8 bytes and caller must release with banana_free.
 */
BANANA_API int banana_create_banana_message(int purchases, int multiplier, char** out_message);

/*
 * Executes one atomic banana profile DB stage operation.
 * - Inputs are mapped to one SQL execution.
 * - Wrapper allocates UTF-8 JSON and caller must release with banana_free.
 */
BANANA_API int banana_db_query_banana_profile(
    int purchases,
    int multiplier,
    char** out_payload,
    int* out_row_count
);

BANANA_API int banana_predict_banana_ripeness(
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double ethylene_exposure,
    double mechanical_damage,
    double storage_temp_c,
    CInteropBananaRipenessPrediction* out_prediction
);

/*
 * JSON payload contract for managed BananaBatchRecord:
 * - batchId, originFarm, exportStatus, storageTempC, ethyleneExposure, estimatedShelfLifeDays
 */
BANANA_API int banana_create_batch(
    const char* batch_id,
    const char* origin_farm,
    double storage_temp_c,
    double ethylene_exposure,
    int estimated_shelf_life_days,
    char** out_batch_json
);

BANANA_API int banana_get_batch_status(
    const char* batch_id,
    char** out_batch_json
);

BANANA_API int banana_predict_batch_ripeness(
    const char* batch_id,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double mechanical_damage,
    CInteropBananaRipenessPrediction* out_prediction
);

/*
 * JSON payload contract for managed BananaHarvestBatchRecord:
 * - harvestBatchId, fieldId, harvestDayOrdinal, bunchCount, totalWeightKg
 */
BANANA_API int banana_create_harvest_batch(
    const char* harvest_batch_id,
    const char* field_id,
    int harvest_day_ordinal,
    char** out_harvest_batch_json
);

BANANA_API int banana_add_bunch_to_harvest_batch(
    const char* harvest_batch_id,
    const char* bunch_id,
    int harvest_day_ordinal,
    double bunch_weight_kg,
    char** out_harvest_batch_json
);

BANANA_API int banana_get_harvest_batch_status(
    const char* harvest_batch_id,
    char** out_harvest_batch_json
);

/*
 * JSON payload contract for managed BananaTruckRecord:
 * - truckId, currentNodeId, nodeType, latitude, longitude, capacityKg, currentLoadKg, containerCount
 */
BANANA_API int banana_register_truck(
    const char* truck_id,
    const char* node_id,
    CInteropDistributionNodeType node_type,
    double latitude,
    double longitude,
    double capacity_kg,
    char** out_truck_json
);

BANANA_API int banana_load_truck_container(
    const char* truck_id,
    const char* container_id,
    double container_weight_kg,
    char** out_truck_json
);

BANANA_API int banana_unload_truck_container(
    const char* truck_id,
    const char* container_id,
    double container_weight_kg,
    char** out_truck_json
);

BANANA_API int banana_relocate_truck(
    const char* truck_id,
    const char* node_id,
    CInteropDistributionNodeType node_type,
    double latitude,
    double longitude,
    char** out_truck_json
);

BANANA_API int banana_get_truck_status(
    const char* truck_id,
    char** out_truck_json
);

BANANA_API void banana_free(void* pointer);

#endif
