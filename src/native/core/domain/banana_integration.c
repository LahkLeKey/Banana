#include "banana_integration.h"

static double fahrenheit_to_celsius(double value_f) {
    return (value_f - 32.0) * (5.0 / 9.0);
}

static BananaRipenessStage ripeness_level_to_stage(int level) {
    if (level <= 2) {
        return BANANA_STAGE_GREEN;
    }

    if (level == 3) {
        return BANANA_STAGE_BREAKING;
    }

    if (level <= 5) {
        return BANANA_STAGE_YELLOW;
    }

    if (level == 6) {
        return BANANA_STAGE_SPOTTED;
    }

    return BANANA_STAGE_OVERRIPE;
}

BananaStatus banana_sensor_adapter_normalize(
    const BananaExternalSensorTelemetry* telemetry,
    BananaSensorTelemetryContext* context
) {
    int index = 0;

    if (telemetry == 0
        || context == 0
        || telemetry->temperature_history_count < 0
        || telemetry->temperature_history_count > BANANA_MAX_SENSOR_HISTORY
        || telemetry->transit_days < 0
        || telemetry->ethylene_ppm < 0.0
        || telemetry->bruise_percent < 0.0
        || telemetry->bruise_percent > 100.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (index = 0; index < telemetry->temperature_history_count; index++) {
        context->temperature_history_c[index] = fahrenheit_to_celsius(telemetry->temperature_history_f[index]);
    }

    context->input.temperature_history_c = context->temperature_history_c;
    context->input.temperature_history_count = telemetry->temperature_history_count;
    context->input.days_since_harvest = telemetry->transit_days;
    context->input.ethylene_exposure = telemetry->ethylene_ppm / 10.0;
    context->input.mechanical_damage = telemetry->bruise_percent / 100.0;
    context->input.storage_temp_c = fahrenheit_to_celsius(telemetry->storage_temp_f);
    return BANANA_OK;
}

BananaStatus banana_inventory_acl_translate(
    const BananaExternalRetailInventoryRecord* external_record,
    const char* inventory_id,
    int reorder_threshold,
    int event_day_ordinal,
    BananaInventoryItem* item,
    BananaDomainEvent* event
) {
    BananaRipenessStage stage;

    if (external_record == 0 || external_record->units_available <= 0 || external_record->total_weight_lb <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    stage = ripeness_level_to_stage(external_record->ripeness_level);
    return banana_inventory_receive(
        inventory_id,
        external_record->store_code,
        external_record->batch_id,
        stage,
        external_record->units_available,
        reorder_threshold,
        external_record->total_weight_lb * 0.45359237,
        event_day_ordinal,
        item,
        event);
}