#include "banana_ml_models.h"

/* ============================================================
 * WEIGHT MATRICES (MODEL PARAMETERS)
 * ============================================================
 * These arrays represent learned model parameters.
 * In real ML systems, these would typically come from training.
 * Here they are hardcoded for deterministic inference.
 *
 * Think of these as "frozen knowledge" the model uses.
 * ============================================================ */

/*
 * Linear regression feature weights
 * Used in: banana_ml_predict_regression_score
 * Each feature contributes positively or negatively to the score.
 */
static const double k_regression_weights[BANANA_ML_FEATURE_COUNT] = {
    0.23, -0.19, 0.31, 0.11, -0.08, 0.17, 0.21, -0.14
};

/*
 * Binary classification weights
 * Used in: banana vs not-banana classification
 * Larger magnitude = stronger influence on decision boundary
 */
static const double k_binary_weights[BANANA_ML_FEATURE_COUNT] = {
    1.40, -1.20, 1.10, 0.90, -1.30, 0.80, 1.00, -1.10
};

/*
 * Transformer projection matrices
 * These simulate Query/Key/Value transformations.
 * Each token is projected into a different representation space.
 */
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

/*
 * Classification heads
 * These convert pooled transformer output into final logits.
 */
static const double k_transformer_banana_head[BANANA_ML_TOKEN_FEATURE_COUNT] = {
    1.25, -1.15, 1.10, -1.05
};

static const double k_transformer_not_banana_head[BANANA_ML_TOKEN_FEATURE_COUNT] = {
    -1.05, 1.20, -0.95, 1.15
};

/* ============================================================
 * BASIC UTILITY FUNCTIONS
 * ============================================================ */

/*
 * Validates numeric stability:
 * - Rejects NaN
 * - Rejects extreme values (simple safety clamp guard)
 */
static int banana_ml_value_is_valid(double value) {
    if (value != value) {
        return 0; // NaN check (NaN != NaN is true)
    }

    if (value > 1000000.0 || value < -1000000.0) {
        return 0; // prevent extreme numerical instability
    }

    return 1;
}

/* Absolute value helper */
static double banana_ml_absolute(double value) {
    return value < 0.0 ? -value : value;
}

