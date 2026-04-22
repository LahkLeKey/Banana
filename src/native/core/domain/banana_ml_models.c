#include "banana_ml_models.h"

static const double k_regression_weights[BANANA_ML_FEATURE_COUNT] = {
    0.23,
    -0.19,
    0.31,
    0.11,
    -0.08,
    0.17,
    0.21,
    -0.14
};

static const double k_binary_weights[BANANA_ML_FEATURE_COUNT] = {
    1.40,
    -1.20,
    1.10,
    0.90,
    -1.30,
    0.80,
    1.00,
    -1.10
};

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
    1.25,
    -1.15,
    1.10,
    -1.05
};

static const double k_transformer_not_banana_head[BANANA_ML_TOKEN_FEATURE_COUNT] = {
    -1.05,
    1.20,
    -0.95,
    1.15
};

static int banana_ml_value_is_valid(double value) {
    if (value != value) {
        return 0;
    }

    if (value > 1000000.0 || value < -1000000.0) {
        return 0;
    }

    return 1;
}

static double banana_ml_absolute(double value) {
    return value < 0.0 ? -value : value;
}

static double banana_ml_clamp(double value, double min_value, double max_value) {
    if (value < min_value) {
        return min_value;
    }

    if (value > max_value) {
        return max_value;
    }

    return value;
}

static double banana_ml_dot(const double* left, const double* right, int count) {
    double result = 0.0;
    int index = 0;

    for (index = 0; index < count; index++) {
        result += left[index] * right[index];
    }

    return result;
}

