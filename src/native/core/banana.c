#include "banana.h"

#include <limits.h>

static int safe_mul_nonnegative(int left, int right, int* result) {
    if (result == 0) {
        return 0;
    }

    if (left < 0 || right < 0) {
        return 0;
    }

    if (left != 0 && right > (INT_MAX / left)) {
        return 0;
    }

    *result = left * right;
    return 1;
}

static int safe_add_nonnegative(int left, int right, int* result) {
    if (result == 0) {
        return 0;
    }

    if (left < 0 || right < 0) {
        return 0;
    }

    if (left > (INT_MAX - right)) {
        return 0;
    }

    *result = left + right;
    return 1;
}

BananaRules banana_default_rules(void) {
    BananaRules rules;

    rules.base_rate = 10;
    rules.bonus_threshold = 10;
    rules.bonus_rate = 25;

    return rules;
}

BananaStatus banana_calculate(
    const BananaInput* input,
    BananaResult* output
) {
    BananaRules rules = banana_default_rules();
    return banana_calculate_with_rules(input, &rules, output);
}

BananaStatus banana_calculate_with_rules(
    const BananaInput* input,
    const BananaRules* rules,
    BananaResult* output
) {
    int base = 0;
    int bonus = 0;
    int banana = 0;

    if (input == 0 || rules == 0 || output == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (input->purchases < 0 || input->multiplier < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (rules->base_rate < 0 || rules->bonus_threshold < 0 || rules->bonus_rate < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (!safe_mul_nonnegative(input->purchases, rules->base_rate, &base)) {
        return BANANA_ERROR_OVERFLOW;
    }

    if (input->purchases >= rules->bonus_threshold) {
        if (!safe_mul_nonnegative(input->multiplier, rules->bonus_rate, &bonus)) {
            return BANANA_ERROR_OVERFLOW;
        }
    }

    if (!safe_add_nonnegative(base, bonus, &banana)) {
        return BANANA_ERROR_OVERFLOW;
    }

    output->banana = banana;

    return BANANA_OK;
}

void banana_run_pipeline(
    BananaResult* result,
    const BananaPipeline* pipeline
) {
    int index = 0;

    if (result == 0 || pipeline == 0 || pipeline->steps == 0 || pipeline->count <= 0) {
        return;
    }

    for (index = 0; index < pipeline->count; index++) {
        BananaStep step = pipeline->steps[index];
        if (step != 0) {
            step(result);
        }
    }
}
