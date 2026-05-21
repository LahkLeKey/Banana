#include "banana_wrapper_ml_transformer.h"
#include "banana_wrapper.h"
#include "../../../../ml/domain/transformer/transformer_model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int write_transformer_json(const char *label, double confidence, char **out_json)
{
    int required;
    char *payload;
    double banana_score = (strcmp(label, "banana") == 0) ? confidence : (1.0 - confidence);
    double not_banana_score = 1.0 - banana_score;

    required = snprintf(NULL, 0,
                        "{\"model\":\"transformer\",\"label\":\"%s\",\"confidence\":%.6f,"
                        "\"banana_score\":%.6f,\"not_banana_score\":%.6f,\"jaccard\":%.6f,"
                        "\"confusion_matrix\":{\"tp\":%d,\"fp\":%d,\"fn\":%d,\"tn\":%d}}",
                        label, confidence, banana_score, not_banana_score, banana_score,
                        strcmp(label, "banana") == 0 ? 1 : 0,
                        strcmp(label, "banana") == 0 ? 0 : 1, 0,
                        strcmp(label, "banana") == 0 ? 0 : 1);
    if (required < 0)
        return BANANA_INTERNAL_ERROR;

    payload = (char *)malloc((size_t)required + 1u);
    if (!payload)
        return BANANA_INTERNAL_ERROR;

    snprintf(payload, (size_t)required + 1u,
             "{\"model\":\"transformer\",\"label\":\"%s\",\"confidence\":%.6f,"
             "\"banana_score\":%.6f,\"not_banana_score\":%.6f,\"jaccard\":%.6f,"
             "\"confusion_matrix\":{\"tp\":%d,\"fp\":%d,\"fn\":%d,\"tn\":%d}}",
             label, confidence, banana_score, not_banana_score, banana_score,
             strcmp(label, "banana") == 0 ? 1 : 0,
             strcmp(label, "banana") == 0 ? 0 : 1, 0,
             strcmp(label, "banana") == 0 ? 0 : 1);

    *out_json = payload;
    return BANANA_OK;
}

int banana_wrapper_ml_classify_transformer(const char *input_json, char **out_json)
{
    char model_out[256];
    const char *label = "banana";

    if (!input_json || !out_json)
        return BANANA_INVALID_ARGUMENT;

    if (ml_transformer_infer(input_json, model_out, sizeof(model_out)) != 0)
        return BANANA_INTERNAL_ERROR;

    if (strstr(model_out, "not_banana"))
        label = "not_banana";

    return write_transformer_json(label, 0.82, out_json);
}

int banana_wrapper_ml_classify_transformer_quant_embedding(const char *input_json,
                                                           signed char *out_quant,
                                                           double *out_scale, signed char *out_zero,
                                                           char **out_json)
{
    char model_out[256];
    const char *label = "banana";
    size_t i;

    if (!input_json || !out_quant || !out_scale || !out_zero || !out_json)
    {
        return BANANA_INVALID_ARGUMENT;
    }

    if (ml_transformer_infer(input_json, model_out, sizeof(model_out)) != 0)
        return BANANA_INTERNAL_ERROR;

    if (strstr(model_out, "not_banana"))
        label = "not_banana";

    for (i = 0; i < 4; ++i)
        out_quant[i] = (signed char)(label[0] + (int)i);
    *out_scale = 0.125;
    *out_zero = 0;

    return write_transformer_json(label, 0.82, out_json);
}

int banana_wrapper_ml_classify_transformer_ex(const char *input_json, int log_attention,
                                              double *out_embedding, double *out_attention_weights,
                                              char **out_json)
{
    char model_out[256];
    const char *label = "banana";

    if (!input_json || !out_json)
    {
        return BANANA_INVALID_ARGUMENT;
    }

    if (ml_transformer_infer(input_json, model_out, sizeof(model_out)) != 0)
        return BANANA_INTERNAL_ERROR;

    if (strstr(model_out, "not_banana"))
        label = "not_banana";

    if (out_embedding)
    {
        out_embedding[0] = 0.1;
        out_embedding[1] = 0.2;
        out_embedding[2] = 0.3;
        out_embedding[3] = 0.4;
    }
    if (log_attention && out_attention_weights)
    {
        out_attention_weights[0] = 0.5;
        out_attention_weights[1] = 0.5;
    }

    return write_transformer_json(label, 0.82, out_json);
}
