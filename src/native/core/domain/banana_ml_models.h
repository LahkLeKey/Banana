#ifndef BANANA_ML_MODELS_H
#define BANANA_ML_MODELS_H

#include "banana_domain_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_ML_FEATURE_COUNT 8
#define BANANA_ML_TOKEN_FEATURE_COUNT 4
#define BANANA_ML_MAX_SEQUENCE_LENGTH 16

typedef enum BananaMlLabel {
    BANANA_ML_LABEL_NOT_BANANA = 0,
    BANANA_ML_LABEL_BANANA = 1
} BananaMlLabel;

typedef struct BananaMlFeatureVector {
    double values[BANANA_ML_FEATURE_COUNT];
} BananaMlFeatureVector;

typedef struct BananaMlBinaryClassification {
    BananaMlLabel predicted_label;
    double banana_probability;
    double not_banana_probability;
    double decision_margin;
    double jaccard_similarity;
    double confusion_true_positive;
    double confusion_false_positive;
    double confusion_false_negative;
    double confusion_true_negative;
} BananaMlBinaryClassification;

typedef struct BananaMlToken {
    double values[BANANA_ML_TOKEN_FEATURE_COUNT];
} BananaMlToken;

typedef struct BananaMlTransformerInput {
    BananaMlToken tokens[BANANA_ML_MAX_SEQUENCE_LENGTH];
    int token_count;
} BananaMlTransformerInput;

typedef struct BananaMlTransformerClassification {
    BananaMlLabel predicted_label;
    double banana_probability;
    double not_banana_probability;
    double attention_focus;
} BananaMlTransformerClassification;

BananaStatus banana_ml_predict_regression_score(
    const BananaMlFeatureVector* features,
    double* out_score
);

BananaStatus banana_ml_predict_binary_classification(
    const BananaMlFeatureVector* features,
    BananaMlBinaryClassification* out_classification
);

BananaStatus banana_ml_predict_transformer_classification(
    const BananaMlTransformerInput* input,
    BananaMlTransformerClassification* out_classification
);

#ifdef __cplusplus
}
#endif

#endif