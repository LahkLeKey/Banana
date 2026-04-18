#ifndef BANANA_INTEGRATION_H
#define BANANA_INTEGRATION_H

#include "banana_agriculture.h"
#include "banana_inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_MAX_SENSOR_HISTORY 16

typedef struct BananaExternalSensorTelemetry {
    double temperature_history_f[BANANA_MAX_SENSOR_HISTORY];
    int temperature_history_count;
    int transit_days;
    double ethylene_ppm;
    double bruise_percent;
    double storage_temp_f;
} BananaExternalSensorTelemetry;

typedef struct BananaSensorTelemetryContext {
    double temperature_history_c[BANANA_MAX_SENSOR_HISTORY];
    BananaRipenessInput input;
} BananaSensorTelemetryContext;

typedef struct BananaExternalRetailInventoryRecord {
    const char* product_code;
    const char* batch_id;
    const char* store_code;
    int units_available;
    double total_weight_lb;
    int ripeness_level;
} BananaExternalRetailInventoryRecord;

typedef struct BananaExternalWeatherObservation {
    double rainfall_mm;
    double humidity_pct;
    double average_temp_c;
} BananaExternalWeatherObservation;

typedef struct BananaCultivationAdvice {
    int should_irrigate;
    int should_harvest_early;
    BananaPlantHealthStatus projected_health;
} BananaCultivationAdvice;

BananaStatus banana_sensor_adapter_normalize(
    const BananaExternalSensorTelemetry* telemetry,
    BananaSensorTelemetryContext* context
);

BananaStatus banana_inventory_acl_translate(
    const BananaExternalRetailInventoryRecord* external_record,
    const char* inventory_id,
    int reorder_threshold,
    int event_day_ordinal,
    BananaInventoryItem* item,
    BananaDomainEvent* event
);

BananaStatus banana_weather_acl_translate(
    const BananaExternalWeatherObservation* weather,
    const BananaSoilConditions* soil_conditions,
    BananaCultivationAdvice* advice
);

#ifdef __cplusplus
}
#endif

#endif