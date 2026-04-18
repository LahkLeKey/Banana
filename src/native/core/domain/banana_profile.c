#include "banana_profile.h"

#include <limits.h>
#include <string.h>

static BananaStatus checked_multiply(int left, int right, int* product) {
    long long candidate = 0;

    if (product == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    candidate = (long long)left * (long long)right;
    if (candidate > INT_MAX || candidate < INT_MIN) {
        return BANANA_ERROR_OVERFLOW;
    }

    *product = (int)candidate;
    return BANANA_OK;
}

static BananaStatus checked_add(int left, int right, int* total) {
    long long candidate = 0;

    if (total == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    candidate = (long long)left + (long long)right;
    if (candidate > INT_MAX || candidate < INT_MIN) {
        return BANANA_ERROR_OVERFLOW;
    }

    *total = (int)candidate;
    return BANANA_OK;
}

static BananaStatus validate_rules(const BananaRules* rules) {
    if (rules == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (rules->base_rate < 0
        || rules->bonus_threshold < 0
        || rules->bonus_rate < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    return BANANA_OK;
}

BananaRules banana_default_rules(void) {
    BananaRules rules;

    rules.base_rate = 10;
    rules.bonus_threshold = 10;
    rules.bonus_rate = 25;
    return rules;
}

BananaStatus banana_validate_input(
    const BananaInput* input,
    const BananaRules* rules
) {
    BananaStatus status = BANANA_OK;
    int base_banana = 0;
    int bonus_banana = 0;
    int total_banana = 0;

    if (input == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = validate_rules(rules);
    if (status != BANANA_OK) {
        return status;
    }

    if (input->purchases < 0 || input->multiplier < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = checked_multiply(input->purchases, rules->base_rate, &base_banana);
    if (status != BANANA_OK) {
        return status;
    }

    if (input->purchases >= rules->bonus_threshold) {
        status = checked_multiply(input->multiplier, rules->bonus_rate, &bonus_banana);
        if (status != BANANA_OK) {
            return status;
        }
    }

    return checked_add(base_banana, bonus_banana, &total_banana);
}

BananaStatus banana_prepare_execution_context(
    const BananaInput* input,
    const BananaRules* rules,
    BananaExecutionContext* context
) {
    BananaStatus status = BANANA_OK;

    if (context == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_validate_input(input, rules);
    if (status != BANANA_OK) {
        return status;
    }

    memset(context, 0, sizeof(*context));
    context->input = *input;
    context->rules = *rules;
    context->status = BANANA_OK;
    return BANANA_OK;
}

BananaStatus banana_calculate_context(
    BananaExecutionContext* context
) {
    BananaStatus status = BANANA_OK;
    int base_banana = 0;
    int bonus_banana = 0;
    int total_banana = 0;

    if (context == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_validate_input(&context->input, &context->rules);
    if (status != BANANA_OK) {
        context->status = status;
        return status;
    }

    status = checked_multiply(context->input.purchases, context->rules.base_rate, &base_banana);
    if (status != BANANA_OK) {
        context->status = status;
        return status;
    }

    if (context->input.purchases >= context->rules.bonus_threshold) {
        status = checked_multiply(context->input.multiplier, context->rules.bonus_rate, &bonus_banana);
        if (status != BANANA_OK) {
            context->status = status;
            return status;
        }
    }

    status = checked_add(base_banana, bonus_banana, &total_banana);
    if (status != BANANA_OK) {
        context->status = status;
        return status;
    }

    context->breakdown.base_banana = base_banana;
    context->breakdown.bonus_banana = bonus_banana;
    context->result.banana = total_banana;
    context->status = BANANA_OK;
    return BANANA_OK;
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
    BananaExecutionContext context;
    BananaStatus status = BANANA_OK;

    if (output == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_prepare_execution_context(input, rules, &context);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_calculate_context(&context);
    if (status != BANANA_OK) {
        return status;
    }

    *output = context.result;
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
        if (pipeline->steps[index] != 0) {
            pipeline->steps[index](result);
        }
    }
}

BananaStatus banana_predict_ripeness_for_profile_input(
    const BananaInput* input,
    BananaRipenessPrediction* prediction
) {
    BananaRules rules = banana_default_rules();
    BananaRipenessInput ripeness_input;
    BananaStatus status = BANANA_OK;

    if (prediction == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_validate_input(input, &rules);
    if (status != BANANA_OK) {
        return status;
    }

    ripeness_input.temperature_history_c = 0;
    ripeness_input.temperature_history_count = 0;
    ripeness_input.days_since_harvest = input->purchases;
    ripeness_input.ethylene_exposure = (double)input->multiplier;
    ripeness_input.mechanical_damage = 0.0;
    ripeness_input.storage_temp_c = 15.0;
    return banana_predict_ripeness(&ripeness_input, prediction);
}