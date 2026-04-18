#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    test_pipeline_null_safety();

    puts("banana_core_tests: all tests passed");
    return 0;
}
