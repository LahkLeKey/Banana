#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "domain/banana_agriculture.h"
#include "domain/banana_application.h"
#include "domain/banana_cultivation.h"
#include "domain/banana_integration.h"
#include "domain/banana_inventory.h"
#include "domain/banana_logistics.h"
#include "domain/banana_processing.h"
#include "domain/banana_profile.h"
#include "domain/banana_read_models.h"
#include "domain/banana_repositories.h"
#include "domain/banana_retail.h"
#include "domain/banana_services.h"
#include "domain/banana_supply_chain.h"
#include "domain/banana_ml_models.h"
#include "domain/banana_not_banana.h"
#include "dal/banana_db.h"
#include "dal/domain/banana_cultivation.h"
#include "dal/domain/banana_inventory.h"
#include "dal/domain/banana_processing.h"
#include "dal/domain/banana_supply_chain.h"

static void require_true(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "banana_core_tests failure: %s\n", message);
        exit(1);
    }
}

static double absolute_difference(double left, double right) {
    return left > right ? left - right : right - left;
}

static void require_binary_confusion_metrics(
    const BananaMlBinaryClassification* classification,
    const char* context
) {
    double confusion_sum = 0.0;
    double jaccard_denominator = 0.0;
    double expected_jaccard = 0.0;

    require_true(classification->jaccard_similarity >= 0.0 && classification->jaccard_similarity <= 1.0, context);
    require_true(classification->confusion_true_positive >= 0.0 && classification->confusion_true_positive <= 1.0, context);
    require_true(classification->confusion_false_positive >= 0.0 && classification->confusion_false_positive <= 1.0, context);
    require_true(classification->confusion_false_negative >= 0.0 && classification->confusion_false_negative <= 1.0, context);
    require_true(classification->confusion_true_negative >= 0.0 && classification->confusion_true_negative <= 1.0, context);

    confusion_sum = classification->confusion_true_positive +
        classification->confusion_false_positive +
        classification->confusion_false_negative +
        classification->confusion_true_negative;
    require_true(absolute_difference(confusion_sum, 1.0) < 0.000001, context);

    jaccard_denominator = classification->confusion_true_positive +
        classification->confusion_false_positive +
        classification->confusion_false_negative;
    expected_jaccard = jaccard_denominator > 0.0
        ? classification->confusion_true_positive / jaccard_denominator
        : 0.0;
    require_true(absolute_difference(classification->jaccard_similarity, expected_jaccard) < 0.000001, context);
}

static void cap_bonus_step(BananaResult* result) {
    if (result->banana > 200) {
        result->banana = 200;
    }
}

static void add_loyalty_step(BananaResult* result) {
    result->banana += 3;
}

static void test_banana_profile_ok(void) {
    BananaInput input;
    BananaResult output;
    BananaStatus status;

    input.purchases = 10;
    input.multiplier = 2;

    status = banana_calculate(&input, &output);
    require_true(status == BANANA_OK, "expected BANANA_OK for default banana calculation");
    require_true(output.banana == 150, "expected banana=150");
}

