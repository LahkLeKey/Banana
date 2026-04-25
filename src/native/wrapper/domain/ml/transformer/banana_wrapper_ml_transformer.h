#ifndef BANANA_WRAPPER_ML_TRANSFORMER_H
#define BANANA_WRAPPER_ML_TRANSFORMER_H

#ifdef __cplusplus
extern "C" {
#endif

int banana_wrapper_ml_classify_transformer(const char* input_json, char** out_json);

#ifdef __cplusplus
}
#endif

#endif