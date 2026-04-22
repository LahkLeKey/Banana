#include "../shared/banana_ml_internal.h"

static const double k_transformer_query[BANANA_ML_TOKEN_FEATURE_COUNT][BANANA_ML_TOKEN_FEATURE_COUNT] = {
    { 0.70, 0.10, 0.05, 0.00 },
    { 0.15, 0.65, 0.05, 0.05 },
    { 0.05, 0.10, 0.70, 0.05 },
    { 0.10, 0.05, 0.05, 0.70 }
};

static const double k_transformer_key[BANANA_ML_TOKEN_FEATURE_COUNT][BANANA_ML_TOKEN_FEATURE_COUNT] = {
    { 0.66, 0.12, 0.08, 0.04 },
    { 0.14, 0.68, 0.06, 0.06 },
    { 0.06, 0.09, 0.69, 0.08 },
    { 0.08, 0.07, 0.07, 0.66 }
};

static const double k_transformer_value[BANANA_ML_TOKEN_FEATURE_COUNT][BANANA_ML_TOKEN_FEATURE_COUNT] = {
    { 0.84, 0.03, 0.07, 0.02 },
    { 0.04, 0.82, 0.05, 0.04 },
    { 0.07, 0.05, 0.81, 0.04 },
    { 0.03, 0.05, 0.04, 0.83 }
};

static const double k_transformer_banana_head[BANANA_ML_TOKEN_FEATURE_COUNT] = {
    1.25, -1.15, 1.10, -1.05
};

static const double k_transformer_not_banana_head[BANANA_ML_TOKEN_FEATURE_COUNT] = {
    -1.05, 1.20, -0.95, 1.15
};

static void banana_ml_transformer_project_token(
    const double matrix[BANANA_ML_TOKEN_FEATURE_COUNT][BANANA_ML_TOKEN_FEATURE_COUNT],
    const BananaMlToken* token,
    double out_vector[BANANA_ML_TOKEN_FEATURE_COUNT]
) {
    for (int row = 0; row < BANANA_ML_TOKEN_FEATURE_COUNT; row++) {
        double value = 0.0;

        for (int col = 0; col < BANANA_ML_TOKEN_FEATURE_COUNT; col++) {
            value += matrix[row][col] * token->values[col];
        }

        out_vector[row] = value;
    }
}

BananaStatus banana_ml_predict_transformer_classification(
    const BananaMlTransformerInput* input,
    BananaMlTransformerClassification* out_classification
) {
    double queries[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];
    double keys[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];
    double values[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];
    double pooled_context[BANANA_ML_TOKEN_FEATURE_COUNT] = {0};
    double attention_focus_sum = 0.0;
    double banana_logit = 0.0;
    double not_banana_logit = 0.0;
    double min_logit = 0.0;
    double banana_shifted = 0.0;
    double not_banana_shifted = 0.0;
    double denom = 0.0;

    if (input == 0 || out_classification == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (input->token_count <= 0 || input->token_count > BANANA_ML_MAX_SEQUENCE_LENGTH) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (int token_index = 0; token_index < input->token_count; token_index++) {
        for (int feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            if (!banana_ml_internal_value_is_valid(input->tokens[token_index].values[feature_index])) {
                return BANANA_ERROR_INVALID_INPUT;
            }
        }

        banana_ml_transformer_project_token(
            k_transformer_query,
            &input->tokens[token_index],
            queries[token_index]);
        banana_ml_transformer_project_token(
            k_transformer_key,
            &input->tokens[token_index],
            keys[token_index]);
        banana_ml_transformer_project_token(
            k_transformer_value,
            &input->tokens[token_index],
            values[token_index]);
    }

    for (int token_index = 0; token_index < input->token_count; token_index++) {
        double row_scores[BANANA_ML_MAX_SEQUENCE_LENGTH];
        double row_min = 0.0;
        double row_sum = 0.0;
        double row_focus = 0.0;
        double context[BANANA_ML_TOKEN_FEATURE_COUNT] = {0};

        for (int other_index = 0; other_index < input->token_count; other_index++) {
            row_scores[other_index] = banana_ml_internal_dot(
                queries[token_index],
                keys[other_index],
                BANANA_ML_TOKEN_FEATURE_COUNT
            ) * 0.5;

            if (other_index == 0 || row_scores[other_index] < row_min) {
                row_min = row_scores[other_index];
            }
        }

        for (int other_index = 0; other_index < input->token_count; other_index++) {
            row_scores[other_index] = (row_scores[other_index] - row_min) + 0.000001;
            row_sum += row_scores[other_index];
        }

        if (row_sum <= 0.0) {
            return BANANA_ERROR_INVALID_INPUT;
        }

        for (int other_index = 0; other_index < input->token_count; other_index++) {
            double attention_weight = row_scores[other_index] / row_sum;

            if (attention_weight > row_focus) {
                row_focus = attention_weight;
            }

            for (int feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
                context[feature_index] += attention_weight * values[other_index][feature_index];
            }
        }

        attention_focus_sum += row_focus;

        for (int feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            pooled_context[feature_index] += context[feature_index];
        }
    }

    for (int feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
        pooled_context[feature_index] /= (double)input->token_count;
    }

    banana_logit =
        0.12 + banana_ml_internal_dot(
            k_transformer_banana_head,
            pooled_context,
            BANANA_ML_TOKEN_FEATURE_COUNT);

    not_banana_logit =
        0.05 + banana_ml_internal_dot(
            k_transformer_not_banana_head,
            pooled_context,
            BANANA_ML_TOKEN_FEATURE_COUNT);

    if (!banana_ml_internal_value_is_valid(banana_logit) ||
        !banana_ml_internal_value_is_valid(not_banana_logit)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    min_logit = banana_logit < not_banana_logit ? banana_logit : not_banana_logit;
    banana_shifted = (banana_logit - min_logit) + 0.000001;
    not_banana_shifted = (not_banana_logit - min_logit) + 0.000001;
    denom = banana_shifted + not_banana_shifted;

    if (denom <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    out_classification->banana_probability = banana_shifted / denom;
    out_classification->not_banana_probability = not_banana_shifted / denom;
    out_classification->predicted_label =
        out_classification->banana_probability >= out_classification->not_banana_probability
            ? BANANA_ML_LABEL_BANANA
            : BANANA_ML_LABEL_NOT_BANANA;
    out_classification->attention_focus = banana_ml_internal_clamp(
        attention_focus_sum / (double)input->token_count,
        0.0,
        1.0
    );

    return BANANA_OK;
}