static BananaStatus banana_ml_validate_feature_vector(const BananaMlFeatureVector* features) {
    int index = 0;

    if (features == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (index = 0; index < BANANA_ML_FEATURE_COUNT; index++) {
        if (!banana_ml_value_is_valid(features->values[index])) {
            return BANANA_ERROR_INVALID_INPUT;
        }
    }

    return BANANA_OK;
}

static double banana_ml_linear_score(
    const double* weights,
    const BananaMlFeatureVector* features,
    double bias
) {
    return bias + banana_ml_dot(weights, features->values, BANANA_ML_FEATURE_COUNT);
}

static double banana_ml_pseudo_sigmoid(double value) {
    double magnitude = banana_ml_absolute(value);
    return 0.5 + (value / (2.0 * (1.0 + magnitude)));
}

static void banana_ml_project_token(
    const double matrix[BANANA_ML_TOKEN_FEATURE_COUNT][BANANA_ML_TOKEN_FEATURE_COUNT],
    const BananaMlToken* token,
    double out_vector[BANANA_ML_TOKEN_FEATURE_COUNT]
) {
    int row = 0;
    int column = 0;

    for (row = 0; row < BANANA_ML_TOKEN_FEATURE_COUNT; row++) {
        double value = 0.0;

        for (column = 0; column < BANANA_ML_TOKEN_FEATURE_COUNT; column++) {
            value += matrix[row][column] * token->values[column];
        }

        out_vector[row] = value;
    }
}

BananaStatus banana_ml_predict_regression_score(
    const BananaMlFeatureVector* features,
    double* out_score
) {
    BananaStatus status = BANANA_OK;
    double score = 0.0;

    if (out_score == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_ml_validate_feature_vector(features);
    if (status != BANANA_OK) {
        return status;
    }

    score = banana_ml_linear_score(k_regression_weights, features, 0.12);
    *out_score = banana_ml_clamp(score, 0.0, 1.0);
    return BANANA_OK;
}

BananaStatus banana_ml_predict_binary_classification(
    const BananaMlFeatureVector* features,
    BananaMlBinaryClassification* out_classification
) {
    BananaStatus status = BANANA_OK;
    double margin = 0.0;
    double banana_probability = 0.0;

    if (out_classification == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_ml_validate_feature_vector(features);
    if (status != BANANA_OK) {
        return status;
    }

    margin = banana_ml_linear_score(k_binary_weights, features, -0.20);
    banana_probability = banana_ml_clamp(banana_ml_pseudo_sigmoid(margin), 0.0, 1.0);

    out_classification->predicted_label = banana_probability >= 0.5
        ? BANANA_ML_LABEL_BANANA
        : BANANA_ML_LABEL_NOT_BANANA;
    out_classification->banana_probability = banana_probability;
    out_classification->not_banana_probability = 1.0 - banana_probability;
    out_classification->decision_margin = margin;
    return BANANA_OK;
}

BananaStatus banana_ml_predict_transformer_classification(
    const BananaMlTransformerInput* input,
    BananaMlTransformerClassification* out_classification
) {
    double queries[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];
    double keys[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];
    double values[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];
    double pooled_context[BANANA_ML_TOKEN_FEATURE_COUNT] = { 0.0, 0.0, 0.0, 0.0 };
    double attention_focus_sum = 0.0;
    double banana_logit = 0.0;
    double not_banana_logit = 0.0;
    double min_logit = 0.0;
    double banana_shifted = 0.0;
    double not_banana_shifted = 0.0;
    double probability_denominator = 0.0;
    int token_index = 0;
    int other_index = 0;
    int feature_index = 0;

    if (input == 0 || out_classification == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (input->token_count <= 0 || input->token_count > BANANA_ML_MAX_SEQUENCE_LENGTH) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (token_index = 0; token_index < input->token_count; token_index++) {
        for (feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            if (!banana_ml_value_is_valid(input->tokens[token_index].values[feature_index])) {
                return BANANA_ERROR_INVALID_INPUT;
            }
        }

        banana_ml_project_token(k_transformer_query, &input->tokens[token_index], queries[token_index]);
        banana_ml_project_token(k_transformer_key, &input->tokens[token_index], keys[token_index]);
        banana_ml_project_token(k_transformer_value, &input->tokens[token_index], values[token_index]);
    }

    for (token_index = 0; token_index < input->token_count; token_index++) {
        double row_scores[BANANA_ML_MAX_SEQUENCE_LENGTH];
        double row_min = 0.0;
        double row_sum = 0.0;
        double row_focus = 0.0;
        double context[BANANA_ML_TOKEN_FEATURE_COUNT] = { 0.0, 0.0, 0.0, 0.0 };

        for (other_index = 0; other_index < input->token_count; other_index++) {
            row_scores[other_index] = banana_ml_dot(
                queries[token_index],
                keys[other_index],
                BANANA_ML_TOKEN_FEATURE_COUNT) * 0.5;

            if (other_index == 0 || row_scores[other_index] < row_min) {
                row_min = row_scores[other_index];
            }
        }

        for (other_index = 0; other_index < input->token_count; other_index++) {
            row_scores[other_index] = (row_scores[other_index] - row_min) + 0.000001;
            row_sum += row_scores[other_index];
        }

        if (row_sum <= 0.0) {
            return BANANA_ERROR_INVALID_INPUT;
        }

        for (other_index = 0; other_index < input->token_count; other_index++) {
            double attention_weight = row_scores[other_index] / row_sum;

            if (attention_weight > row_focus) {
                row_focus = attention_weight;
            }

            for (feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
                context[feature_index] += attention_weight * values[other_index][feature_index];
            }
        }

        attention_focus_sum += row_focus;
        for (feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
            pooled_context[feature_index] += context[feature_index];
        }
    }

    for (feature_index = 0; feature_index < BANANA_ML_TOKEN_FEATURE_COUNT; feature_index++) {
        pooled_context[feature_index] /= (double)input->token_count;
    }

    banana_logit = 0.12 + banana_ml_dot(
        k_transformer_banana_head,
        pooled_context,
        BANANA_ML_TOKEN_FEATURE_COUNT);
    not_banana_logit = 0.05 + banana_ml_dot(
        k_transformer_not_banana_head,
        pooled_context,
        BANANA_ML_TOKEN_FEATURE_COUNT);

    if (!banana_ml_value_is_valid(banana_logit) || !banana_ml_value_is_valid(not_banana_logit)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    min_logit = banana_logit < not_banana_logit ? banana_logit : not_banana_logit;
    banana_shifted = (banana_logit - min_logit) + 0.000001;
    not_banana_shifted = (not_banana_logit - min_logit) + 0.000001;
    probability_denominator = banana_shifted + not_banana_shifted;

    if (probability_denominator <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    out_classification->banana_probability = banana_shifted / probability_denominator;
    out_classification->not_banana_probability = not_banana_shifted / probability_denominator;
    out_classification->predicted_label = out_classification->banana_probability >= out_classification->not_banana_probability
        ? BANANA_ML_LABEL_BANANA
        : BANANA_ML_LABEL_NOT_BANANA;
    out_classification->attention_focus = banana_ml_clamp(
        attention_focus_sum / (double)input->token_count,
        0.0,
        1.0);
    return BANANA_OK;
}