/* Clamp value into [min, max] range */
static double banana_ml_clamp(double value, double min_value, double max_value) {
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

/*
 * Dot product:
 * Core operation in ML models (linear layers, attention, etc.)
 */
static double banana_ml_dot(const double* left, const double* right, int count) {
    double result = 0.0;

    for (int i = 0; i < count; i++) {
        result += left[i] * right[i];
    }

    return result;
}

/* ============================================================
 * FEATURE VALIDATION
 * ============================================================ */

/*
 * Ensures feature vector is safe for inference:
 * - non-null
 * - all values valid
 */
static BananaStatus banana_ml_validate_feature_vector(const BananaMlFeatureVector* features) {
    if (features == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (int i = 0; i < BANANA_ML_FEATURE_COUNT; i++) {
        if (!banana_ml_value_is_valid(features->values[i])) {
            return BANANA_ERROR_INVALID_INPUT;
        }
    }

    return BANANA_OK;
}

/* ============================================================
 * LINEAR MODEL CORE
 * ============================================================ */

/*
 * Computes:
 *   score = bias + dot(weights, features)
 */
static double banana_ml_linear_score(
    const double* weights,
    const BananaMlFeatureVector* features,
    double bias
) {
    return bias + banana_ml_dot(weights, features->values, BANANA_ML_FEATURE_COUNT);
}

/*
 * Lightweight sigmoid approximation.
 * Faster than exp-based sigmoid, good enough for toy ML.
 */
static double banana_ml_pseudo_sigmoid(double value) {
    double magnitude = banana_ml_absolute(value);
    return 0.5 + (value / (2.0 * (1.0 + magnitude)));
}

static double banana_ml_jaccard_similarity(double tp, double fp, double fn) {
    double denominator = tp + fp + fn;
    if (denominator <= 0.0) {
        return 0.0;
    }

    return banana_ml_clamp(tp / denominator, 0.0, 1.0);
}

static void banana_ml_fill_soft_confusion_matrix(
    double banana_probability,
    double not_banana_probability,
    BananaMlBinaryClassification* out_classification
) {
    double tp = banana_probability * banana_probability;
    double fp = banana_probability * not_banana_probability;
    double fn = not_banana_probability * banana_probability;
    double tn = not_banana_probability * not_banana_probability;

    out_classification->confusion_true_positive = tp;
    out_classification->confusion_false_positive = fp;
    out_classification->confusion_false_negative = fn;
    out_classification->confusion_true_negative = tn;
    out_classification->jaccard_similarity = banana_ml_jaccard_similarity(tp, fp, fn);
}

/* ============================================================
 * TRANSFORMER UTILITY
 * ============================================================ */

/*
 * Projects a token into a new feature space using a matrix.
 * This simulates transformer Q/K/V linear projections.
 */
static void banana_ml_project_token(
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

/* ============================================================
 * REGRESSION MODEL
 * ============================================================ */

/*
 * Predicts a bounded regression score [0, 1]
 */
BananaStatus banana_ml_predict_regression_score(
    const BananaMlFeatureVector* features,
    double* out_score
) {
    if (out_score == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    BananaStatus status = banana_ml_validate_feature_vector(features);
    if (status != BANANA_OK) {
        return status;
    }

    double score = banana_ml_linear_score(k_regression_weights, features, 0.12);

    // Ensure output stays in valid probability-like range
    *out_score = banana_ml_clamp(score, 0.0, 1.0);

    return BANANA_OK;
}

/* ============================================================
 * BINARY CLASSIFICATION MODEL
 * ============================================================ */

/*
 * Predicts:
 * - banana probability
 * - not-banana probability
 * - decision margin (raw score)
 */
BananaStatus banana_ml_predict_binary_classification(
    const BananaMlFeatureVector* features,
    BananaMlBinaryClassification* out_classification
) {
    if (out_classification == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    BananaStatus status = banana_ml_validate_feature_vector(features);
    if (status != BANANA_OK) {
        return status;
    }

    double margin = banana_ml_linear_score(k_binary_weights, features, -0.20);

    double probability = banana_ml_clamp(
        banana_ml_pseudo_sigmoid(margin),
        0.0,
        1.0
    );

    out_classification->predicted_label =
        probability >= 0.5 ? BANANA_ML_LABEL_BANANA : BANANA_ML_LABEL_NOT_BANANA;

    out_classification->banana_probability = probability;
    out_classification->not_banana_probability = 1.0 - probability;
    out_classification->decision_margin = margin;
    banana_ml_fill_soft_confusion_matrix(
        out_classification->banana_probability,
        out_classification->not_banana_probability,
        out_classification);

    return BANANA_OK;
}

/* ============================================================
 * TRANSFORMER CLASSIFICATION MODEL
 * ============================================================
 * This is a simplified transformer:
 * 1. Project tokens into Q/K/V spaces
 * 2. Compute attention between tokens
 * 3. Build weighted context vectors
 * 4. Pool across sequence
 * 5. Apply classification heads
 * ============================================================ */

BananaStatus banana_ml_predict_transformer_classification(
    const BananaMlTransformerInput* input,
    BananaMlTransformerClassification* out_classification
) {
    if (input == 0 || out_classification == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (input->token_count <= 0 ||
        input->token_count > BANANA_ML_MAX_SEQUENCE_LENGTH) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    /* Projected representations */
    double queries[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];
    double keys[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];
    double values[BANANA_ML_MAX_SEQUENCE_LENGTH][BANANA_ML_TOKEN_FEATURE_COUNT];

    double pooled_context[BANANA_ML_TOKEN_FEATURE_COUNT] = {0};

    double attention_focus_sum = 0.0;

    /* ========================================================
     * STEP 1: VALIDATE + PROJECT TOKENS
     * ======================================================== */
    for (int t = 0; t < input->token_count; t++) {

        for (int f = 0; f < BANANA_ML_TOKEN_FEATURE_COUNT; f++) {
            if (!banana_ml_value_is_valid(input->tokens[t].values[f])) {
                return BANANA_ERROR_INVALID_INPUT;
            }
        }

        banana_ml_project_token(k_transformer_query, &input->tokens[t], queries[t]);
        banana_ml_project_token(k_transformer_key,   &input->tokens[t], keys[t]);
        banana_ml_project_token(k_transformer_value, &input->tokens[t], values[t]);
    }

    /* ========================================================
     * STEP 2: SELF-ATTENTION
     * ======================================================== */
    for (int t = 0; t < input->token_count; t++) {

        double row_scores[BANANA_ML_MAX_SEQUENCE_LENGTH];
        double row_min = 0.0;
        double row_sum = 0.0;
        double row_focus = 0.0;
        double context[BANANA_ML_TOKEN_FEATURE_COUNT] = {0};

        /* Compute similarity scores (Q · K) */
        for (int o = 0; o < input->token_count; o++) {
            row_scores[o] = banana_ml_dot(
                queries[t],
                keys[o],
                BANANA_ML_TOKEN_FEATURE_COUNT
            ) * 0.5;

            if (o == 0 || row_scores[o] < row_min) {
                row_min = row_scores[o];
            }
        }

        /* Normalize scores (softmax-like stability trick) */
        for (int o = 0; o < input->token_count; o++) {
            row_scores[o] = (row_scores[o] - row_min) + 0.000001;
            row_sum += row_scores[o];
        }

        if (row_sum <= 0.0) {
            return BANANA_ERROR_INVALID_INPUT;
        }

        /* Build context vector */
        for (int o = 0; o < input->token_count; o++) {
            double attention_weight = row_scores[o] / row_sum;

            if (attention_weight > row_focus) {
                row_focus = attention_weight;
            }

            for (int f = 0; f < BANANA_ML_TOKEN_FEATURE_COUNT; f++) {
                context[f] += attention_weight * values[o][f];
            }
        }

        attention_focus_sum += row_focus;

        for (int f = 0; f < BANANA_ML_TOKEN_FEATURE_COUNT; f++) {
            pooled_context[f] += context[f];
        }
    }

    /* Average pooled representation */
    for (int f = 0; f < BANANA_ML_TOKEN_FEATURE_COUNT; f++) {
        pooled_context[f] /= (double)input->token_count;
    }

    /* ========================================================
     * STEP 3: CLASSIFICATION HEADS
     * ======================================================== */
    double banana_logit =
        0.12 + banana_ml_dot(k_transformer_banana_head, pooled_context,
                             BANANA_ML_TOKEN_FEATURE_COUNT);

    double not_banana_logit =
        0.05 + banana_ml_dot(k_transformer_not_banana_head, pooled_context,
                             BANANA_ML_TOKEN_FEATURE_COUNT);

    if (!banana_ml_value_is_valid(banana_logit) ||
        !banana_ml_value_is_valid(not_banana_logit)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    /* ========================================================
     * STEP 4: SOFTMAX NORMALIZATION
     * ======================================================== */
    double min_logit = banana_logit < not_banana_logit
        ? banana_logit
        : not_banana_logit;

    double banana_shifted = (banana_logit - min_logit) + 0.000001;
    double not_banana_shifted = (not_banana_logit - min_logit) + 0.000001;

    double denom = banana_shifted + not_banana_shifted;

    if (denom <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    /* ========================================================
     * STEP 5: OUTPUT PROBABILITIES
     * ======================================================== */
    out_classification->banana_probability = banana_shifted / denom;
    out_classification->not_banana_probability = not_banana_shifted / denom;

    out_classification->predicted_label =
        out_classification->banana_probability >=
        out_classification->not_banana_probability
            ? BANANA_ML_LABEL_BANANA
            : BANANA_ML_LABEL_NOT_BANANA;

    out_classification->attention_focus =
        banana_ml_clamp(
            attention_focus_sum / (double)input->token_count,
            0.0,
            1.0
        );

    return BANANA_OK;
}