static void test_banana_profile_input_validation(void) {
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

static void test_banana_profile_overflow_paths(void) {
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

static void test_banana_profile_custom_rules_and_pipeline(void) {
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

static void test_ml_regression_predicts_deterministic_score(void) {
    BananaMlFeatureVector features;
    double score = 0.0;
    BananaStatus status = BANANA_OK;
    int index = 0;
    const double values[BANANA_ML_FEATURE_COUNT] = {
        0.82,
        0.14,
        0.76,
        0.43,
        0.05,
        0.61,
        0.72,
        0.18
    };

    for (index = 0; index < BANANA_ML_FEATURE_COUNT; index++) {
        features.values[index] = values[index];
    }

    status = banana_ml_predict_regression_score(&features, &score);
    require_true(status == BANANA_OK, "expected BANANA_OK for regression inference");
    require_true(score > 0.7905 && score < 0.7907, "expected deterministic regression score");
}

static void test_ml_regression_rejects_invalid_input(void) {
    BananaMlFeatureVector features;
    double score = 0.0;
    BananaStatus status = BANANA_OK;
    int index = 0;

    for (index = 0; index < BANANA_ML_FEATURE_COUNT; index++) {
        features.values[index] = 0.0;
    }

    status = banana_ml_predict_regression_score(0, &score);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null regression features");

    status = banana_ml_predict_regression_score(&features, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null regression output pointer");

    features.values[0] = 2000001.0;
    status = banana_ml_predict_regression_score(&features, &score);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for out-of-range regression feature");
}

static void test_ml_binary_classifier_separates_banana_and_not_banana(void) {
    BananaMlFeatureVector banana_features;
    BananaMlFeatureVector not_banana_features;
    BananaMlBinaryClassification classification;
    BananaStatus status = BANANA_OK;
    int index = 0;
    const double banana_values[BANANA_ML_FEATURE_COUNT] = {
        0.82,
        0.14,
        0.76,
        0.43,
        0.05,
        0.61,
        0.72,
        0.18
    };
    const double not_banana_values[BANANA_ML_FEATURE_COUNT] = {
        0.12,
        0.81,
        0.17,
        0.09,
        0.74,
        0.18,
        0.22,
        0.66
    };

    for (index = 0; index < BANANA_ML_FEATURE_COUNT; index++) {
        banana_features.values[index] = banana_values[index];
        not_banana_features.values[index] = not_banana_values[index];
    }

    status = banana_ml_predict_binary_classification(&banana_features, &classification);
    require_true(status == BANANA_OK, "expected BANANA_OK for banana-like binary classification sample");
    require_true(classification.predicted_label == BANANA_ML_LABEL_BANANA, "expected banana label for banana-like sample");
    require_true(classification.banana_probability > 0.80, "expected strong banana probability for banana-like sample");
    require_true(absolute_difference(
            classification.banana_probability + classification.not_banana_probability,
            1.0) < 0.000001,
        "expected binary probabilities to sum to one");
    require_binary_confusion_metrics(
        &classification,
        "expected binary confusion metrics to be normalized for banana-like sample");

    status = banana_ml_predict_binary_classification(&not_banana_features, &classification);
    require_true(status == BANANA_OK, "expected BANANA_OK for not-banana binary classification sample");
    require_true(classification.predicted_label == BANANA_ML_LABEL_NOT_BANANA, "expected not-banana label for non-banana sample");
    require_true(classification.banana_probability < 0.20, "expected low banana probability for non-banana sample");
    require_binary_confusion_metrics(
        &classification,
        "expected binary confusion metrics to be normalized for non-banana sample");
}

static void test_ml_binary_classifier_rejects_invalid_input(void) {
    BananaMlFeatureVector features;
    BananaMlBinaryClassification classification;
    BananaStatus status = BANANA_OK;
    int index = 0;

    for (index = 0; index < BANANA_ML_FEATURE_COUNT; index++) {
        features.values[index] = 0.0;
    }

    status = banana_ml_predict_binary_classification(0, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null binary features");

    status = banana_ml_predict_binary_classification(&features, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null binary output");

    features.values[3] = -2000001.0;
    status = banana_ml_predict_binary_classification(&features, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for out-of-range binary feature");
}

static void test_ml_transformer_classifier_separates_sequences(void) {
    BananaMlTransformerInput input;
    BananaMlTransformerClassification classification;
    BananaStatus status = BANANA_OK;
    int token_index = 0;
    int feature_index = 0;
    const double banana_tokens[3][BANANA_ML_TOKEN_FEATURE_COUNT] = {
        { 0.82, 0.15, 0.76, 0.18 },
        { 0.79, 0.12, 0.74, 0.16 },
        { 0.84, 0.11, 0.78, 0.14 }
    };
    const double not_banana_tokens[3][BANANA_ML_TOKEN_FEATURE_COUNT] = {
        { 0.18, 0.82, 0.22, 0.79 },
        { 0.16, 0.85, 0.20, 0.81 },
        { 0.20, 0.78, 0.24, 0.76 }
    };

    input.token_count = 3;
    for (token_index = 0; token_index < input.token_count; token_index++) {
        for (feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            input.tokens[token_index].values[feature_index] = banana_tokens[token_index][feature_index];
        }
    }

    status = banana_ml_predict_transformer_classification(&input, &classification);
    require_true(status == BANANA_OK, "expected BANANA_OK for transformer banana-like sequence");
    require_true(classification.predicted_label == BANANA_ML_LABEL_BANANA, "expected banana label for transformer banana-like sequence");
    require_true(classification.banana_probability > 0.95, "expected high banana probability for transformer banana-like sequence");
    require_true(classification.attention_focus > 0.0 && classification.attention_focus <= 1.0, "expected normalized attention focus");

    input.token_count = 3;
    for (token_index = 0; token_index < input.token_count; token_index++) {
        for (feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            input.tokens[token_index].values[feature_index] = not_banana_tokens[token_index][feature_index];
        }
    }

    status = banana_ml_predict_transformer_classification(&input, &classification);
    require_true(status == BANANA_OK, "expected BANANA_OK for transformer non-banana sequence");
    require_true(classification.predicted_label == BANANA_ML_LABEL_NOT_BANANA, "expected not-banana label for transformer non-banana sequence");
    require_true(classification.banana_probability < 0.05, "expected low banana probability for transformer non-banana sequence");
    require_true(absolute_difference(
            classification.banana_probability + classification.not_banana_probability,
            1.0) < 0.000001,
        "expected transformer probabilities to sum to one");
}

static void test_ml_transformer_classifier_rejects_invalid_input(void) {
    BananaMlTransformerInput input;
    BananaMlTransformerClassification classification;
    BananaStatus status = BANANA_OK;
    int token_index = 0;
    int feature_index = 0;

    input.token_count = 0;
    status = banana_ml_predict_transformer_classification(&input, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for zero token count");

    status = banana_ml_predict_transformer_classification(0, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null transformer input");

    status = banana_ml_predict_transformer_classification(&input, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null transformer output");

    input.token_count = BANANA_ML_MAX_SEQUENCE_LENGTH + 1;
    status = banana_ml_predict_transformer_classification(&input, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for out-of-range transformer sequence length");

    input.token_count = 1;
    for (token_index = 0; token_index < input.token_count; token_index++) {
        for (feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            input.tokens[token_index].values[feature_index] = 0.0;
        }
    }

    input.tokens[0].values[2] = 2000001.0;
    status = banana_ml_predict_transformer_classification(&input, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for out-of-range transformer token value");
}

static void test_domain_capacity_constants(void) {
    require_true(BANANA_ID_CAPACITY == 37, "expected identifier capacity constant to remain stable");
    require_true(BANANA_MAX_SUCKER_OFFSPRING == 8, "expected sucker offspring capacity constant to remain stable");
    require_true(BANANA_MAX_BUNCHES_PER_BATCH == 16, "expected batch-size constant to remain stable");
    require_true(BANANA_MAX_BATCHES_PER_NODE == 32, "expected node-capacity constant to remain stable");
    require_true(BANANA_MAX_BATCH_REGISTRY == 64, "expected registry capacity constant to remain stable");
}

static void test_lifecycle_identifier_and_name_helpers(void) {
    BananaIdentifier identifier;
    BananaStatus status;
    char too_long[BANANA_ID_CAPACITY + 1];
    int index = 0;

    for (index = 0; index < BANANA_ID_CAPACITY; index++) {
        too_long[index] = 'x';
    }
    too_long[BANANA_ID_CAPACITY] = '\0';

    require_true(!banana_identifier_is_valid(0), "expected invalid identifier for null value");
    require_true(!banana_identifier_is_valid(""), "expected invalid identifier for empty value");
    require_true(banana_identifier_is_valid("id-1"), "expected valid identifier for short value");
    require_true(!banana_identifier_is_valid(too_long), "expected invalid identifier for out-of-range length");

    status = banana_identifier_copy(0, "id-1");
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null identifier output");

    status = banana_identifier_copy(&identifier, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null identifier value");

    status = banana_identifier_copy(&identifier, too_long);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for too-long identifier value");

    status = banana_identifier_copy(&identifier, "id-1");
    require_true(status == BANANA_OK, "expected BANANA_OK for valid identifier copy");
    require_true(strcmp(identifier.value, "id-1") == 0, "expected copied identifier value");

    require_true(strcmp(banana_species_name(BANANA_SPECIES_CAVENDISH), "Cavendish") == 0, "expected Cavendish species name");
    require_true(strcmp(banana_species_name(BANANA_SPECIES_PLANTAIN), "Plantain") == 0, "expected Plantain species name");
    require_true(strcmp(banana_species_name(BANANA_SPECIES_OTHER), "Other") == 0, "expected Other species name");
    require_true(strcmp(banana_species_name((BananaSpecies)999), "Unknown") == 0, "expected Unknown species fallback");

    require_true(strcmp(banana_ripeness_stage_name(BANANA_STAGE_GREEN), "GREEN") == 0, "expected GREEN ripeness name");
    require_true(strcmp(banana_ripeness_stage_name(BANANA_STAGE_BREAKING), "BREAKING") == 0, "expected BREAKING ripeness name");
    require_true(strcmp(banana_ripeness_stage_name(BANANA_STAGE_YELLOW), "YELLOW") == 0, "expected YELLOW ripeness name");
    require_true(strcmp(banana_ripeness_stage_name(BANANA_STAGE_SPOTTED), "SPOTTED") == 0, "expected SPOTTED ripeness name");
    require_true(strcmp(banana_ripeness_stage_name(BANANA_STAGE_OVERRIPE), "OVERRIPE") == 0, "expected OVERRIPE ripeness name");
    require_true(strcmp(banana_ripeness_stage_name(BANANA_STAGE_BIODEGRADATION), "BIODEGRADATION") == 0, "expected BIODEGRADATION ripeness name");
    require_true(strcmp(banana_ripeness_stage_name((BananaRipenessStage)999), "UNKNOWN") == 0, "expected UNKNOWN ripeness fallback");
}

static void test_domain_event_helpers_cover_status_and_validation_paths(void) {
    BananaDomainEvent event;
    BananaStatus status;
    char too_long[BANANA_ID_CAPACITY + 1];
    int index = 0;

    for (index = 0; index < BANANA_ID_CAPACITY; index++) {
        too_long[index] = 'z';
    }
    too_long[BANANA_ID_CAPACITY] = '\0';

    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_NONE), "NONE") == 0, "expected NONE event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_PLANTED), "PLANTED") == 0, "expected PLANTED event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_BLOOMED), "BLOOMED") == 0, "expected BLOOMED event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_HARVESTED), "HARVESTED") == 0, "expected HARVESTED event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_SHIPPED), "SHIPPED") == 0, "expected SHIPPED event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_ARRIVED), "ARRIVED") == 0, "expected ARRIVED event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_RIPENED), "RIPENED") == 0, "expected RIPENED event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_SOLD), "SOLD") == 0, "expected SOLD event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_SPOILED), "SPOILED") == 0, "expected SPOILED event name");
    require_true(strcmp(banana_domain_event_type_name(BANANA_EVENT_INVENTORY_RECEIVED), "INVENTORY_RECEIVED") == 0, "expected INVENTORY_RECEIVED event name");
    require_true(strcmp(banana_domain_event_type_name((BananaDomainEventType)999), "NONE") == 0, "expected NONE fallback for unknown event type");

    event.type = BANANA_EVENT_SHIPPED;
    event.quantity = 11;
    banana_domain_event_clear(&event);
    require_true(event.type == BANANA_EVENT_NONE, "expected event clear to reset event type");
    require_true(event.quantity == 0, "expected event clear to reset quantity");

    banana_domain_event_clear(0);

    status = banana_domain_event_record(
        0,
        BANANA_EVENT_SHIPPED,
        "aggregate-1",
        "related-1",
        BANANA_STAGE_YELLOW,
        12,
        110,
        18.4);
    require_true(status == BANANA_OK, "expected BANANA_OK when event output is null");

    status = banana_domain_event_record(
        &event,
        BANANA_EVENT_SHIPPED,
        "aggregate-1",
        "related-1",
        BANANA_STAGE_YELLOW,
        12,
        110,
        18.4);
    require_true(status == BANANA_OK, "expected BANANA_OK for valid event recording");
    require_true(strcmp(event.aggregate_id.value, "aggregate-1") == 0, "expected aggregate identifier to be copied");
    require_true(strcmp(event.related_id.value, "related-1") == 0, "expected related identifier to be copied");

    status = banana_domain_event_record(
        &event,
        BANANA_EVENT_RIPENED,
        0,
        "",
        BANANA_STAGE_SPOTTED,
        3,
        203,
        7.1);
    require_true(status == BANANA_OK, "expected BANANA_OK for optional identifiers");
    require_true(event.aggregate_id.value[0] == '\0', "expected empty aggregate identifier when omitted");
    require_true(event.related_id.value[0] == '\0', "expected empty related identifier when omitted");

    status = banana_domain_event_record(
        &event,
        BANANA_EVENT_SHIPPED,
        too_long,
        "related-1",
        BANANA_STAGE_YELLOW,
        12,
        110,
        18.4);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for too-long aggregate identifier");
    require_true(event.type == BANANA_EVENT_NONE, "expected event state to be cleared after aggregate-id failure");

    status = banana_domain_event_record(
        &event,
        BANANA_EVENT_SHIPPED,
        "aggregate-1",
        too_long,
        BANANA_STAGE_YELLOW,
        12,
        110,
        18.4);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for too-long related identifier");
    require_true(event.type == BANANA_EVENT_NONE, "expected event state to be cleared after related-id failure");
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

static void test_predict_ripeness_for_profile_input_returns_profile(void) {
    BananaInput input;
    BananaRipenessPrediction prediction;
    BananaStatus status;

    input.purchases = 10;
    input.multiplier = 2;

    status = banana_predict_ripeness_for_profile_input(&input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for banana profile prediction");
    require_true(prediction.predicted_stage == BANANA_STAGE_YELLOW, "expected YELLOW stage for default banana profile scenario");
    require_true(prediction.shelf_life_hours > 0, "expected positive remaining shelf life");
    require_true(prediction.spoilage_probability > 0.0, "expected non-zero spoilage probability");
}

static void test_predict_ripeness_validation_and_stage_edges(void) {
    BananaRipenessInput input;
    BananaRipenessPrediction prediction;
    BananaStatus status;
    double cold_history[1] = { 10.0 };
    double warm_history[2] = { 25.0, 26.0 };
    double invalid_history[1] = { 70.0 };

    status = banana_predict_ripeness(0, &prediction);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null ripeness input");

    status = banana_predict_ripeness(&input, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null ripeness output");

    memset(&input, 0, sizeof(input));
    input.temperature_history_c = cold_history;
    input.temperature_history_count = 1;
    input.days_since_harvest = -1;
    input.ethylene_exposure = 0.0;
    input.mechanical_damage = 0.0;
    input.storage_temp_c = 12.0;
    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for negative days-since-harvest");

    input.days_since_harvest = 1;
    input.storage_temp_c = 61.0;
    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for out-of-range storage temperature");

    input.temperature_history_c = invalid_history;
    input.temperature_history_count = 1;
    input.storage_temp_c = 15.0;
    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for out-of-range temperature history");

    input.temperature_history_c = 0;
    input.temperature_history_count = 0;
    input.days_since_harvest = 5;
    input.ethylene_exposure = 0.0;
    input.mechanical_damage = 0.0;
    input.storage_temp_c = 20.0;
    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for BREAKING-stage boundary scenario");
    require_true(prediction.predicted_stage == BANANA_STAGE_BREAKING, "expected BREAKING stage from fallback average-temperature path");

    input.temperature_history_c = cold_history;
    input.temperature_history_count = 1;
    input.days_since_harvest = 20;
    input.ethylene_exposure = 6.0;
    input.mechanical_damage = 0.0;
    input.storage_temp_c = 10.0;
    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for SPOTTED-stage scenario");
    require_true(prediction.predicted_stage == BANANA_STAGE_SPOTTED, "expected SPOTTED stage from boosted ripening index");

    input.temperature_history_c = 0;
    input.temperature_history_count = 0;
    input.days_since_harvest = 17;
    input.ethylene_exposure = 0.0;
    input.mechanical_damage = 0.0;
    input.storage_temp_c = 20.0;
    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for OVERRIPE-stage scenario");
    require_true(prediction.predicted_stage == BANANA_STAGE_OVERRIPE, "expected OVERRIPE stage from high ripening index");

    input.temperature_history_c = warm_history;
    input.temperature_history_count = 2;
    input.days_since_harvest = 20;
    input.ethylene_exposure = 10.0;
    input.mechanical_damage = 1.0;
    input.storage_temp_c = 25.0;
    status = banana_predict_ripeness(&input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for BIODEGRADATION-stage scenario");
    require_true(prediction.predicted_stage == BANANA_STAGE_BIODEGRADATION, "expected BIODEGRADATION stage from extreme ripening index");
    require_true(prediction.shelf_life_hours == 0, "expected zero shelf life once ripening index exceeds biodegradation threshold");
    require_true(absolute_difference(prediction.spoilage_probability, 1.0) < 0.000001, "expected spoilage probability clamp to upper bound");
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

static void test_batch_registry_persists_mutations(void) {
    BananaBatch batch;
    BananaBatch loaded;
    BananaStatus status = banana_batch_register("persisted-batch-1", "persisted-farm-1", 13.1, 2.0, 6, &batch);

    require_true(status == BANANA_OK, "expected BANANA_OK for persisted batch registration");

    status = banana_batch_add_bunch(&batch, "persisted-bunch-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when adding bunch to persisted batch");

    status = banana_batch_get("persisted-batch-1", &loaded);
    require_true(status == BANANA_OK, "expected BANANA_OK when reloading persisted batch after bunch add");
    require_true(loaded.bunch_count == 1, "expected persisted batch to retain bunch count");
    require_true(strcmp(loaded.bunch_ids[0].value, "persisted-bunch-1") == 0, "expected persisted batch to retain bunch identifier");

    status = banana_batch_advance_export_status(&batch, BANANA_EXPORT_SHIPPED);
    require_true(status == BANANA_OK, "expected BANANA_OK when advancing persisted batch export status");

    status = banana_batch_get("persisted-batch-1", &loaded);
    require_true(status == BANANA_OK, "expected BANANA_OK when reloading persisted batch after export transition");
    require_true(loaded.export_status == BANANA_EXPORT_SHIPPED, "expected persisted batch export status to remain updated");
}

static void test_agriculture_registers_farms_fields_and_seedling_transplants(void) {
    BananaFarm farm;
    BananaField field;
    BananaSoilConditions soil_conditions;
    BananaGeoCoordinates location;
    BananaSeedling seedling;
    BananaPlant plant;
    BananaDomainEvent event;
    BananaStatus status;
    int recommended_plant_count = 0;
    int recommended_pick_count = 0;

    soil_conditions.moisture_pct = 42.0;
    soil_conditions.ph = 6.2;
    soil_conditions.organic_matter_pct = 5.0;
    location.latitude = 14.6;
    location.longitude = -90.5;

    status = banana_farm_register("farm-core-1", "North Farm", &farm);
    require_true(status == BANANA_OK, "expected BANANA_OK for farm registration");

    status = banana_field_register("field-core-1", farm.farm_id.value, location, soil_conditions, 2.5, &field);
    require_true(status == BANANA_OK, "expected BANANA_OK for field registration");
    status = banana_farm_add_field(&farm, &field);
    require_true(status == BANANA_OK, "expected BANANA_OK when adding field to farm");
    require_true(farm.field_count == 1, "expected one field on farm");

    status = banana_seedling_register("seedling-1", BANANA_SPECIES_CAVENDISH, 4, &seedling);
    require_true(status == BANANA_OK, "expected BANANA_OK for seedling registration");

    status = banana_seedling_transplant(&seedling, &field, "plant-seedling-1", 12, 2, &plant, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for seedling transplant");
    require_true(field.active_plant_count == 1, "expected active plant count on field after transplant");
    require_true(event.type == BANANA_EVENT_PLANTED, "expected planted event from transplant");

    status = banana_field_plan_planting(&field, &recommended_plant_count);
    require_true(status == BANANA_OK, "expected BANANA_OK for planting plan");
    require_true(recommended_plant_count > 2500, "expected planting recommendation based on area and soil");

    status = banana_field_plan_harvest(&field, 20, &recommended_pick_count);
    require_true(status == BANANA_OK, "expected BANANA_OK for harvest plan");
    require_true(recommended_pick_count >= 15, "expected strong harvest recommendation for healthy soil conditions");
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

static void test_processing_factory_creates_individual_banana_entities(void) {
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaBunchSpec spec;
    BananaFruitSpec fruit_specs[3];
    BananaBunchRecord record;
    BananaDomainEvent bloom_event;
    BananaDomainEvent harvest_event;
    BananaStatus status;

    location.latitude = 15.5;
    location.longitude = -61.4;
    status = banana_plant_register("plant-processing-1", BANANA_SPECIES_CAVENDISH, location, 7, 2, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for processing plant registration");

    status = banana_plant_record_bloom(&plant, "bunch-processing-1", 55, &bloom_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for bloom event recording");
    require_true(bloom_event.type == BANANA_EVENT_BLOOMED, "expected bloomed event type");

    spec.harvest_day_ordinal = 90;
    spec.weight_kg = 1.35;
    spec.finger_count = 3;
    spec.maturity_stage = BANANA_STAGE_GREEN;
    spec.quality_score = 0.95;

    fruit_specs[0].fruit_id = "fruit-1";
    fruit_specs[0].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[0].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[0].weight_kg = 0.45;
    fruit_specs[1].fruit_id = "fruit-2";
    fruit_specs[1].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[1].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[1].weight_kg = 0.45;
    fruit_specs[2].fruit_id = "fruit-3";
    fruit_specs[2].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[2].ripeness_stage = BANANA_STAGE_BREAKING;
    fruit_specs[2].weight_kg = 0.45;

    status = banana_bunch_factory_create(
        &plant,
        "bunch-processing-1",
        &spec,
        BANANA_SPECIES_CAVENDISH,
        fruit_specs,
        3,
        &record,
        &harvest_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for bunch factory creation");
    require_true(harvest_event.type == BANANA_EVENT_HARVESTED, "expected harvested event from bunch factory");
    require_true(record.banana_count == 3, "expected fruit count in bunch record");
    require_true(banana_bunch_record_validate(&record) == BANANA_OK, "expected valid bunch record");
    require_true(banana_bunch_record_all_same_cultivar(&record), "expected all fruit to share cultivar");
    require_true(banana_bunch_record_uniform_ripeness(&record), "expected fruit ripeness to be within one stage");
    require_true(banana_bunch_record_total_weight_kg(&record) > 1.34 && banana_bunch_record_total_weight_kg(&record) < 1.36, "expected total weight to match bunch record");
}

static void test_processing_supports_dimensions_crates_and_inspection(void) {
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaBunchSpec spec;
    BananaFruitSpec fruit_specs[2];
    BananaBunchRecord bunch;
    BananaDimensions dimensions;
    BananaCrate crate;
    BananaInspection inspection;
    BananaStatus status;

    location.latitude = 17.0;
    location.longitude = -88.0;
    status = banana_plant_register("process-plant-2", BANANA_SPECIES_CAVENDISH, location, 3, 1, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for crate test plant registration");

    spec.harvest_day_ordinal = 45;
    spec.weight_kg = 0.9;
    spec.finger_count = 2;
    spec.maturity_stage = BANANA_STAGE_GREEN;
    spec.quality_score = 0.97;
    fruit_specs[0].fruit_id = "crate-fruit-1";
    fruit_specs[0].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[0].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[0].weight_kg = 0.45;
    fruit_specs[1].fruit_id = "crate-fruit-2";
    fruit_specs[1].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[1].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[1].weight_kg = 0.45;

    status = banana_bunch_factory_create(&plant, "crate-bunch-1", &spec, BANANA_SPECIES_CAVENDISH, fruit_specs, 2, &bunch, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for crate test bunch creation");

    status = banana_fruit_estimate_dimensions(&bunch.bananas[0], &dimensions);
    require_true(status == BANANA_OK, "expected BANANA_OK for fruit dimension estimate");
    require_true(dimensions.length_cm > 20.0, "expected positive fruit length estimate");

    status = banana_crate_register("crate-1", 5.0, &crate);
    require_true(status == BANANA_OK, "expected BANANA_OK for crate registration");
    status = banana_crate_pack_bunch(&crate, &bunch);
    require_true(status == BANANA_OK, "expected BANANA_OK when packing bunch into crate");
    require_true(crate.bunch_count == 1, "expected one bunch in crate");

    status = banana_inspection_record("inspector-1", &bunch, 0.9, 0, &inspection);
    require_true(status == BANANA_OK, "expected BANANA_OK for inspection record");
    require_true(inspection.accepted, "expected accepted inspection for high-quality bunch");
}

static void test_processing_supports_harvest_batches(void) {
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaBunchSpec spec;
    BananaFruitSpec fruit_specs[2];
    BananaBunchRecord bunch;
    BananaHarvestBatch harvest_batch;
    BananaStatus status;

    location.latitude = 10.0;
    location.longitude = -84.0;
    status = banana_plant_register("harvest-plant-1", BANANA_SPECIES_PLANTAIN, location, 8, 1, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for harvest-batch plant registration");

    spec.harvest_day_ordinal = 70;
    spec.weight_kg = 0.7;
    spec.finger_count = 2;
    spec.maturity_stage = BANANA_STAGE_GREEN;
    spec.quality_score = 0.91;
    fruit_specs[0].fruit_id = "harvest-fruit-1";
    fruit_specs[0].cultivar = BANANA_SPECIES_PLANTAIN;
    fruit_specs[0].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[0].weight_kg = 0.35;
    fruit_specs[1].fruit_id = "harvest-fruit-2";
    fruit_specs[1].cultivar = BANANA_SPECIES_PLANTAIN;
    fruit_specs[1].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[1].weight_kg = 0.35;

    status = banana_bunch_factory_create(&plant, "harvest-bunch-1", &spec, BANANA_SPECIES_PLANTAIN, fruit_specs, 2, &bunch, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for harvest-batch bunch creation");

    status = banana_harvest_batch_create("harvest-batch-1", "field-harvest-1", 70, &harvest_batch);
    require_true(status == BANANA_OK, "expected BANANA_OK for harvest batch creation");
    status = banana_harvest_batch_add_bunch(&harvest_batch, &bunch);
    require_true(status == BANANA_OK, "expected BANANA_OK when adding bunch to harvest batch");
    require_true(harvest_batch.bunch_count == 1, "expected one bunch in harvest batch");
    require_true(harvest_batch.total_weight_kg > 0.69 && harvest_batch.total_weight_kg < 0.71, "expected harvest batch total weight to accumulate bunch weight");
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

static void test_logistics_supports_containers_and_ripening_rooms(void) {
    BananaBatch batch;
    BananaCurrentLocation location;
    BananaTemperatureSetting temperature;
    BananaContainer container;
    BananaRipeningRoom room;
    BananaGeoCoordinates coordinates;
    BananaRipenessPrediction prediction;
    BananaDomainEvent event;
    BananaStatus status;
    double history[2] = { 16.0, 17.0 };

    coordinates.latitude = 25.0;
    coordinates.longitude = -80.0;
    status = banana_batch_register("logistics-batch-1", "logistics-farm-1", 13.0, 2.0, 5, &batch);
    require_true(status == BANANA_OK, "expected BANANA_OK for logistics batch registration");

    status = banana_current_location_set("node-logistics-1", coordinates, BANANA_NODE_PORT, &location);
    require_true(status == BANANA_OK, "expected BANANA_OK for current location setup");

    temperature.set_point_c = 13.0;
    temperature.tolerance_c = 1.0;
    status = banana_container_register("container-1", location, temperature, 40.0, &container);
    require_true(status == BANANA_OK, "expected BANANA_OK for container registration");
    status = banana_container_load_batch(&container, &batch);
    require_true(status == BANANA_OK, "expected BANANA_OK for loading batch into container");
    require_true(container.batch_count == 1, "expected one batch in container");

    status = banana_ripening_room_register("room-1", location, 16.5, 5.0, BANANA_STAGE_YELLOW, &room);
    require_true(status == BANANA_OK, "expected BANANA_OK for ripening room registration");
    status = banana_ripening_room_admit_batch(&room, &batch);
    require_true(status == BANANA_OK, "expected BANANA_OK when admitting batch to ripening room");
    status = banana_ripening_room_apply(&room, &batch, history, 2, 6, &prediction, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for ripening room application");
    require_true(event.type == BANANA_EVENT_RIPENED, "expected ripened event from ripening room");
    require_true(prediction.predicted_stage >= BANANA_STAGE_BREAKING, "expected ripening room prediction to advance stage");
    status = banana_ripening_room_release_batch(&room, batch.batch_id.value);
    require_true(status == BANANA_OK, "expected BANANA_OK when releasing batch from ripening room");
    status = banana_container_unload_batch(&container, batch.batch_id.value);
    require_true(status == BANANA_OK, "expected BANANA_OK when unloading batch from container");
}

static void test_logistics_supports_trucks(void) {
    BananaBatch batch;
    BananaGeoCoordinates origin_coordinates;
    BananaGeoCoordinates destination_coordinates;
    BananaCurrentLocation origin_location;
    BananaCurrentLocation destination_location;
    BananaTemperatureSetting temperature;
    BananaContainer container;
    BananaTruck truck;
    BananaStatus status;

    origin_coordinates.latitude = 9.9;
    origin_coordinates.longitude = -79.6;
    destination_coordinates.latitude = 9.0;
    destination_coordinates.longitude = -79.5;

    status = banana_batch_register("truck-batch-1", "truck-farm-1", 13.0, 2.0, 6, &batch);
    require_true(status == BANANA_OK, "expected BANANA_OK for truck batch registration");
    status = banana_batch_add_bunch(&batch, "truck-bunch-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when adding bunch for truck test");

    status = banana_current_location_set("truck-node-origin", origin_coordinates, BANANA_NODE_WAREHOUSE, &origin_location);
    require_true(status == BANANA_OK, "expected BANANA_OK for truck origin location");
    status = banana_current_location_set("truck-node-destination", destination_coordinates, BANANA_NODE_RETAIL, &destination_location);
    require_true(status == BANANA_OK, "expected BANANA_OK for truck destination location");

    temperature.set_point_c = 13.0;
    temperature.tolerance_c = 1.0;
    status = banana_container_register("truck-container-1", origin_location, temperature, 40.0, &container);
    require_true(status == BANANA_OK, "expected BANANA_OK for truck container registration");
    status = banana_container_load_batch(&container, &batch);
    require_true(status == BANANA_OK, "expected BANANA_OK when loading truck container batch");

    status = banana_truck_register("truck-1", origin_location, 60.0, &truck);
    require_true(status == BANANA_OK, "expected BANANA_OK for truck registration");
    status = banana_truck_load_container(&truck, &container);
    require_true(status == BANANA_OK, "expected BANANA_OK when loading container onto truck");
    require_true(truck.container_count == 1, "expected one container on truck");

    status = banana_truck_relocate(&truck, destination_location);
    require_true(status == BANANA_OK, "expected BANANA_OK when relocating truck");
    require_true(strcmp(truck.current_location.node_id.value, "truck-node-destination") == 0, "expected truck to track destination location");

    status = banana_truck_unload_container(&truck, container.container_id.value, container.current_weight_kg);
    require_true(status == BANANA_OK, "expected BANANA_OK when unloading container from truck");
    require_true(truck.container_count == 0, "expected no containers on truck after unload");
}

static void test_repositories_persist_aggregates(void) {
    BananaPlant plant;
    BananaPlant loaded_plant;
    BananaGeoCoordinates location;
    BananaBunchSpec spec;
    BananaFruitSpec fruit_specs[2];
    BananaBunchRecord bunch;
    BananaBunchRecord loaded_bunch;
    BananaShipment shipment;
    BananaShipment loaded_shipment;
    BananaInventoryItem item;
    BananaInventoryItem loaded_item;
    BananaStatus status;

    banana_plant_repository_clear();
    banana_bunch_repository_clear();
    banana_shipment_repository_clear();
    banana_inventory_repository_clear();

    location.latitude = 12.0;
    location.longitude = -70.0;
    status = banana_plant_register("repo-plant-1", BANANA_SPECIES_OTHER, location, 3, 1, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for repository plant registration");
    status = banana_plant_repository_save(&plant);
    require_true(status == BANANA_OK, "expected BANANA_OK when saving plant to repository");
    status = banana_plant_repository_get("repo-plant-1", &loaded_plant);
    require_true(status == BANANA_OK, "expected BANANA_OK when loading plant from repository");
    require_true(strcmp(loaded_plant.plant_id.value, "repo-plant-1") == 0, "expected loaded plant identifier");

    spec.harvest_day_ordinal = 60;
    spec.weight_kg = 0.8;
    spec.finger_count = 2;
    spec.maturity_stage = BANANA_STAGE_GREEN;
    spec.quality_score = 0.8;
    fruit_specs[0].fruit_id = "repo-fruit-1";
    fruit_specs[0].cultivar = BANANA_SPECIES_OTHER;
    fruit_specs[0].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[0].weight_kg = 0.4;
    fruit_specs[1].fruit_id = "repo-fruit-2";
    fruit_specs[1].cultivar = BANANA_SPECIES_OTHER;
    fruit_specs[1].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[1].weight_kg = 0.4;

    status = banana_bunch_factory_create(&plant, "repo-bunch-1", &spec, BANANA_SPECIES_OTHER, fruit_specs, 2, &bunch, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for repository bunch factory creation");
    status = banana_bunch_repository_save(&bunch);
    require_true(status == BANANA_OK, "expected BANANA_OK when saving bunch to repository");
    status = banana_bunch_repository_get("repo-bunch-1", &loaded_bunch);
    require_true(status == BANANA_OK, "expected BANANA_OK when loading bunch from repository");
    require_true(loaded_bunch.banana_count == 2, "expected loaded bunch fruit count");

    status = banana_shipment_create("repo-shipment-1", "origin-a", "destination-a", &shipment);
    require_true(status == BANANA_OK, "expected BANANA_OK for repository shipment creation");
    status = banana_shipment_repository_save(&shipment);
    require_true(status == BANANA_OK, "expected BANANA_OK when saving shipment to repository");
    status = banana_shipment_repository_get("repo-shipment-1", &loaded_shipment);
    require_true(status == BANANA_OK, "expected BANANA_OK when loading shipment from repository");
    require_true(strcmp(loaded_shipment.shipment_id.value, "repo-shipment-1") == 0, "expected loaded shipment identifier");

    status = banana_inventory_receive("repo-inventory-1", "store-a", "repo-batch-1", BANANA_STAGE_YELLOW, 10, 2, 2.0, 90, &item, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for repository inventory creation");
    status = banana_inventory_repository_save(&item);
    require_true(status == BANANA_OK, "expected BANANA_OK when saving inventory to repository");
    status = banana_inventory_repository_get("repo-inventory-1", &loaded_item);
    require_true(status == BANANA_OK, "expected BANANA_OK when loading inventory from repository");
    require_true(loaded_item.quantity_on_hand == 10, "expected loaded inventory quantity");

    banana_plant_repository_clear();
    status = banana_plant_repository_get("repo-plant-1", &loaded_plant);
    require_true(status == BANANA_ERROR_NOT_FOUND, "expected cleared plant repository to drop persisted plant");
}

static void test_domain_services_apply_ripening_and_quality_control(void) {
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaBunchSpec spec;
    BananaFruitSpec fruit_specs[2];
    BananaBunchRecord bunch;
    BananaRipenessInput input;
    BananaRipenessPrediction prediction;
    BananaDomainEvent event;
    BananaStatus status;

    location.latitude = -10.0;
    location.longitude = 33.0;
    status = banana_plant_register("service-plant-1", BANANA_SPECIES_PLANTAIN, location, 8, 1, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for service plant registration");

    spec.harvest_day_ordinal = 80;
    spec.weight_kg = 0.6;
    spec.finger_count = 2;
    spec.maturity_stage = BANANA_STAGE_GREEN;
    spec.quality_score = 0.78;
    fruit_specs[0].fruit_id = "service-fruit-1";
    fruit_specs[0].cultivar = BANANA_SPECIES_PLANTAIN;
    fruit_specs[0].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[0].weight_kg = 0.3;
    fruit_specs[1].fruit_id = "service-fruit-2";
    fruit_specs[1].cultivar = BANANA_SPECIES_PLANTAIN;
    fruit_specs[1].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[1].weight_kg = 0.3;

    status = banana_bunch_factory_create(&plant, "service-bunch-1", &spec, BANANA_SPECIES_PLANTAIN, fruit_specs, 2, &bunch, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for service bunch factory creation");
    require_true(banana_quality_control_average_fruit_weight_kg(&bunch) > 0.29, "expected measurable average fruit weight");
    require_true(banana_quality_control_is_underweight(&bunch, 0.35), "expected underweight bunch under stricter threshold");

    input.temperature_history_c = 0;
    input.temperature_history_count = 0;
    input.days_since_harvest = 7;
    input.ethylene_exposure = 7.0;
    input.mechanical_damage = 0.0;
    input.storage_temp_c = 16.0;

    status = banana_ripening_service_evaluate_bunch(&bunch, &input, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for ripening service evaluation");
    require_true(prediction.predicted_stage >= BANANA_STAGE_BREAKING, "expected ripening service to advance stage");

    status = banana_ripening_service_apply(&bunch, &input, 88, &prediction, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for ripening service apply");
    require_true(event.type == BANANA_EVENT_RIPENED, "expected ripened event from ripening service apply");
    require_true(bunch.bananas[0].ripeness_stage == bunch.aggregate.bunch.maturity_stage, "expected child banana stage sync after ripening apply");
}

static void test_domain_services_validation_and_null_prediction_paths(void) {
    BananaBunchRecord bunch;
    BananaBunchSpec spec;
    BananaFruitSpec fruit_spec;
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaRipenessInput input;
    BananaRipenessPrediction prediction;
    BananaDomainEvent event;
    BananaStatus status;

    memset(&bunch, 0, sizeof(bunch));
    memset(&input, 0, sizeof(input));

    require_true(absolute_difference(banana_quality_control_average_fruit_weight_kg(0), 0.0) < 0.000001,
        "expected zero average fruit weight for null bunch");
    require_true(absolute_difference(banana_quality_control_average_fruit_weight_kg(&bunch), 0.0) < 0.000001,
        "expected zero average fruit weight for empty bunch");
    require_true(!banana_quality_control_is_underweight(&bunch, 0.0),
        "expected non-positive threshold to short-circuit underweight check");

    status = banana_ripening_service_evaluate_bunch(0, &input, &prediction);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null bunch in ripening service evaluate");

    status = banana_ripening_service_evaluate_bunch(&bunch, 0, &prediction);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null ripeness input in evaluate");

    status = banana_ripening_service_evaluate_bunch(&bunch, &input, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null ripeness output in evaluate");

    input.temperature_history_c = 0;
    input.temperature_history_count = 0;
    input.days_since_harvest = 3;
    input.ethylene_exposure = 0.0;
    input.mechanical_damage = 0.0;
    input.storage_temp_c = 14.0;
    status = banana_ripening_service_evaluate_bunch(&bunch, &input, &prediction);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for invalid bunch in ripening service evaluate");

    location.latitude = -8.4;
    location.longitude = 34.2;
    status = banana_plant_register("service-validate-plant-1", BANANA_SPECIES_CAVENDISH, location, 7, 1, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for service validation plant registration");

    spec.harvest_day_ordinal = 90;
    spec.weight_kg = 0.45;
    spec.finger_count = 1;
    spec.maturity_stage = BANANA_STAGE_GREEN;
    spec.quality_score = 0.83;
    fruit_spec.fruit_id = "service-validate-fruit-1";
    fruit_spec.cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_spec.ripeness_stage = BANANA_STAGE_GREEN;
    fruit_spec.weight_kg = 0.45;

    status = banana_bunch_factory_create(
        &plant,
        "service-validate-bunch-1",
        &spec,
        BANANA_SPECIES_CAVENDISH,
        &fruit_spec,
        1,
        &bunch,
        0);
    require_true(status == BANANA_OK, "expected BANANA_OK for service validation bunch creation");

    input.days_since_harvest = 6;
    input.ethylene_exposure = 4.0;
    input.storage_temp_c = 16.0;
    status = banana_ripening_service_apply(&bunch, &input, 97, 0, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for ripening service apply with null prediction output");
    require_true(event.type == BANANA_EVENT_RIPENED, "expected ripened event when service apply resolves local prediction");
}

static void test_application_services_execute_commands(void) {
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaHarvestCommand harvest_command;
    BananaFruitSpec fruit_specs[2];
    BananaBunchRecord bunch;
    BananaRipenCommand ripen_command;
    BananaRipenessPrediction prediction;
    BananaShipCommand ship_command;
    BananaBatch batch;
    BananaShipment shipment;
    BananaInventoryReceiveCommand inventory_receive_command;
    BananaInventoryItem item;
    BananaSellCommand sell_command;
    BananaDiscardSpoiledCommand discard_command;
    BananaDomainEvent first_event;
    BananaDomainEvent second_event;
    BananaStatus status;

    banana_plant_repository_clear();
    banana_bunch_repository_clear();
    banana_shipment_repository_clear();
    banana_inventory_repository_clear();

    location.latitude = 13.1;
    location.longitude = -59.6;
    status = banana_plant_register("app-plant-1", BANANA_SPECIES_CAVENDISH, location, 10, 2, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for application plant registration");
    status = banana_plant_repository_save(&plant);
    require_true(status == BANANA_OK, "expected BANANA_OK when saving application plant");

    fruit_specs[0].fruit_id = "app-fruit-1";
    fruit_specs[0].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[0].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[0].weight_kg = 0.4;
    fruit_specs[1].fruit_id = "app-fruit-2";
    fruit_specs[1].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[1].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[1].weight_kg = 0.4;

    harvest_command.bunch_id = "app-bunch-1";
    harvest_command.bloom_day_ordinal = 45;
    harvest_command.bunch_spec.harvest_day_ordinal = 70;
    harvest_command.bunch_spec.weight_kg = 0.8;
    harvest_command.bunch_spec.finger_count = 2;
    harvest_command.bunch_spec.maturity_stage = BANANA_STAGE_GREEN;
    harvest_command.bunch_spec.quality_score = 0.9;
    harvest_command.cultivar = BANANA_SPECIES_CAVENDISH;
    harvest_command.fruit_specs = fruit_specs;
    harvest_command.fruit_count = 2;

    status = banana_application_harvest_bunch("app-plant-1", &harvest_command, &bunch, &first_event, &second_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for harvest application service");
    require_true(first_event.type == BANANA_EVENT_BLOOMED, "expected bloom event from harvest application service");
    require_true(second_event.type == BANANA_EVENT_HARVESTED, "expected harvest event from harvest application service");

    ripen_command.bunch_id = "app-bunch-1";
    ripen_command.input.temperature_history_c = 0;
    ripen_command.input.temperature_history_count = 0;
    ripen_command.input.days_since_harvest = 8;
    ripen_command.input.ethylene_exposure = 8.0;
    ripen_command.input.mechanical_damage = 0.0;
    ripen_command.input.storage_temp_c = 16.0;
    ripen_command.event_day_ordinal = 78;

    status = banana_application_ripen_bunch(&ripen_command, &bunch, &prediction, &first_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for ripen application service");
    require_true(first_event.type == BANANA_EVENT_RIPENED, "expected ripened event from application service");

    ship_command.bunch_id = "app-bunch-1";
    ship_command.batch_id = "app-batch-1";
    ship_command.origin_farm = "app-farm-1";
    ship_command.storage_temp_c = 13.0;
    ship_command.ethylene_exposure = 2.5;
    ship_command.estimated_shelf_life_days = 4;
    ship_command.shipment_id = "app-shipment-1";
    ship_command.origin_node_id = "app-origin-1";
    ship_command.destination_node_id = "app-destination-1";
    ship_command.departure_day_ordinal = 79;

    status = banana_application_ship_bunch(&ship_command, &batch, &shipment, &first_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for ship application service");
    require_true(first_event.type == BANANA_EVENT_SHIPPED, "expected shipped event from application ship service");

    inventory_receive_command.inventory_id = "app-inventory-1";
    inventory_receive_command.location_id = "app-retail-1";
    inventory_receive_command.source_batch_id = batch.batch_id.value;
    inventory_receive_command.ripeness_stage = BANANA_STAGE_YELLOW;
    inventory_receive_command.quantity_on_hand = 20;
    inventory_receive_command.reorder_threshold = 5;
    inventory_receive_command.total_weight_kg = 4.0;
    inventory_receive_command.event_day_ordinal = 82;

    status = banana_application_receive_inventory(&inventory_receive_command, &item, &first_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for inventory receive application service");
    require_true(first_event.type == BANANA_EVENT_INVENTORY_RECEIVED, "expected inventory-received event from application service");

    sell_command.inventory_id = "app-inventory-1";
    sell_command.quantity = 16;
    sell_command.event_day_ordinal = 83;
    status = banana_application_sell_bananas(&sell_command, &item, &first_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for sell application service");
    require_true(item.quantity_on_hand == 4, "expected reduced inventory after sell application service");
    require_true(first_event.type == BANANA_EVENT_SOLD, "expected sold event from application service");

    item.ripeness_stage = BANANA_STAGE_OVERRIPE;
    status = banana_inventory_repository_save(&item);
    require_true(status == BANANA_OK, "expected BANANA_OK when persisting overripe inventory for discard");

    discard_command.inventory_id = "app-inventory-1";
    discard_command.quantity = 2;
    discard_command.event_day_ordinal = 84;
    status = banana_application_discard_spoiled(&discard_command, &item, &first_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for discard application service");
    require_true(item.quantity_on_hand == 2, "expected reduced inventory after discard application service");
    require_true(first_event.type == BANANA_EVENT_SPOILED, "expected spoiled event from discard application service");
}

static void test_application_services_validation_and_not_found_paths(void) {
    BananaHarvestCommand harvest_command;
    BananaFruitSpec fruit_spec;
    BananaBunchRecord bunch;
    BananaRipenCommand ripen_command;
    BananaRipenessPrediction prediction;
    BananaShipCommand ship_command;
    BananaBatch batch;
    BananaShipment shipment;
    BananaInventoryReceiveCommand inventory_receive_command;
    BananaInventoryItem item;
    BananaSellCommand sell_command;
    BananaDiscardSpoiledCommand discard_command;
    BananaDomainEvent event;
    BananaStatus status;

    memset(&harvest_command, 0, sizeof(harvest_command));
    memset(&ripen_command, 0, sizeof(ripen_command));
    memset(&ship_command, 0, sizeof(ship_command));
    memset(&inventory_receive_command, 0, sizeof(inventory_receive_command));
    memset(&sell_command, 0, sizeof(sell_command));
    memset(&discard_command, 0, sizeof(discard_command));

    fruit_spec.fruit_id = "missing-fruit-1";
    fruit_spec.cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_spec.ripeness_stage = BANANA_STAGE_GREEN;
    fruit_spec.weight_kg = 0.3;

    harvest_command.bunch_id = "missing-bunch-app-1";
    harvest_command.bloom_day_ordinal = 10;
    harvest_command.bunch_spec.harvest_day_ordinal = 20;
    harvest_command.bunch_spec.weight_kg = 0.3;
    harvest_command.bunch_spec.finger_count = 1;
    harvest_command.bunch_spec.maturity_stage = BANANA_STAGE_GREEN;
    harvest_command.bunch_spec.quality_score = 0.9;
    harvest_command.cultivar = BANANA_SPECIES_CAVENDISH;
    harvest_command.fruit_specs = &fruit_spec;
    harvest_command.fruit_count = 1;

    status = banana_application_harvest_bunch("missing-plant-app-1", 0, &bunch, &event, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null harvest command");

    status = banana_application_harvest_bunch("missing-plant-app-1", &harvest_command, 0, &event, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null harvest bunch output");

    status = banana_application_harvest_bunch("missing-plant-app-1", &harvest_command, &bunch, &event, &event);
    require_true(status == BANANA_ERROR_NOT_FOUND, "expected not-found status when harvesting from unknown plant");

    ripen_command.bunch_id = "missing-bunch-app-1";
    ripen_command.input.temperature_history_c = 0;
    ripen_command.input.temperature_history_count = 0;
    ripen_command.input.days_since_harvest = 2;
    ripen_command.input.ethylene_exposure = 0.0;
    ripen_command.input.mechanical_damage = 0.0;
    ripen_command.input.storage_temp_c = 14.0;
    ripen_command.event_day_ordinal = 22;

    status = banana_application_ripen_bunch(0, &bunch, &prediction, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null ripen command");

    status = banana_application_ripen_bunch(&ripen_command, 0, &prediction, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null ripen bunch output");

    status = banana_application_ripen_bunch(&ripen_command, &bunch, &prediction, &event);
    require_true(status == BANANA_ERROR_NOT_FOUND, "expected not-found status when ripening unknown bunch");

    ship_command.bunch_id = "missing-bunch-app-1";
    ship_command.batch_id = "missing-batch-app-1";
    ship_command.origin_farm = "missing-farm-app-1";
    ship_command.storage_temp_c = 13.0;
    ship_command.ethylene_exposure = 1.0;
    ship_command.estimated_shelf_life_days = 4;
    ship_command.shipment_id = "missing-shipment-app-1";
    ship_command.origin_node_id = "missing-origin-app-1";
    ship_command.destination_node_id = "missing-destination-app-1";
    ship_command.departure_day_ordinal = 25;

    status = banana_application_ship_bunch(0, &batch, &shipment, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null ship command");

    status = banana_application_ship_bunch(&ship_command, 0, &shipment, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null ship batch output");

    status = banana_application_ship_bunch(&ship_command, &batch, &shipment, &event);
    require_true(status == BANANA_ERROR_NOT_FOUND, "expected not-found status when shipping unknown bunch");

    inventory_receive_command.inventory_id = "invalid-inventory-app-1";
    inventory_receive_command.location_id = "invalid-location-app-1";
    inventory_receive_command.source_batch_id = "invalid-batch-app-1";
    inventory_receive_command.ripeness_stage = BANANA_STAGE_YELLOW;
    inventory_receive_command.quantity_on_hand = -1;
    inventory_receive_command.reorder_threshold = 3;
    inventory_receive_command.total_weight_kg = 1.0;
    inventory_receive_command.event_day_ordinal = 30;

    status = banana_application_receive_inventory(0, &item, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null inventory-receive command");

    status = banana_application_receive_inventory(&inventory_receive_command, 0, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null inventory output");

    status = banana_application_receive_inventory(&inventory_receive_command, &item, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for invalid inventory receive payload");

    sell_command.inventory_id = "missing-inventory-app-1";
    sell_command.quantity = 1;
    sell_command.event_day_ordinal = 31;

    status = banana_application_sell_bananas(0, &item, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null sell command");

    status = banana_application_sell_bananas(&sell_command, 0, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null sell inventory output");

    status = banana_application_sell_bananas(&sell_command, &item, &event);
    require_true(status == BANANA_ERROR_NOT_FOUND, "expected not-found status when selling unknown inventory");

    discard_command.inventory_id = "missing-inventory-app-1";
    discard_command.quantity = 1;
    discard_command.event_day_ordinal = 32;

    status = banana_application_discard_spoiled(0, &item, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null discard command");

    status = banana_application_discard_spoiled(&discard_command, 0, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for null discard inventory output");

    status = banana_application_discard_spoiled(&discard_command, &item, &event);
    require_true(status == BANANA_ERROR_NOT_FOUND, "expected not-found status when discarding unknown inventory");
}

static void test_supply_chain_dal_validation_and_round_trip_paths(void) {
    BananaBatch batch;
    BananaShipment shipment;
    BananaStatus status;
    int db_status;

    memset(&batch, 0, sizeof(batch));
    memset(&shipment, 0, sizeof(shipment));

    db_status = banana_supply_chain_db_save_batch(0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null supply-chain batch save");

    db_status = banana_supply_chain_db_get_batch(0, &batch);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null supply-chain batch id");

    db_status = banana_supply_chain_db_get_batch("", &batch);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for empty supply-chain batch id");

    db_status = banana_supply_chain_db_get_batch("dal-batch-1", 0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null supply-chain batch output");

    status = banana_identifier_copy(&batch.batch_id, "dal-batch-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when assigning dal batch identifier");
    status = banana_identifier_copy(&batch.origin_farm, "dal-farm-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when assigning dal farm identifier");

    db_status = banana_supply_chain_db_save_batch(&batch);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected supply-chain batch save to return a known DB status");

    db_status = banana_supply_chain_db_get_batch("dal-batch-1", &batch);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_NOT_FOUND || db_status == BANANA_DB_ERROR,
        "expected supply-chain batch get to return a known DB status");

    db_status = banana_supply_chain_db_clear_batches();
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected supply-chain batch clear to return a known DB status");

    db_status = banana_supply_chain_db_save_shipment(0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null supply-chain shipment save");

    db_status = banana_supply_chain_db_get_shipment(0, &shipment);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null supply-chain shipment id");

    db_status = banana_supply_chain_db_get_shipment("", &shipment);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for empty supply-chain shipment id");

    db_status = banana_supply_chain_db_get_shipment("dal-shipment-1", 0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null supply-chain shipment output");

    status = banana_identifier_copy(&shipment.shipment_id, "dal-shipment-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when assigning dal shipment identifier");
    status = banana_identifier_copy(&shipment.origin_node_id, "dal-origin-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when assigning dal shipment origin node");
    status = banana_identifier_copy(&shipment.destination_node_id, "dal-destination-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when assigning dal shipment destination node");

    db_status = banana_supply_chain_db_save_shipment(&shipment);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected supply-chain shipment save to return a known DB status");

    db_status = banana_supply_chain_db_get_shipment("dal-shipment-1", &shipment);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_NOT_FOUND || db_status == BANANA_DB_ERROR,
        "expected supply-chain shipment get to return a known DB status");

    db_status = banana_supply_chain_db_clear_shipments();
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected supply-chain shipment clear to return a known DB status");
}

static void test_integration_adapters_translate_external_inputs(void) {
    BananaExternalSensorTelemetry telemetry;
    BananaSensorTelemetryContext telemetry_context;
    BananaExternalRetailInventoryRecord external_inventory;
    BananaInventoryItem item;
    BananaDomainEvent event;
    BananaStatus status;

    telemetry.temperature_history_f[0] = 57.2;
    telemetry.temperature_history_f[1] = 59.0;
    telemetry.temperature_history_count = 2;
    telemetry.transit_days = 5;
    telemetry.ethylene_ppm = 40.0;
    telemetry.bruise_percent = 10.0;
    telemetry.storage_temp_f = 57.2;

    status = banana_sensor_adapter_normalize(&telemetry, &telemetry_context);
    require_true(status == BANANA_OK, "expected BANANA_OK for telemetry normalization");
    require_true(telemetry_context.input.storage_temp_c > 13.9 && telemetry_context.input.storage_temp_c < 14.1, "expected Fahrenheit to Celsius conversion for storage temperature");
    require_true(telemetry_context.input.ethylene_exposure == 4.0, "expected ppm normalization for ethylene exposure");

    external_inventory.product_code = "banana-cav-retail";
    external_inventory.batch_id = "ext-batch-1";
    external_inventory.store_code = "store-ext-1";
    external_inventory.units_available = 12;
    external_inventory.total_weight_lb = 6.0;
    external_inventory.ripeness_level = 5;

    status = banana_inventory_acl_translate(&external_inventory, "ext-inventory-1", 3, 90, &item, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for external inventory translation");
    require_true(item.ripeness_stage == BANANA_STAGE_YELLOW, "expected translated ripeness stage from external inventory level");
    require_true(item.total_weight_kg > 2.7 && item.total_weight_kg < 2.73, "expected pound-to-kilogram conversion for inventory weight");
    require_true(event.type == BANANA_EVENT_INVENTORY_RECEIVED, "expected inventory-received event from ACL translation");
}

static void test_weather_acl_translates_growing_conditions(void) {
    BananaExternalWeatherObservation weather;
    BananaSoilConditions soil_conditions;
    BananaCultivationAdvice advice;
    BananaStatus status;

    weather.rainfall_mm = 5.0;
    weather.humidity_pct = 82.0;
    weather.average_temp_c = 33.5;
    soil_conditions.moisture_pct = 18.0;
    soil_conditions.ph = 6.0;
    soil_conditions.organic_matter_pct = 4.2;

    status = banana_weather_acl_translate(&weather, &soil_conditions, &advice);
    require_true(status == BANANA_OK, "expected BANANA_OK for weather ACL translation");
    require_true(advice.should_irrigate, "expected irrigation advice for dry soil and low rainfall");
    require_true(advice.should_harvest_early, "expected early-harvest advice for hot humid conditions");
    require_true(advice.projected_health == BANANA_HEALTH_STRESSED, "expected stressed projected health for harsh conditions");
}

static void test_read_models_project_stock_and_stats(void) {
    BananaStockView stock_view;
    BananaAverageRipenessReport report;
    BananaCultivarStats stats;
    BananaInventoryItem item;
    BananaDomainEvent inventory_event;
    BananaPlant plant;
    BananaGeoCoordinates location;
    BananaBunchSpec spec;
    BananaFruitSpec fruit_specs[2];
    BananaBunchRecord bunch;
    BananaRipenessPrediction prediction;
    BananaStatus status;

    status = banana_inventory_receive("view-inventory-1", "view-store-1", "view-batch-1", BANANA_STAGE_SPOTTED, 14, 4, 3.0, 100, &item, &inventory_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for read-model inventory setup");

    banana_stock_view_clear(&stock_view);
    status = banana_stock_view_project_inventory_event(&stock_view, &item, &inventory_event);
    require_true(status == BANANA_OK, "expected BANANA_OK for stock-view projection");
    require_true(stock_view.stage_quantities[BANANA_STAGE_SPOTTED] == 14, "expected spotted stage quantity in stock view");

    banana_average_ripeness_report_clear(&report);
    memset(&prediction, 0, sizeof(prediction));
    prediction.predicted_stage = BANANA_STAGE_YELLOW;
    status = banana_average_ripeness_report_record_prediction(&report, &prediction);
    require_true(status == BANANA_OK, "expected BANANA_OK for ripeness report prediction recording");
    require_true(report.average_stage_score > 1.9 && report.average_stage_score < 2.1, "expected average stage score for yellow prediction");

    location.latitude = 11.2;
    location.longitude = -60.0;
    status = banana_plant_register("view-plant-1", BANANA_SPECIES_CAVENDISH, location, 2, 1, &plant, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for stats plant setup");
    spec.harvest_day_ordinal = 50;
    spec.weight_kg = 0.7;
    spec.finger_count = 2;
    spec.maturity_stage = BANANA_STAGE_GREEN;
    spec.quality_score = 0.88;
    fruit_specs[0].fruit_id = "view-fruit-1";
    fruit_specs[0].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[0].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[0].weight_kg = 0.35;
    fruit_specs[1].fruit_id = "view-fruit-2";
    fruit_specs[1].cultivar = BANANA_SPECIES_CAVENDISH;
    fruit_specs[1].ripeness_stage = BANANA_STAGE_GREEN;
    fruit_specs[1].weight_kg = 0.35;

    status = banana_bunch_factory_create(&plant, "view-bunch-1", &spec, BANANA_SPECIES_CAVENDISH, fruit_specs, 2, &bunch, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for stats bunch setup");

    status = banana_average_ripeness_report_record_bunch(&report, &bunch);
    require_true(status == BANANA_OK, "expected BANANA_OK for bunch-based ripeness report recording");
    require_true(report.observation_count == 2, "expected two ripeness observations in report");

    banana_cultivar_stats_clear(&stats, BANANA_SPECIES_CAVENDISH);
    status = banana_cultivar_stats_record_harvest(&stats, &bunch);
    require_true(status == BANANA_OK, "expected BANANA_OK for cultivar harvest stats recording");
    status = banana_cultivar_stats_record_sale(&stats, 2, 0.7);
    require_true(status == BANANA_OK, "expected BANANA_OK for cultivar sale stats recording");
    require_true(stats.harvested_bunch_count == 1, "expected harvested bunch count in cultivar stats");
    require_true(stats.sold_fruit_count == 2, "expected sold fruit count in cultivar stats");
}

static void test_retail_supports_store_sections_pricing_barcodes_and_orders(void) {
    BananaPrice price;
    BananaBarcode barcode;
    BananaStoreSection section;
    BananaRetailOrder order;
    BananaInventoryItem item;
    BananaDomainEvent event;
    BananaStatus status;

    status = banana_price_set(199, "USD", &price);
    require_true(status == BANANA_OK, "expected BANANA_OK for price creation");
    status = banana_barcode_set("0123456789012", &barcode);
    require_true(status == BANANA_OK, "expected BANANA_OK for barcode creation");
    status = banana_store_section_register("section-1", "store-1", barcode, price, BANANA_STAGE_YELLOW, 20, &section);
    require_true(status == BANANA_OK, "expected BANANA_OK for store-section registration");

    status = banana_inventory_receive("retail-inventory-1", "store-1", "retail-batch-1", BANANA_STAGE_YELLOW, 12, 3, 2.4, 60, &item, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for retail inventory setup");
    require_true(banana_store_section_can_face_inventory(&section, &item), "expected store section to accept matching inventory");

    status = banana_retail_order_create("order-1", "store-1", "section-1", 10, price, &order);
    require_true(status == BANANA_OK, "expected BANANA_OK for retail order creation");
    status = banana_retail_order_fulfill(&order, &item, 10, 61, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for retail order fulfillment");
    require_true(order.status == BANANA_ORDER_FULFILLED, "expected fulfilled retail order state");
    require_true(item.quantity_on_hand == 2, "expected reduced inventory after order fulfillment");
    require_true(event.type == BANANA_EVENT_SOLD, "expected sold event for retail order fulfillment");
}

static void test_retail_validation_and_order_state_paths(void) {
    BananaPrice price;
    BananaBarcode barcode;
    BananaStoreSection section;
    BananaRetailOrder order;
    BananaInventoryItem inventory;
    BananaDomainEvent event;
    BananaStatus status;
    char too_long_barcode[BANANA_BARCODE_CAPACITY + 2];
    int index = 0;

    for (index = 0; index < BANANA_BARCODE_CAPACITY + 1; index++) {
        too_long_barcode[index] = '9';
    }
    too_long_barcode[BANANA_BARCODE_CAPACITY + 1] = '\0';

    status = banana_price_set(100, "US", &price);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for non-ISO currency code");

    status = banana_barcode_set(too_long_barcode, &barcode);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for too-long barcode");

    status = banana_price_set(159, "USD", &price);
    require_true(status == BANANA_OK, "expected BANANA_OK for valid retail price");

    status = banana_barcode_set("1234567890123", &barcode);
    require_true(status == BANANA_OK, "expected BANANA_OK for valid barcode");

    status = banana_store_section_register("section-retail-2", "store-retail-2", barcode, price, BANANA_STAGE_YELLOW, 0, &section);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for non-positive facing capacity");

    status = banana_store_section_register("section-retail-2", "store-retail-2", barcode, price, BANANA_STAGE_YELLOW, 12, &section);
    require_true(status == BANANA_OK, "expected BANANA_OK for valid retail section registration");

    status = banana_retail_order_create("order-retail-invalid", "store-retail-2", "section-retail-2", 0, price, &order);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status for non-positive retail order quantity");

    status = banana_retail_order_create("order-retail-partial", "store-retail-2", "section-retail-2", 10, price, &order);
    require_true(status == BANANA_OK, "expected BANANA_OK for retail order creation");

    status = banana_inventory_receive("inventory-retail-partial", "store-retail-2", "batch-retail-2", BANANA_STAGE_YELLOW, 20, 3, 4.0, 121, &inventory, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for retail partial inventory setup");

    status = banana_retail_order_fulfill(&order, &inventory, 11, 122, &event);
    require_true(status == BANANA_ERROR_INVALID_INPUT, "expected invalid-input status when fulfillment quantity exceeds ordered quantity");

    status = banana_retail_order_fulfill(&order, &inventory, 4, 122, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for partial retail order fulfillment");
    require_true(order.status == BANANA_ORDER_PARTIALLY_FULFILLED, "expected PARTIALLY_FULFILLED order status after partial fulfillment");

    status = banana_retail_order_create("order-retail-backorder", "store-retail-2", "section-retail-2", 10, price, &order);
    require_true(status == BANANA_OK, "expected BANANA_OK for backorder scenario order creation");

    status = banana_inventory_receive("inventory-retail-backorder", "store-retail-2", "batch-retail-3", BANANA_STAGE_YELLOW, 4, 2, 0.8, 123, &inventory, 0);
    require_true(status == BANANA_OK, "expected BANANA_OK for backorder inventory setup");

    status = banana_retail_order_fulfill(&order, &inventory, 4, 124, &event);
    require_true(status == BANANA_OK, "expected BANANA_OK for backorder scenario fulfillment");
    require_true(order.status == BANANA_ORDER_BACKORDERED, "expected BACKORDERED order status when stock hits zero before full order quantity");

    require_true(!banana_store_section_can_face_inventory(0, &inventory), "expected false when section is null");
    require_true(!banana_store_section_can_face_inventory(&section, 0), "expected false when inventory is null");

    inventory.quantity_on_hand = 20;
    require_true(!banana_store_section_can_face_inventory(&section, &inventory), "expected false when quantity exceeds facing capacity");

    inventory.quantity_on_hand = 5;
    inventory.ripeness_stage = BANANA_STAGE_GREEN;
    require_true(!banana_store_section_can_face_inventory(&section, &inventory), "expected false when ripeness stage distance exceeds one");
}

static void test_cultivation_inventory_processing_dal_validation_paths(void) {
    BananaPlant plant;
    BananaInventoryItem item;
    BananaBunchRecord bunch;
    BananaStatus status;
    int db_status;

    memset(&plant, 0, sizeof(plant));
    memset(&item, 0, sizeof(item));
    memset(&bunch, 0, sizeof(bunch));

    db_status = banana_cultivation_db_save_plant(0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null cultivation DAL save");
    db_status = banana_cultivation_db_get_plant(0, &plant);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null cultivation DAL get id");
    db_status = banana_cultivation_db_get_plant("", &plant);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for empty cultivation DAL get id");
    db_status = banana_cultivation_db_get_plant("dal-cultivation-plant-1", 0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null cultivation DAL get output");

    status = banana_identifier_copy(&plant.plant_id, "dal-cultivation-plant-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when assigning cultivation DAL plant identifier");
    db_status = banana_cultivation_db_save_plant(&plant);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected cultivation DAL save to return a known DB status");
    db_status = banana_cultivation_db_get_plant("dal-cultivation-plant-1", &plant);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_NOT_FOUND || db_status == BANANA_DB_ERROR,
        "expected cultivation DAL get to return a known DB status");
    db_status = banana_cultivation_db_clear_plants();
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected cultivation DAL clear to return a known DB status");

    db_status = banana_inventory_db_save_item(0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null inventory DAL save");
    db_status = banana_inventory_db_get_item(0, &item);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null inventory DAL get id");
    db_status = banana_inventory_db_get_item("", &item);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for empty inventory DAL get id");
    db_status = banana_inventory_db_get_item("dal-inventory-item-1", 0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null inventory DAL get output");

    status = banana_identifier_copy(&item.inventory_id, "dal-inventory-item-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when assigning inventory DAL item identifier");
    db_status = banana_inventory_db_save_item(&item);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected inventory DAL save to return a known DB status");
    db_status = banana_inventory_db_get_item("dal-inventory-item-1", &item);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_NOT_FOUND || db_status == BANANA_DB_ERROR,
        "expected inventory DAL get to return a known DB status");
    db_status = banana_inventory_db_clear_items();
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected inventory DAL clear to return a known DB status");

    db_status = banana_processing_db_save_bunch(0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null processing DAL save");
    db_status = banana_processing_db_get_bunch(0, &bunch);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null processing DAL get id");
    db_status = banana_processing_db_get_bunch("", &bunch);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for empty processing DAL get id");
    db_status = banana_processing_db_get_bunch("dal-processing-bunch-1", 0);
    require_true(db_status == BANANA_DB_INVALID_ARGUMENT, "expected DB_INVALID_ARGUMENT for null processing DAL get output");

    status = banana_identifier_copy(&bunch.aggregate.bunch.bunch_id, "dal-processing-bunch-1");
    require_true(status == BANANA_OK, "expected BANANA_OK when assigning processing DAL bunch identifier");
    db_status = banana_processing_db_save_bunch(&bunch);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected processing DAL save to return a known DB status");
    db_status = banana_processing_db_get_bunch("dal-processing-bunch-1", &bunch);
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_NOT_FOUND || db_status == BANANA_DB_ERROR,
        "expected processing DAL get to return a known DB status");
    db_status = banana_processing_db_clear_bunches();
    require_true(
        db_status == BANANA_DB_OK || db_status == BANANA_DB_NOT_CONFIGURED || db_status == BANANA_DB_ERROR,
        "expected processing DAL clear to return a known DB status");
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

static void test_not_banana_classifier_flags_polymorphic_junk(void) {
    const char* tokens[] = { "envelope", "metadata", "actor", "entity", "junk" };
    BananaNotBananaClassification classification;
    BananaStatus status;

    status = banana_not_banana_classify(tokens, 5, 2, 1, &classification);
    require_true(status == BANANA_OK, "expected BANANA_OK for junk-only polymorphic payload");
    require_true(classification.predicted_label == BANANA_NOT_BANANA_LABEL_NOT_BANANA,
        "expected NOT_BANANA label for junk-only payload");
    require_true(classification.signal_token_count == 0,
        "expected zero banana signal tokens for junk payload");
    require_true(classification.total_token_count == 5,
        "expected five considered tokens for junk payload");
    require_true(classification.actor_count == 2,
        "expected actor count to round-trip");
    require_true(classification.entity_count == 1,
        "expected entity count to round-trip");
    require_true(absolute_difference(classification.banana_probability, 0.0) < 0.000001,
        "expected zero banana probability for junk-only payload");
    require_true(absolute_difference(classification.not_banana_probability, 1.0) < 0.000001,
        "expected unit not-banana probability for junk-only payload");
    require_true(classification.junk_confidence > 0.5,
        "expected high junk confidence for junk-only payload with actors and entities");
}

static void test_not_banana_classifier_flags_banana_signals(void) {
    const char* tokens[] = { "Banana", "ETHYLENE", "yellow", "harvest", "envelope" };
    BananaNotBananaClassification classification;
    BananaStatus status;

    status = banana_not_banana_classify(tokens, 5, 1, 0, &classification);
    require_true(status == BANANA_OK, "expected BANANA_OK for banana-rich payload");
    require_true(classification.predicted_label == BANANA_NOT_BANANA_LABEL_BANANA,
        "expected BANANA label for banana-signal-rich payload");
    require_true(classification.signal_token_count == 4,
        "expected four banana signal tokens (case-insensitive)");
    require_true(classification.total_token_count == 5,
        "expected five considered tokens for banana-rich payload");
    require_true(classification.banana_probability >= 0.5,
        "expected banana probability >= 0.5 for banana-rich payload");
    require_true(absolute_difference(
        classification.banana_probability + classification.not_banana_probability,
        1.0) < 0.000001,
        "expected banana/not-banana probabilities to sum to 1.0");
}

static void test_not_banana_classifier_handles_empty_payload(void) {
    BananaNotBananaClassification classification;
    BananaStatus status;

    status = banana_not_banana_classify(0, 0, 0, 0, &classification);
    require_true(status == BANANA_OK, "expected BANANA_OK for completely empty payload");
    require_true(classification.predicted_label == BANANA_NOT_BANANA_LABEL_INDETERMINATE,
        "expected INDETERMINATE label when no tokens, actors, or entities are supplied");
    require_true(classification.signal_token_count == 0, "expected zero signals for empty payload");
    require_true(classification.total_token_count == 0, "expected zero considered tokens for empty payload");
    require_true(absolute_difference(classification.banana_probability, 0.0) < 0.000001,
        "expected zero banana probability for empty payload");
    require_true(absolute_difference(classification.not_banana_probability, 0.0) < 0.000001,
        "expected zero not-banana probability for empty payload");
    require_true(absolute_difference(classification.junk_confidence, 0.0) < 0.000001,
        "expected zero junk confidence for empty payload");
}

static void test_not_banana_classifier_skips_empty_tokens(void) {
    const char* tokens[] = { "", 0, "banana", "" };
    BananaNotBananaClassification classification;
    BananaStatus status;

    status = banana_not_banana_classify(tokens, 4, 0, 0, &classification);
    require_true(status == BANANA_OK, "expected BANANA_OK when null/empty tokens are skipped");
    require_true(classification.total_token_count == 1,
        "expected only the non-empty token to be considered");
    require_true(classification.signal_token_count == 1,
        "expected the surviving token to be counted as a banana signal");
    require_true(classification.predicted_label == BANANA_NOT_BANANA_LABEL_BANANA,
        "expected BANANA label when only the banana token survives");
}

static void test_not_banana_classifier_rejects_invalid_input(void) {
    const char* tokens[] = { "banana" };
    BananaNotBananaClassification classification;
    BananaStatus status;

    status = banana_not_banana_classify(tokens, 1, 0, 0, 0);
    require_true(status == BANANA_ERROR_INVALID_INPUT,
        "expected BANANA_ERROR_INVALID_INPUT when out_classification is null");

    status = banana_not_banana_classify(0, 1, 0, 0, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT,
        "expected BANANA_ERROR_INVALID_INPUT when tokens is null but token_count > 0");

    status = banana_not_banana_classify(tokens, -1, 0, 0, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT,
        "expected BANANA_ERROR_INVALID_INPUT for negative token_count");

    status = banana_not_banana_classify(tokens, 1, -1, 0, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT,
        "expected BANANA_ERROR_INVALID_INPUT for negative actor_count");

    status = banana_not_banana_classify(tokens, 1, 0, -1, &classification);
    require_true(status == BANANA_ERROR_INVALID_INPUT,
        "expected BANANA_ERROR_INVALID_INPUT for negative entity_count");
}

int main(void) {
    test_banana_profile_ok();
    test_banana_profile_input_validation();
    test_banana_profile_overflow_paths();
    test_banana_profile_custom_rules_and_pipeline();
    test_prepare_execution_context_and_breakdown();
    test_shared_validation_rejects_invalid_rules_and_context();
    test_ml_regression_predicts_deterministic_score();
    test_ml_regression_rejects_invalid_input();
    test_ml_binary_classifier_separates_banana_and_not_banana();
    test_ml_binary_classifier_rejects_invalid_input();
    test_ml_transformer_classifier_separates_sequences();
    test_ml_transformer_classifier_rejects_invalid_input();
    test_domain_capacity_constants();
    test_lifecycle_identifier_and_name_helpers();
    test_domain_event_helpers_cover_status_and_validation_paths();
    test_predict_ripeness_progresses_through_real_banana_stages();
    test_predict_ripeness_for_profile_input_returns_profile();
    test_predict_ripeness_validation_and_stage_edges();
    test_batch_registry_create_get_and_predict();
    test_batch_registry_returns_not_found();
    test_batch_registry_persists_mutations();
    test_agriculture_registers_farms_fields_and_seedling_transplants();
    test_cultivation_registers_plants_and_harvests_bunches();
    test_processing_factory_creates_individual_banana_entities();
    test_processing_supports_dimensions_crates_and_inspection();
    test_processing_supports_harvest_batches();
    test_cultivation_ripening_is_monotonic_and_spoilage_is_tracked();
    test_supply_chain_tracks_shipments_nodes_and_batch_transitions();
    test_logistics_supports_containers_and_ripening_rooms();
    test_logistics_supports_trucks();
    test_repositories_persist_aggregates();
    test_domain_services_apply_ripening_and_quality_control();
    test_domain_services_validation_and_null_prediction_paths();
    test_application_services_execute_commands();
    test_application_services_validation_and_not_found_paths();
    test_supply_chain_dal_validation_and_round_trip_paths();
    test_cultivation_inventory_processing_dal_validation_paths();
    test_integration_adapters_translate_external_inputs();
    test_weather_acl_translates_growing_conditions();
    test_read_models_project_stock_and_stats();
    test_retail_supports_store_sections_pricing_barcodes_and_orders();
    test_retail_validation_and_order_state_paths();
    test_inventory_receive_sell_and_discard_spoiled_bananas();
    test_pipeline_null_safety();

    test_not_banana_classifier_flags_polymorphic_junk();
    test_not_banana_classifier_flags_banana_signals();
    test_not_banana_classifier_handles_empty_payload();
    test_not_banana_classifier_skips_empty_tokens();
    test_not_banana_classifier_rejects_invalid_input();

    puts("banana_core_tests: all tests passed");
    return 0;
}
