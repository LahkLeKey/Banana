#ifndef BANANA_ENGINE_RUNTIME_INPUT_CLICK_POLICY_H
#define BANANA_ENGINE_RUNTIME_INPUT_CLICK_POLICY_H

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_input_click_policy_normalize(float click_x,
                                             float click_y,
                                             int input_width,
                                             int input_height,
                                             float *out_normalized_x,
                                             float *out_normalized_y);

#ifdef __cplusplus
}
#endif

#endif
