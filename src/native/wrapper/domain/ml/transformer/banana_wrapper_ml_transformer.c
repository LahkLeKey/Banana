#include "banana_wrapper_ml_transformer.h"
#include "banana_wrapper.h"
#include "domain/banana_ml_models.h"
#include "domain/ml/transformer/banana_ml_transformer.h"
#include "domain/ml/shared/banana_ml_shared.h"
#include "../shared/banana_wrapper_ml_json.h"

#include <stdint.h>
#include <string.h>

int banana_wrapper_ml_classify_transformer(const char* input_json, char** out_json) {
    BananaMlClassificationResult result;
    int rc;

    rc = banana_ml_classify_transformer(input_json, &result);
    if (rc != 0) {
        return rc;
    }

    return banana_wrapper_ml_result_to_json(&result, "transformer", out_json);
}

int banana_wrapper_ml_classify_transformer_quant_embedding(const char* input_json,
                                                             signed char* out_quant,
                                                             double* out_scale,
                                                             signed char* out_zero,
                                                             char** out_json) {
    BananaMlFeatureVector features;
    BananaMlClassificationResult result;
    BananaMlTransformerOptions options;
    double embedding[BANANA_ML_TRANSFORMER_EMBEDDING_DIM];
    int8_t quant[BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM];
    double scale = 0.0;
    int8_t zero = 0;
    int rc;
    size_t i;

    if (!input_json || !out_quant || !out_scale || !out_zero || !out_json) {
        return BANANA_INVALID_ARGUMENT;
    }

    memset(&result, 0, sizeof(result));
    memset(&options, 0, sizeof(options));
    options.log_attention_weights = 0;
    options.out_embedding = embedding;
    options.out_attention_weights = NULL;

    rc = banana_ml_build_feature_vector(input_json, &features);
    if (rc != BANANA_OK) {
        return rc;
    }

    rc = banana_ml_transformer_classify_ex(&features, &options, &result);
    if (rc != BANANA_OK) {
        return rc;
    }

    rc = banana_ml_transformer_embedding_quantize(embedding,
                                                    BANANA_ML_TRANSFORMER_EMBEDDING_DIM,
                                                    quant,
                                                    &scale,
                                                    &zero);
    if (rc != BANANA_OK) {
        return rc;
    }

    for (i = 0; i < BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM; ++i) {
        out_quant[i] = (signed char)quant[i];
    }
    *out_scale = scale;
    *out_zero = (signed char)zero;

    return banana_wrapper_ml_result_to_json(&result, "transformer", out_json);
}

int banana_wrapper_ml_classify_transformer_ex(const char* input_json,
                                                int log_attention,
                                                double* out_embedding,
                                                double* out_attention_weights,
                                                char** out_json) {
    BananaMlFeatureVector features;
    BananaMlClassificationResult result;
    BananaMlTransformerOptions options;
    int rc;

    if (!input_json || !out_json) {
        return BANANA_INVALID_ARGUMENT;
    }

    memset(&result, 0, sizeof(result));
    memset(&options, 0, sizeof(options));
    options.log_attention_weights = log_attention ? 1 : 0;
    options.out_embedding = out_embedding;
    options.out_attention_weights = out_attention_weights;

    rc = banana_ml_build_feature_vector(input_json, &features);
    if (rc != BANANA_OK) {
        return rc;
    }

    rc = banana_ml_transformer_classify_ex(&features, &options, &result);
    if (rc != BANANA_OK) {
        return rc;
    }

    return banana_wrapper_ml_result_to_json(&result, "transformer", out_json);
}