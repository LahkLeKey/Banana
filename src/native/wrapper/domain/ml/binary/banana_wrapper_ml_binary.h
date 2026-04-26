#ifndef BANANA_WRAPPER_ML_BINARY_H
#define BANANA_WRAPPER_ML_BINARY_H

#ifdef __cplusplus
extern "C" {
#endif

int banana_wrapper_ml_classify_binary(const char* input_json, char** out_json);
int banana_wrapper_ml_classify_not_banana_junk(const char* input_json, char** out_json);

#ifdef __cplusplus
}
#endif

#endif