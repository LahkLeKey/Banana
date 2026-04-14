#include <stdio.h>
#include <stdlib.h>

#include "banana.h"

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

static void test_default_calculation_ok(void) {
    BananaInput input;
    BananaResult output;
    BananaStatus status;

    input.purchases = 10;
    input.multiplier = 2;

    status = banana_calculate(&input, &output);
    require_true(status == BANANA_OK, "expected BANANA_OK for default banana calculation");
    require_true(output.banana == 150, "expected banana=150");
}

static void test_invalid_input_rejected(void) {
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

static void test_rules_overflow_paths(void) {
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

static void test_custom_rules_and_pipeline(void) {
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
    test_default_calculation_ok();
    test_invalid_input_rejected();
    test_rules_overflow_paths();
    test_custom_rules_and_pipeline();
    test_pipeline_null_safety();

    puts("banana_core_tests: all tests passed");
    return 0;
}
