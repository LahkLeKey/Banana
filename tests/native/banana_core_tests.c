#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "domain/banana_cultivation.h"
#include "domain/banana_inventory.h"
#include "domain/banana_projection_legacy.h"
#include "domain/banana_supply_chain.h"

static void require_true(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "banana_core_tests failure: %s\n", message);
        exit(1);
    }
}

static void cap_bonus_step(BananaResult* result) {
    if (result->banana > 200) {
        result->banana = 200;
    }
}

static void add_loyalty_step(BananaResult* result) {
    result->banana += 3;
}

static void test_legacy_compatibility_projection_ok(void) {
    BananaInput input;
    BananaResult output;
    BananaStatus status;

    input.purchases = 10;
    input.multiplier = 2;

    status = banana_calculate(&input, &output);
    require_true(status == BANANA_OK, "expected BANANA_OK for default banana calculation");
    require_true(output.banana == 150, "expected banana=150");
}

static void test_legacy_compatibility_input_validation(void) {
    BananaInput input;
    BananaResult output;
    BananaStatus status;

    input.purchases = -1;
    input.multiplier = 2;

    status = banana_calculate(&input, &output);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for negative purchases");

    status = banana_calculate(0, &output);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null input");

    status = banana_calculate(&input, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null output");
}

static void test_legacy_projection_overflow_paths(void) {
    BananaInput input;
    BananaRules rules;
    BananaResult output;
    BananaStatus status;

    input.purchases = 214748364;
    input.multiplier = 1;
    rules.base_rate = 11;
    rules.bonus_threshold = 10;
    rules.bonus_rate = 25;

    status = banana_calculate_with_rules(&input, &rules, &output);
    require_true(status == BANANA_ERROR_OVERFLOW, "expected overflow for base calculation");

    input.purchases = 10;
    input.multiplier = 1073741824;
    rules.base_rate = 10;
    rules.bonus_threshold = 10;
    rules.bonus_rate = 3;

    status = banana_calculate_with_rules(&input, &rules, &output);
    require_true(status == BANANA_ERROR_OVERFLOW, "expected overflow for bonus calculation");

    input.purchases = 100000000;
    input.multiplier = 1;
    rules.base_rate = 20;
    rules.bonus_threshold = 0;
    rules.bonus_rate = 147483648;

    status = banana_calculate_with_rules(&input, &rules, &output);
    require_true(status == BANANA_ERROR_OVERFLOW, "expected overflow for base+bonus total calculation");
}

static void test_legacy_projection_custom_rules_and_pipeline(void) {
    BananaInput input;
    BananaRules rules;
    BananaResult output;
    BananaStatus status;
    BananaStep steps[2];
    BananaPipeline pipeline;

    input.purchases = 12;
    input.multiplier = 7;
    rules.base_rate = 12;
    rules.bonus_threshold = 10;
    rules.bonus_rate = 20;

    status = banana_calculate_with_rules(&input, &rules, &output);
    require_true(status == BANANA_OK, "expected BANANA_OK for custom rules");
    require_true(output.banana == 284, "expected custom banana=284");

    steps[0] = cap_bonus_step;
    steps[1] = add_loyalty_step;
    pipeline.steps = steps;
    pipeline.count = 2;

    banana_run_pipeline(&output, &pipeline);
    require_true(output.banana == 203, "pipeline should cap then add loyalty bananas");
}

static void test_prepare_execution_context_and_breakdown(void) {
    BananaInput input;
    BananaRules rules;
    BananaExecutionContext context;
    BananaStatus status;

    input.purchases = 12;
    input.multiplier = 4;
    rules.base_rate = 10;
    rules.bonus_threshold = 10;
    rules.bonus_rate = 25;

    status = banana_prepare_execution_context(&input, &rules, &context);
    require_true(status == BANANA_OK, "expected BANANA_OK when preparing execution context");
    require_true(context.input.purchases == 12, "execution context should copy purchases");
    require_true(context.input.multiplier == 4, "execution context should copy multiplier");
    require_true(context.result.banana == 0, "execution context should zero result");

    status = banana_calculate_context(&context);
    require_true(status == BANANA_OK, "expected BANANA_OK when calculating prepared execution context");
    require_true(context.breakdown.base_banana == 120, "expected base breakdown value");
    require_true(context.breakdown.bonus_banana == 100, "expected bonus breakdown value");
    require_true(context.result.banana == 220, "expected combined banana result");
}

static void test_shared_validation_rejects_invalid_rules_and_context(void) {
    BananaInput input;
    BananaRules rules;
    BananaExecutionContext context;
    BananaStatus status;

    input.purchases = 1;
    input.multiplier = 1;
    rules.base_rate = -1;
    rules.bonus_threshold = 10;
    rules.bonus_rate = 25;

    status = banana_validate_input(&input, &rules);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for invalid rules");

    status = banana_prepare_execution_context(&input, &rules, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null execution context");

    context.input = input;
    context.rules = rules;
    status = banana_calculate_context(&context);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status when calculating invalid execution context");
}

static void test_domain_capacity_constants(void) {
    require_true(BANANA_ID_CAPACITY == 37, "expected identifier capacity constant to remain stable");
    require_true(BANANA_MAX_SUCKER_OFFSPRING == 8, "expected sucker offspring capacity constant to remain stable");
    require_true(BANANA_MAX_BUNCHES_PER_BATCH == 16, "expected batch-size constant to remain stable");
    require_true(BANANA_MAX_BATCHES_PER_NODE == 32, "expected node-capacity constant to remain stable");
    require_true(BANANA_MAX_BATCH_REGISTRY == 64, "expected registry capacity constant to remain stable");
}

static void test_predict_ripeness_progresses_through_real_banana_stages(void) {
    double green_history[3] = { 12.5, 12.7, 12.9 };
    double yellow_history[3] = { 15.2, 16.1, 17.0 };
    BananaRipenessInput input;
    BananaRipenessPrediction prediction;
    BananaStatus status;

    input.temperature_history_c = green_history;
    input.temperature_history_count = 3;
    input.days_since_harvest = 2;
    input.ethylene_exposure = 1.0;
    input.mechanical_damage = 0.0;
    input.storage_temp_c = 12.8;

    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for green-stage ripeness prediction");
    require_true(prediction.predicted_stage == BANANA_STAGE_GREEN, "expected GREEN stage for cold fresh batch");
    require_true(prediction.shelf_life_hours > 200, "expected long shelf life for green batch");

    input.temperature_history_c = yellow_history;
    input.temperature_history_count = 3;
    input.days_since_harvest = 9;
    input.ethylene_exposure = 8.0;
    input.mechanical_damage = 0.1;
    input.storage_temp_c = 16.0;

    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for yellow-stage ripeness prediction");
    require_true(prediction.predicted_stage == BANANA_STAGE_YELLOW, "expected YELLOW stage for retail-ready batch");
    require_true(prediction.shelf_life_hours < 200, "expected reduced shelf life for more ripe batch");
}

static void test_predict_ripeness_for_legacy_input_returns_profile(void) {
    BananaInput input;
    BananaRipenessPrediction prediction;
    BananaStatus status;

    input.purchases = 10;
    input.multiplier = 2;

    status = banana_predict_ripeness_for_legacy_input(&input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for legacy banana profile prediction");
    require_true(prediction.predicted_stage == BANANA_STAGE_YELLOW, "expected YELLOW stage for default legacy banana scenario");
    require_true(prediction.shelf_life_hours > 0, "expected positive remaining shelf life");
    require_true(prediction.spoilage_probability > 0.0, "expected non-zero spoilage probability");
}

static void test_batch_registry_create_get_and_predict(void) {
    BananaBatch batch;
    BananaBatch loaded;
    BananaRipenessPrediction prediction;
    double history[3] = { 12.5, 13.0, 14.2 };
    BananaStatus status = banana_batch_register("batch-1", "farm-1", 13.2, 2.5, 3, &batch);

    require_true(status == BANANA_OK, "expected BANANA_OK for batch registration");
    require_true(strcmp(batch.batch_id.value, "batch-1") == 0, "expected batch identifier to be stored");
    require_true(strcmp(batch.origin_farm.value, "farm-1") == 0, "expected origin farm to be stored");

    status = banana_batch_get("batch-1", &loaded);
    require_true(status == BANANA_OK, "expected BANANA_OK for batch lookup");
    require_true(loaded.storage_temp_c == 13.2, "expected storage temperature from registry");

    status = banana_batch_predict_ripeness("batch-1", history, 3, 5, 0.1, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for batch ripeness prediction");
    require_true(prediction.shelf_life_hours <= 72, "expected batch shelf life cap to apply");
}

static void test_batch_registry_returns_not_found(void) {
    BananaBatch batch;
    BananaRipenessPrediction prediction;
    BananaStatus status = banana_batch_get("missing-batch", &batch);
    require_true(status == BANANA_ERROR_NOT_FOUND, "expected not-found status for unknown batch");

    status = banana_batch_predict_ripeness("missing-batch", 0, 0, 1, 0.0, &prediction);
    require_true(status == BANANA_ERROR_NOT_FOUND, "expected not-found status for unknown batch prediction");
}

static void test_cultivation_registers_plants_and_harvests_bunches(void) {
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaBunchSpec spec;
    BananaBunchAggregate bunch;
    BananaDomainEvent event;
    BananaStatus status;

    location.latitude = -0.431;
    location.longitude = 36.948;

    status = banana_plant_register(
        "plant-1",
        BANANA_SPECIES_CAVENDISH,
        location,
        12,
        3,
        &plant,
        &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for plant registration");
    require_true(strcmp(plant.plant_id.value, "plant-1") == 0, "expected plant identifier to be stored");
    require_true(event.type == BANANA_EVENT_PLANTED, "expected planted event for plant registration");
    require_true(strcmp(banana_domain_event_type_name(event.type), "PLANTED") == 0, "expected planted event name");

    status = banana_plant_add_sucker(&plant, "plant-1-s1");
    require_true(status == BANANA_OK, "expected BANANA_OK when adding sucker offspring");
    require_true(plant.sucker_offspring_count == 1, "expected sucker offspring count to increase");

    spec.harvest_day_ordinal = 120;
    spec.weight_kg = 24.5;
    spec.finger_count = 108;
    spec.maturity_stage = BANANA_STAGE_GREEN;
    spec.quality_score = 0.92;

    status = banana_bunch_harvest(&plant, "bunch-1", &spec, &bunch, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for bunch harvest");
    require_true(strcmp(bunch.bunch.bunch_id.value, "bunch-1") == 0, "expected bunch identifier to be stored");
    require_true(strcmp(bunch.bunch.plant_id.value, "plant-1") == 0, "expected bunch to reference its plant");
    require_true(bunch.lifecycle_state == BANANA_BUNCH_STATE_HARVESTED, "expected harvested bunch lifecycle state");
    require_true(event.type == BANANA_EVENT_HARVESTED, "expected harvested event for bunch harvest");
}

static void test_cultivation_ripening_is_monotonic_and_spoilage_is_tracked(void) {
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaBunchSpec spec;
    BananaBunchAggregate bunch;
    BananaDomainEvent event;
    BananaStatus status;

    location.latitude = 9.0;
    location.longitude = -79.5;

    status = banana_plant_register("plant-2", BANANA_SPECIES_PLANTAIN, location, 5, 2, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for second plant registration");

    spec.harvest_day_ordinal = 90;
    spec.weight_kg = 18.0;
    spec.finger_count = 72;
    spec.maturity_stage = BANANA_STAGE_BREAKING;
    spec.quality_score = 0.88;

    status = banana_bunch_harvest(&plant, "bunch-2", &spec, &bunch, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for second bunch harvest");

    status = banana_bunch_update_ripeness(&bunch, BANANA_STAGE_YELLOW, 95, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for monotonic ripeness update");
    require_true(banana_bunch_is_market_ready(&bunch), "expected yellow bunch to be market ready");
    require_true(bunch.lifecycle_state == BANANA_BUNCH_STATE_RIPENING, "expected ripening lifecycle state after stage update");
    require_true(event.type == BANANA_EVENT_RIPENED, "expected ripened event for ripeness update");

    status = banana_bunch_update_ripeness(&bunch, BANANA_STAGE_BREAKING, 96, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for ripeness regression");

    status = banana_bunch_mark_spoiled(&bunch, 110, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK when marking bunch spoiled");
    require_true(bunch.bunch.maturity_stage == BANANA_STAGE_BIODEGRADATION, "expected biodegradation stage for spoiled bunch");
    require_true(bunch.lifecycle_state == BANANA_BUNCH_STATE_SPOILED, "expected spoiled lifecycle state");
    require_true(event.type == BANANA_EVENT_SPOILED, "expected spoiled event for spoiled bunch");
}

static void test_supply_chain_tracks_shipments_nodes_and_batch_transitions(void) {
    BananaBatch batch;
    DistributionNode origin;
    DistributionNode destination;
    BananaShipment shipment;
    BananaDomainEvent event;
    BananaGeoCoordinates origin_location;
    BananaGeoCoordinates destination_location;
    BananaStatus status;

    origin_location.latitude = 4.7;
    origin_location.longitude = -74.1;
    destination_location.latitude = 40.7;
    destination_location.longitude = -74.0;

    status = banana_batch_register("batch-ship-1", "farm-ship-1", 13.5, 2.0, 6, &batch);
    require_true(status == BANANA_OK, "expected BANANA_OK for shipment batch registration");

    status = banana_batch_add_bunch(&batch, "bunch-ship-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when adding bunch to batch");
    require_true(batch.bunch_count == 1, "expected batch bunch count to increase");

    status = banana_distribution_node_register("node-origin", BANANA_NODE_PORT, origin_location, 1000.0, &origin);
    require_true(status == BANANA_OK, "expected BANANA_OK for origin node registration");
    status = banana_distribution_node_register("node-destination", BANANA_NODE_WAREHOUSE, destination_location, 1500.0, &destination);
    require_true(status == BANANA_OK, "expected BANANA_OK for destination node registration");

    status = banana_distribution_node_receive_batch(&origin, batch.batch_id.value);
    require_true(status == BANANA_OK, "expected BANANA_OK when origin receives batch");
    require_true(origin.inventory_batch_count == 1, "expected batch to appear in origin inventory");

    status = banana_shipment_create("shipment-1", origin.node_id.value, destination.node_id.value, &shipment);
    require_true(status == BANANA_OK, "expected BANANA_OK for shipment creation");

    status = banana_shipment_add_batch(&shipment, &batch);
    require_true(status == BANANA_OK, "expected BANANA_OK when adding batch to shipment");
    require_true(shipment.batch_count == 1, "expected shipment batch count to increase");

    status = banana_distribution_node_dispatch_batch(&origin, batch.batch_id.value);
    require_true(status == BANANA_OK, "expected BANANA_OK when origin dispatches batch");
    require_true(origin.inventory_batch_count == 0, "expected origin inventory to drop after dispatch");

    status = banana_shipment_dispatch(&shipment, &batch, 140, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK when dispatching shipment");
    require_true(batch.export_status == BANANA_EXPORT_SHIPPED, "expected batch export status to advance to SHIPPED");
    require_true(shipment.status == BANANA_SHIPMENT_IN_TRANSIT, "expected shipment to move in transit");
    require_true(event.type == BANANA_EVENT_SHIPPED, "expected shipped event during dispatch");

    status = banana_shipment_arrive(&shipment, &batch, 147, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK when shipment arrives");
    require_true(batch.export_status == BANANA_EXPORT_CUSTOMS, "expected batch export status to advance to CUSTOMS");
    require_true(shipment.status == BANANA_SHIPMENT_ARRIVED, "expected shipment to move to arrived");
    require_true(event.type == BANANA_EVENT_ARRIVED, "expected arrived event for shipment arrival");

    status = banana_batch_advance_export_status(&batch, BANANA_EXPORT_DISTRIBUTED);
    require_true(status == BANANA_OK, "expected BANANA_OK when advancing batch to distributed");
    require_true(batch.export_status == BANANA_EXPORT_DISTRIBUTED, "expected distributed export status");

    status = banana_distribution_node_receive_batch(&destination, batch.batch_id.value);
    require_true(status == BANANA_OK, "expected BANANA_OK when destination receives distributed batch");
    require_true(destination.inventory_batch_count == 1, "expected destination inventory to contain the batch");
}

static void test_inventory_receive_sell_and_discard_spoiled_bananas(void) {
    BananaInventoryItem item;
    BananaDomainEvent event;
    BananaStatus status;

    status = banana_inventory_receive(
        "inventory-1",
        "retail-1",
        "batch-ship-1",
        BANANA_STAGE_YELLOW,
        120,
        40,
        24.0,
        150,
        &item,
        &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for inventory receipt");
    require_true(event.type == BANANA_EVENT_INVENTORY_RECEIVED, "expected inventory-received event on receipt");
    require_true(!banana_inventory_requires_reorder(&item), "expected no reorder when inventory is above threshold");

    status = banana_inventory_sell(&item, 90, 151, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK when selling bananas from inventory");
    require_true(item.quantity_on_hand == 30, "expected inventory quantity to decrease after sale");
    require_true(event.type == BANANA_EVENT_SOLD, "expected sold event for inventory sale");
    require_true(banana_inventory_requires_reorder(&item), "expected reorder signal when inventory drops below threshold");

    item.ripeness_stage = BANANA_STAGE_OVERRIPE;
    status = banana_inventory_discard_spoiled(&item, 10, 154, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK when discarding spoiled bananas");
    require_true(item.quantity_on_hand == 20, "expected inventory quantity to decrease after spoilage discard");
    require_true(event.type == BANANA_EVENT_SPOILED, "expected spoiled event for discarded bananas");
}

static void test_pipeline_null_safety(void) {
    BananaResult output;
    BananaPipeline pipeline;

    output.banana = 3;

    pipeline.steps = 0;
    pipeline.count = 0;

    banana_run_pipeline(0, &pipeline);
    banana_run_pipeline(&output, 0);
    banana_run_pipeline(&output, &pipeline);

    require_true(output.banana == 3, "null-safe pipeline must not mutate banana");
}

int main(void) {
    test_legacy_compatibility_projection_ok();
    test_legacy_compatibility_input_validation();
    test_legacy_projection_overflow_paths();
    test_legacy_projection_custom_rules_and_pipeline();
    test_prepare_execution_context_and_breakdown();
    test_shared_validation_rejects_invalid_rules_and_context();
    test_domain_capacity_constants();
    test_predict_ripeness_progresses_through_real_banana_stages();
    test_predict_ripeness_for_legacy_input_returns_profile();
    test_batch_registry_create_get_and_predict();
    test_batch_registry_returns_not_found();
    test_cultivation_registers_plants_and_harvests_bunches();
    test_cultivation_ripening_is_monotonic_and_spoilage_is_tracked();
    test_supply_chain_tracks_shipments_nodes_and_batch_transitions();
    test_inventory_receive_sell_and_discard_spoiled_bananas();
    test_pipeline_null_safety();

    puts("banana_core_tests: all tests passed");
    return 0;
}
